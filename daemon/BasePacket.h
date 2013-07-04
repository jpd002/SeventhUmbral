#pragma once

#include "PacketUtils.h"

class CBasePacket
{
public:
	enum 
	{
		MIN_PACKET_SIZE = 0x10,
	};

							CBasePacket();
	virtual					~CBasePacket();

	void					SetSourceId(uint32);
	void					SetTargetId(uint32);

	virtual PacketData		ToPacketData() const;

protected:
	uint16					m_packetSize;
	uint32					m_sourceId;
	uint32					m_targetId;
	uint16					m_commandId;
};
