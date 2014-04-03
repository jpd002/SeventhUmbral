#include <assert.h>
#include <thread>
#include <boost/lexical_cast.hpp>
#include "LobbyServerPlayer.h"
#include "AppConfig.h"
#include "DatabaseConnectionManager.h"
#include "Log.h"
#include "../common/BLOWFISH.H"
#include "LobbyServer_Login.h"
#include "Character.h"
#include "Base64.h"
#include "GameServer.h"
#include "string_format.h"

#define LOG_NAME "LobbyServerPlayer"

#define PREF_FFXIVD_GAMESERVER_ADDRESS "ffxivd.gameserver.address"

CLobbyServerPlayer::CLobbyServerPlayer(SOCKET clientSocket)
: m_clientSocket(clientSocket)
, m_disconnect(false)
, m_userId(~0)
{
	m_dbConnection = CDatabaseConnectionManager::GetInstance().CreateConnection();
}

CLobbyServerPlayer::~CLobbyServerPlayer()
{

}

bool CLobbyServerPlayer::IsConnected() const
{
	return !m_disconnect;
}

void CLobbyServerPlayer::QueuePacket(const PacketData& packet)
{
	m_packetQueue.push_back(packet);
}

void CLobbyServerPlayer::Update()
{
	//Write to socket
	{
		int totalSent = 0;
		while(!m_packetQueue.empty())
		{
			if(totalSent >= 0x1000)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				break;
			}
			const auto& nextPacket = m_packetQueue.front();
			int sent = send(m_clientSocket, reinterpret_cast<const char*>(nextPacket.data()), nextPacket.size(), 0);
			assert(sent == nextPacket.size());
			m_packetQueue.pop_front();
			totalSent += sent;
		}
	}

	//Read from socket
	{
		static const unsigned int maxPacketSize = 0x10000;
		uint8 incomingPacket[maxPacketSize];
		int read = recv(m_clientSocket, reinterpret_cast<char*>(incomingPacket), maxPacketSize, 0);
		if(read == 0)
		{
			//Client disconnected
			CLog::GetInstance().LogMessage(LOG_NAME, "Client disconnected.");
			m_disconnect = true;
			return;
		}
		if(read > 0)
		{
			m_incomingStream.Write(incomingPacket, read);
		}
	}

	if(CPacketUtils::HasPacket(m_incomingStream))
	{
		auto incomingPacket = CPacketUtils::ReadPacket(m_incomingStream);
		if((incomingPacket.size() == 0x288) && (incomingPacket[0x44] == 'T'))		//Test Ticket Data
		{
			//Crypto handshake
			ProcessStartSession(incomingPacket);
		}
		else
		{
			auto subPackets = CPacketUtils::SplitPacket(incomingPacket);
			for(const auto& cryptedSubPacket : subPackets)
			{
				auto subPacket = CPacketUtils::DecryptSubPacket(cryptedSubPacket);
				uint16 commandId = CPacketUtils::GetSubPacketCommand(subPacket);
				switch(commandId)
				{
				case 0x03:
					ProcessGetCharacters(subPacket);
					break;
				case 0x04:
					ProcessSelectCharacter(subPacket);
					break;
				case 0x05:
					ProcessSessionAcknowledgement(subPacket);
					break;
				case 0x0B:
					//ModifyCharacter (used for add and delete?)
					CLog::GetInstance().LogDebug(LOG_NAME, "ModifyCharacter();");
					break;
				default:
					CLog::GetInstance().LogDebug(LOG_NAME, "Unknown command 0x%0.4X received.", commandId);
					break;
				}
			}
		}
	}
}

