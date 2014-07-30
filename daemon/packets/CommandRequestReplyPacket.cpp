#include "CommandRequestReplyPacket.h"

CCommandRequestReplyPacket::CCommandRequestReplyPacket()
{
	m_commandId = 0xE1;
	m_packetSize = 0x30;
}

CCommandRequestReplyPacket::~CCommandRequestReplyPacket()
{

}

void CCommandRequestReplyPacket::SetAnimationId(uint32 animationId)
{
	m_info.animationId = animationId;
}

void CCommandRequestReplyPacket::SetActorId(uint32 actorId)
{
	m_info.actorId = actorId;
}

void CCommandRequestReplyPacket::SetDescriptionId(uint32 descriptionId)
{
	m_info.descriptionId = descriptionId;
}

PacketPtr CCommandRequestReplyPacket::Clone() const
{
	return std::make_shared<CCommandRequestReplyPacket>(*this);
}

PacketData CCommandRequestReplyPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	memcpy(result.data() + PACKET_HEADER_SIZE, &m_info, sizeof(PACKET_INFO));
	return result;
}
