#include <WinSock2.h>
#include <assert.h>
#include "GameServer.h"
#include "PacketUtils.h"
#include "GameServerPlayer.h"
#include "GameServer_Login.h"

#define LOGNAME		("GameServer")

static void SendPacket(SOCKET clientSocket, const void* packet, size_t size)
{
	int sent = send(clientSocket, reinterpret_cast<const char*>(packet), size, 0);
	assert(sent == size);
}

static void UpdateEntities(SOCKET clientSocket)
{
	static int entityUpdateCounter = 0;

	if(entityUpdateCounter >= 5)
	{
		entityUpdateCounter = 0;

		uint8 entityStatusPacket[] =
		{
			0x01, 0x00, 0x00, 0x00, 0x60, 0x00, 0x1D, 0x00, 0x75, 0x1C, 0x8B, 0xEE, 0x3B, 0x01, 0x00, 0x00,
			0x50, 0x00, 0x03, 0x00, 0x06, 0x00, 0xa0, 0x47, 0x41, 0x29, 0x9b, 0x02, 0x00, 0xe0, 0xd2, 0xfe,
			0x14, 0x00, 0xcf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x60, 0xe1, 0x50, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xd7, 0x1f, 0x43, 0x33, 0x33, 0x33, 0x3f,
			0x85, 0x2b, 0x27, 0x43, 0xcd, 0xcc, 0x4c, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		};

		float itemPosX = *reinterpret_cast<float*>(&entityStatusPacket[0x38]);
		float itemPosY = *reinterpret_cast<float*>(&entityStatusPacket[0x3C]);
		float itemPosZ = *reinterpret_cast<float*>(&entityStatusPacket[0x40]);

		static float someAlpha = 0;
		itemPosX += 5.f * sin(someAlpha);
		itemPosZ += 5.f * cos(someAlpha);
		someAlpha += 0.1f;

		*reinterpret_cast<float*>(&entityStatusPacket[0x38]) = itemPosX;
		*reinterpret_cast<float*>(&entityStatusPacket[0x3C]) = itemPosY;
		*reinterpret_cast<float*>(&entityStatusPacket[0x40]) = itemPosZ;


		//Send entity status
		{
			std::vector<uint8> outgoingPacket(std::begin(entityStatusPacket), std::end(entityStatusPacket));
			int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
			assert(sent == outgoingPacket.size());
		}
	}

	entityUpdateCounter++;
}

static void ClientThreadProc(SOCKET clientSocket, int clientId)
{
	u_long notBlockingMode = 1;
	ioctlsocket(clientSocket, FIONBIO, &notBlockingMode);

	printf("%s: Received connection.\r\n", LOGNAME);

	if(clientId == 0)
	{
		CGameServerPlayer player(clientSocket);
		while(player.IsConnected())
		{
			player.Update();
		}
	}
	else
	{
		{
			std::vector<uint8> outgoingPacket(std::begin(g_client1_login1), std::end(g_client1_login1));
			int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
			assert(sent == outgoingPacket.size());
		}

		{
			std::vector<uint8> outgoingPacket(std::begin(g_client1_login2), std::end(g_client1_login2));
			int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
			assert(sent == outgoingPacket.size());
		}

		while(1)
		{
			static const unsigned int maxPacketSize = 0x10000;
			uint8 incomingPacket[maxPacketSize];
			int read = recv(clientSocket, reinterpret_cast<char*>(incomingPacket), maxPacketSize, 0);
			if(read == 0)
			{
				//Client disconnected
				printf("%s: Client disconnected.\r\n", LOGNAME);
				break;
			}
			Sleep(100);
		}
	}

	closesocket(clientSocket);
}

CGameServer::CGameServer()
{

}

CGameServer::~CGameServer()
{

}

void CGameServer::Start()
{
	std::thread serverThread(std::bind(&CGameServer::ServerThreadProc, this));
	m_serverThread = std::move(serverThread);
}

void CGameServer::ServerThreadProc()
{
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in service;
	service.sin_family			= AF_INET;
	service.sin_addr.s_addr		= inet_addr("127.0.0.1");
	service.sin_port			= htons(GAME_SERVER_PORT);
	if(bind(listenSocket, reinterpret_cast<sockaddr*>(&service), sizeof(sockaddr_in)))
	{
		printf("Failed to bind socket.\r\n");
		return;
	}

	if(listen(listenSocket, SOMAXCONN))
	{
		printf("Failed to listen on socket.\r\n");
		return;
	}

	printf("Game server started.\r\n");

	int clientId = 0;

	while(1)
	{
		sockaddr_in incomingAddr;
		int incomingAddrSize = sizeof(sockaddr_in);
		SOCKET incomingSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&incomingAddr), &incomingAddrSize);
		std::thread clientThread(std::bind(&ClientThreadProc, incomingSocket, clientId & 1));
		clientThread.detach();
		clientId++;
	}

	closesocket(listenSocket);
}
