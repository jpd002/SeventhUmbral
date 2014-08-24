#include "KeepAlivePacket.h"

CKeepAlivePacket::CKeepAlivePacket()
{
	m_commandId = 0x01;
	m_packetSize = 0x40;
}

CKeepAlivePacket::~CKeepAlivePacket()
{

}

PacketPtr CKeepAlivePacket::Clone() const
{
	return std::make_shared<CKeepAlivePacket>(*this);
}

PacketData CKeepAlivePacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	memcpy(result.data() + PACKET_HEADER_SIZE, &m_info, sizeof(KEEP_ALIVE_INFO));
	return result;
}