void CLobbyServerPlayer::ProcessStartSession(const PacketData& packetData)
{
	uint32 clientTime = *reinterpret_cast<const uint32*>(&packetData[0x84]);

	//We assume clientTime is 0x50E0E812, but we need to generate a proper key later
	uint8 blowfishKey[0x10] = { 0xB4, 0xEE, 0x3F, 0x6C, 0x01, 0x6F, 0x5B, 0xD9, 0x71, 0x50, 0x0D, 0xB1, 0x85, 0xA2, 0xAB, 0x43 };
	InitializeBlowfish(reinterpret_cast<char*>(blowfishKey), 0x10);

	CLog::GetInstance().LogMessage(LOG_NAME, "Received encryption key: 0x%0.8X.\r\n", clientTime);

	//Respond with acknowledgment
	std::vector<uint8> outgoingPacket(std::begin(g_secureConnectionAcknowledgment), std::end(g_secureConnectionAcknowledgment));
	CPacketUtils::EncryptPacket(outgoingPacket);
	QueuePacket(outgoingPacket);
}

void CLobbyServerPlayer::ProcessSessionAcknowledgement(const PacketData& packetData)
{
	std::string sessionId = reinterpret_cast<const char*>(&packetData[0x30]);
	std::string clientVersion = reinterpret_cast<const char*>(&packetData[0x70]);

	CLog::GetInstance().LogMessage(LOG_NAME, "Got acknowledgment for secure session.");
	CLog::GetInstance().LogMessage(LOG_NAME, "SESSION_ID: %s", sessionId.c_str());
	CLog::GetInstance().LogMessage(LOG_NAME, "CLIENT_VERSION: %s", clientVersion.c_str());

	if(m_dbConnection.IsEmpty())
	{
		CLog::GetInstance().LogMessage(LOG_NAME, "No database connection available. Bailing.");
		m_disconnect = true;
		return;
	}

	try
	{
		auto query = string_format("SELECT userId FROM ffxiv_sessions WHERE id = '%s' AND expiration > NOW()", sessionId.c_str());
		auto result = m_dbConnection.Query(query.c_str());
		if(result.GetRowCount() == 0)
		{
			throw std::runtime_error("Session expired or doesn't exist.");
		}
		auto row = result.FetchRow();
		assert(row != nullptr);
		assert(result.GetFieldCount() == 1);
		m_userId = boost::lexical_cast<uint32>(row[0]);
		CLog::GetInstance().LogMessage(LOG_NAME, "User (id: %u) logged in.", m_userId);
	}
	catch(const std::exception& exception)
	{
		CLog::GetInstance().LogError(LOG_NAME, "Failed to validate user session (id: %s): %s.", sessionId.c_str(), exception.what());
		m_disconnect = true;
		return;
	}

	std::vector<uint8> outgoingPacket(std::begin(g_loginAcknowledgment), std::end(g_loginAcknowledgment));
	CPacketUtils::EncryptPacket(outgoingPacket);
	QueuePacket(outgoingPacket);
}

