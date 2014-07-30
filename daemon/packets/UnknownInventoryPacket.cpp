#include "UnknownInventoryPacket.h"

////////////////////////////////////////////////////////////////////////
//Packet 0x0146
////////////////////////////////////////////////////////////////////////

CUnknownInventoryPacket_0146::CUnknownInventoryPacket_0146()
{
	m_commandId = 0x146;
	m_packetSize = 0x28;
}

CUnknownInventoryPacket_0146::~CUnknownInventoryPacket_0146()
{

}

void CUnknownInventoryPacket_0146::SetActorId(uint32 actorId)
{
	m_info.actorId = actorId;
}

void CUnknownInventoryPacket_0146::SetUnknown0(uint16 unknown0)
{
	m_info.unknown0 = unknown0;
}

void CUnknownInventoryPacket_0146::SetUnknown1(uint16 unknown1)
{
	m_info.unknown1 = unknown1;
}

PacketPtr CUnknownInventoryPacket_0146::Clone() const
{
	return std::make_shared<CUnknownInventoryPacket_0146>(*this);
}

PacketData CUnknownInventoryPacket_0146::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	memcpy(result.data() + PACKET_HEADER_SIZE, &m_info, sizeof(PACKET_INFO));
	return result;
}

////////////////////////////////////////////////////////////////////////
//Packet 0x0147
////////////////////////////////////////////////////////////////////////

CUnknownInventoryPacket_0147::CUnknownInventoryPacket_0147()
{
	m_commandId = 0x147;
	m_packetSize = 0x28;
}

CUnknownInventoryPacket_0147::~CUnknownInventoryPacket_0147()
{

}

PacketPtr CUnknownInventoryPacket_0147::Clone() const
{
	return std::make_shared<CUnknownInventoryPacket_0147>(*this);
}

PacketData CUnknownInventoryPacket_0147::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	memcpy(result.data() + PACKET_HEADER_SIZE, &m_info, sizeof(PACKET_INFO));
	return result;
}

////////////////////////////////////////////////////////////////////////
//Packet 0x016D
////////////////////////////////////////////////////////////////////////

CUnknownInventoryPacket_016D::CUnknownInventoryPacket_016D()
{
	m_commandId = 0x16D;
	m_packetSize = 0x28;
}

CUnknownInventoryPacket_016D::~CUnknownInventoryPacket_016D()
{

}

PacketPtr CUnknownInventoryPacket_016D::Clone() const
{
	return std::make_shared<CUnknownInventoryPacket_016D>(*this);
}

PacketData CUnknownInventoryPacket_016D::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	memcpy(result.data() + PACKET_HEADER_SIZE, &m_info, sizeof(PACKET_INFO));
	return result;
}

////////////////////////////////////////////////////////////////////////
//Packet 0x016E
////////////////////////////////////////////////////////////////////////

CUnknownInventoryPacket_016E::CUnknownInventoryPacket_016E()
{
	m_commandId = 0x16E;
	m_packetSize = 0x28;
}

CUnknownInventoryPacket_016E::~CUnknownInventoryPacket_016E()
{

}

PacketPtr CUnknownInventoryPacket_016E::Clone() const
{
	return std::make_shared<CUnknownInventoryPacket_016E>(*this);
}

PacketData CUnknownInventoryPacket_016E::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	memcpy(result.data() + PACKET_HEADER_SIZE, &m_info, sizeof(PACKET_INFO));
	return result;
}
