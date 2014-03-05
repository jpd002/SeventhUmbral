#include "DisplayMessagePacket.h"

CDisplayMessagePacket::CDisplayMessagePacket()
{
	m_commandId = 0x03;
	m_packetSize = 0x248;
}

CDisplayMessagePacket::~CDisplayMessagePacket()
{

}

void CDisplayMessagePacket::SetMessage(const std::string& message)
{
	m_message = message;
}

PacketData CDisplayMessagePacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	auto innerPacketData = result.data() + PACKET_HEADER_SIZE;
	*reinterpret_cast<uint32*>(innerPacketData + 0x20) = 0x1D;		//Seems to change the color (0x1C = red)
	memcpy(innerPacketData + 0x24, m_message.c_str(), m_message.size());
	return result;
}