void CLobbyServerPlayer::ProcessGetCharacters(const PacketData& packetData)
{
	CLog::GetInstance().LogMessage(LOG_NAME, "GetCharacters");

	if(m_dbConnection.IsEmpty())
	{
		CLog::GetInstance().LogMessage(LOG_NAME, "No database connection available. Bailing.");
		m_disconnect = true;
		return;
	}

	PacketData outgoingPacket(std::begin(g_characterListPacket), std::end(g_characterListPacket));

	CCharacter character;

	try
	{
		auto query = string_format("SELECT * FROM ffxiv_characters WHERE userId = %d", m_userId);
		auto result = m_dbConnection.Query(query.c_str());
		if(result.GetRowCount() != 0)
		{
			character = CCharacter(result);
		}
	}
	catch(const std::exception& exception)
	{
		CLog::GetInstance().LogError(LOG_NAME, "Failed to fetch characters for user (id = %d): %s", m_userId, exception.what());
		m_disconnect = true;
		return;
	}

	PacketData characterData(std::begin(g_characterData), std::end(g_characterData));
	
	characterData[0x21] = CCharacter::GetModelFromTribe(character.tribe);
	characterData[0x9F] = character.tribe;
	characterData[0xC7] = character.guardian;
	characterData[0xC8] = character.birthMonth;
	characterData[0xC9] = character.birthDay;

	*reinterpret_cast<uint32*>(&characterData[0x25]) = character.size;				//size

	*reinterpret_cast<uint32*>(&characterData[0x29]) = character.GetColorInfo();	//hairColor + skinColor
	*reinterpret_cast<uint32*>(&characterData[0x2D]) = character.GetFaceInfo();		//face Stuff?
	*reinterpret_cast<uint32*>(&characterData[0x31]) = character.hairStyle << 10;	//hair model
	*reinterpret_cast<uint32*>(&characterData[0x35]) = character.voice;				//voice
	*reinterpret_cast<uint32*>(&characterData[0x39]) = character.weapon1;			//weapon1
	*reinterpret_cast<uint32*>(&characterData[0x3D]) = character.weapon2;			//weapon2
	*reinterpret_cast<uint32*>(&characterData[0x55]) = character.headGear;			//headGear
	*reinterpret_cast<uint32*>(&characterData[0x59]) = character.bodyGear;			//bodyGear
	*reinterpret_cast<uint32*>(&characterData[0x5D]) = character.legsGear;			//legsGear
	*reinterpret_cast<uint32*>(&characterData[0x61]) = character.handsGear;			//handsGear
	*reinterpret_cast<uint32*>(&characterData[0x65]) = character.feetGear;			//feetGear
	*reinterpret_cast<uint32*>(&characterData[0x69]) = character.waistGear;			//waistGear
//	*reinterpret_cast<uint32*>(&characterData[0x6D]) = 0;							//???
	*reinterpret_cast<uint32*>(&characterData[0x71]) = character.rightEarGear;		//rightEarGear
	*reinterpret_cast<uint32*>(&characterData[0x75]) = character.leftEarGear;		//leftEarGear
//	*reinterpret_cast<uint32*>(&characterData[0x79]) = 0;							//???
//	*reinterpret_cast<uint32*>(&characterData[0x7D]) = 0;							//???
	*reinterpret_cast<uint32*>(&characterData[0x81]) = character.rightFingerGear;	//rightFingerGear
	*reinterpret_cast<uint32*>(&characterData[0x85]) = character.leftFingerGear;	//leftFingerGear

	auto encodedCharacterData = Framework::ToBase64(characterData.data(), characterData.size());
	std::replace(std::begin(encodedCharacterData), std::end(encodedCharacterData), '+', '-');
	std::replace(std::begin(encodedCharacterData), std::end(encodedCharacterData), '/', '_');

	static const uint32 characterInfoBase = 0x860;

	if(character.active)
	{
		for(unsigned int i = 0; i < encodedCharacterData.size(); i++)
		{
			outgoingPacket[characterInfoBase + 0x40 + i] = encodedCharacterData[i];
		}

		*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x00]) = 0x0158E7FC;
		*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x04]) = character.id;
		*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x0C]) = 0x000000F4;

		//Insert character name
		for(unsigned int i = 0; i < character.name.size(); i++)
		{
			outgoingPacket[characterInfoBase + 0x10 + i] = character.name[i];
		}
		outgoingPacket[characterInfoBase + 0x10 + character.name.size()] = 0;
	}

	CPacketUtils::EncryptPacket(outgoingPacket);
	QueuePacket(outgoingPacket);
}

void CLobbyServerPlayer::ProcessSelectCharacter(const PacketData& packetData)
{
	uint32 characterId = *reinterpret_cast<const uint32*>(&packetData[0x28]);

	CLog::GetInstance().LogMessage(LOG_NAME, "SelectCharacter(id = %d).", characterId);

	const char* gameServerAddress = CAppConfig::GetInstance().GetPreferenceString(PREF_FFXIVD_GAMESERVER_ADDRESS);

	std::vector<uint8> outgoingPacket(std::begin(g_selectCharacterPacket), std::end(g_selectCharacterPacket));
	*reinterpret_cast<uint32*>(outgoingPacket.data() + 0x38) = characterId;			//Character Id (normally the actor id which is different from the id passed in that packet)
	*reinterpret_cast<uint16*>(outgoingPacket.data() + 0x86) = CGameServer::GAME_SERVER_PORT;
	strcpy(reinterpret_cast<char*>(outgoingPacket.data() + 0x88), gameServerAddress);

	CPacketUtils::EncryptPacket(outgoingPacket);
	QueuePacket(outgoingPacket);
}
