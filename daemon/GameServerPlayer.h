#pragma once

#include <deque>
#include <WinSock2.h>
#include "Types.h"
#include "MemStream.h"
#include "PacketUtils.h"

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

	void						PrepareInitialPackets();

	void						ProcessKeepAlive(const PacketData&);
	void						ProcessSetPlayerPosition(const PacketData&);
	void						ProcessScriptCommand(const PacketData&);
	void						ProcessScriptResult(const PacketData&);

	SOCKET						m_clientSocket;
	Framework::CMemStream		m_incomingStream;
	PacketQueue					m_packetQueue;
	bool						m_disconnect;
	bool						m_alreadyMovedOutOfRoom;
};
