#pragma once

#include <deque>
#include <map>
#include "SocketDef.h"
#include "Types.h"
#include "MemStream.h"
#include "PacketUtils.h"
#include "packets/CompositePacket.h"
#include "mysql/Client.h"
#include "Instance.h"

class CGameServerPlayer
{
public:
								CGameServerPlayer(SOCKET);
	virtual						~CGameServerPlayer();

	bool						IsConnected() const;
	void						Update();

private:
	typedef std::deque<PacketData> PacketQueue;

	void						QueuePacket(const PacketData&);
	void						QueueToCurrentComposite(CActor*, const PacketPtr&);

	void						PrepareInitialPackets();

	void						ProcessInitialHandshake(unsigned int, const PacketData&);
	void						ProcessKeepAlive(const PacketData&);
	void						ProcessChat(const PacketData&);
	void						ProcessSetPlayerPosition(const PacketData&);
	void						ProcessSetSelection(const PacketData&);
	void						ProcessScriptCommand(const PacketData&);
	void						ProcessScriptResult(const PacketData&);

	void						ScriptCommand_EquipItem(const PacketData&, uint32);
	void						ScriptCommand_Emote(const PacketData&, uint32);
	void						ScriptCommand_TrashItem(const PacketData&, uint32);

	PacketData					GetCharacterInfo();
	void						ResetInstance();
	void						SpawnNpc(uint32, uint32, uint32, float, float, float, float);
	void						SendTeleportSequence(uint32, uint32, float, float, float, float);

	SOCKET						m_clientSocket = 0;
	Framework::CMemStream		m_incomingStream;
	PacketQueue					m_packetQueue;
	bool						m_disconnect = false;
	Framework::MySql::CClient	m_dbConnection;
	bool						m_sentInitialHandshake = false;
	bool						m_alreadyMovedOutOfRoom = false;
	bool						m_zoneMasterCreated = false;
	uint32						m_characterId = 0;

	CInstance					m_instance;
	CCompositePacket			m_currentComposite;
};
