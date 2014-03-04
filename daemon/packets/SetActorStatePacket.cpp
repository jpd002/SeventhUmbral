#include "SetActorStatePacket.h"

CSetActorStatePacket::CSetActorStatePacket()
{
	m_commandId = 0x134;
	m_packetSize = 0x28;
}

CSetActorStatePacket::~CSetActorStatePacket()
{

}

void CSetActorStatePacket::SetState(uint32 state)
{
	m_state = state;
}

PacketData CSetActorStatePacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	auto innerPacketData = result.data() + PACKET_HEADER_SIZE;
	reinterpret_cast<uint32*>(innerPacketData)[0] = m_state;
	return result;
}
