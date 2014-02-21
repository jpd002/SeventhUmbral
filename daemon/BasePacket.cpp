#include "BasePacket.h"
#include <assert.h>

CBasePacket::CBasePacket()
: m_packetSize(PACKET_HEADER_SIZE)
, m_sourceId(0)
, m_targetId(0)
, m_commandId(0)
{

}

CBasePacket::~CBasePacket()
{

}

void CBasePacket::SetSourceId(uint32 sourceId)
{
	m_sourceId = sourceId;
}

void CBasePacket::SetTargetId(uint32 targetId)
{
	m_targetId = targetId;
}

PacketData CBasePacket::ToPacketData() const
{
	assert(m_packetSize >= PACKET_HEADER_SIZE);

	PacketData result;
//	result.resize(m_packetSize + 0x10);
	result.resize(m_packetSize);

	//Write packet header
/*
	result[0x00] = 0x01;
	*reinterpret_cast<uint16*>(result.data() + 0x04) = m_packetSize + 0x10;
	*reinterpret_cast<uint16*>(result.data() + 0x06) = 0x1D;		//Unknown
	*reinterpret_cast<uint32*>(result.data() + 0x08) = 0xEE8B1C75;	//Unknown
	*reinterpret_cast<uint32*>(result.data() + 0x0C) = 0x13B;		//Unknown
*/
	//Write subpacket header
	*reinterpret_cast<uint16*>(result.data() + 0x00) = m_packetSize;
	*reinterpret_cast<uint16*>(result.data() + 0x02) = 3;			//Unknown
	*reinterpret_cast<uint32*>(result.data() + 0x04) = m_sourceId;
	*reinterpret_cast<uint32*>(result.data() + 0x08) = m_targetId;
	*reinterpret_cast<uint32*>(result.data() + 0x0C) = 0xFED2E000;	//Unknown

	//0x14, 0x00, 0x7B, 0x01, 0x00, 0x00, 0x00, 0x00, 0xDB, 0xE9, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00,
	*reinterpret_cast<uint16*>(result.data() + 0x10) = 0x14;		//Unknown
	*reinterpret_cast<uint16*>(result.data() + 0x12) = m_commandId;
	*reinterpret_cast<uint32*>(result.data() + 0x14) = 0;
	*reinterpret_cast<uint32*>(result.data() + 0x18) = 0x50E0E838;	//Time?
	*reinterpret_cast<uint32*>(result.data() + 0x1C) = 0;

	return result;
}
