#pragma once

#include <deque>
#include "SocketDef.h"
#include "Types.h"
#include "MemStream.h"
#include "PacketUtils.h"
#include "mysql/Client.h"

class CLobbyServerPlayer
{
public:
								CLobbyServerPlayer(SOCKET);
	virtual						~CLobbyServerPlayer();

	bool						IsConnected() const;
	void						Update();

private:
	typedef std::deque<PacketData> PacketQueue;

	void						QueuePacket(const PacketData&);

	void						ProcessStartSession(const PacketData&);
	void						ProcessSessionAcknowledgement(const PacketData&);
	void						ProcessSelectCharacter(const PacketData&);
	void						ProcessGetCharacters(const PacketData&);

	SOCKET						m_clientSocket;
	Framework::CMemStream		m_incomingStream;
	PacketQueue					m_packetQueue;
	bool						m_disconnect;
	uint32						m_userId;
	Framework::MySql::CClient	m_dbConnection;
};
