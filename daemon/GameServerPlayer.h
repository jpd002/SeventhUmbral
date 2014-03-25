#pragma once

#include <deque>
#include <map>
#include "SocketDef.h"
#include "Types.h"
#include "MemStream.h"
#include "PacketUtils.h"
#include "packets/CompositePacket.h"
#include "mysql/Client.h"

class CGameServerPlayer
{
public:
								CGameServerPlayer(SOCKET);
	virtual						~CGameServerPlayer();

	bool						IsConnected() const;
	void						Update();

private:
	enum
	{
		EMPTY_LOCKON_ID = 0xE0000000,
	};

	typedef std::deque<PacketData> PacketQueue;

	void						QueuePacket(const PacketData&);

	void						PrepareInitialPackets();

	void						ProcessInitialHandshake(unsigned int, const PacketData&);
	void						ProcessKeepAlive(const PacketData&);
	void						ProcessChat(const PacketData&);
	void						ProcessSetPlayerPosition(const PacketData&);
	void						ProcessSetSelection(const PacketData&);
	void						ProcessScriptCommand(const PacketData&);
	void						ProcessScriptResult(const PacketData&);
	void						ProcessDamageToNpc(CCompositePacket&, uint32, uint32);

	void						ScriptCommand_EquipItem(const PacketData&, uint32);
	void						ScriptCommand_Emote(const PacketData&, uint32);
	void						ScriptCommand_TrashItem(const PacketData&, uint32);
	void						ScriptCommand_SwitchToActiveMode(CCompositePacket&);
	void						ScriptCommand_SwitchToPassiveMode(CCompositePacket&);
	void						ScriptCommand_BattleSkill(CCompositePacket&, uint32, uint32, uint32);

	PacketData					GetCharacterInfo();
	static PacketData			SpawnNpc(uint32, uint32, uint32, float, float, float, float);
	void						SendTeleportSequence(uint32, uint32, float, float, float, float);

	SOCKET						m_clientSocket;
	Framework::CMemStream		m_incomingStream;
	PacketQueue					m_packetQueue;
	bool						m_disconnect;
	Framework::MySql::CClient	m_dbConnection;
	uint32						m_characterId = 0;
	bool						m_sentInitialHandshake = false;
	bool						m_isActiveMode = false;
	int							m_playerAutoAttackTimer = 0;
	int							m_enemyAutoAttackTimer = 0;
	uint32						m_lockOnId = EMPTY_LOCKON_ID;
	bool						m_alreadyMovedOutOfRoom;
	bool						m_zoneMasterCreated;
	std::map<uint32, int32>		m_npcHp;
};
