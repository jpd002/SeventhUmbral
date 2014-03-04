#include <assert.h>
#include "SetActorPropertyPacket.h"

CSetActorPropertyPacket::CSetActorPropertyPacket()
{
	m_commandId = 0x137;
	m_packetSize = 0xA8;
	m_modificationStream.Allocate(m_packetSize - PACKET_HEADER_SIZE - 1);
	memset(m_modificationStream.GetBuffer(), 0, m_modificationStream.GetSize());
}

CSetActorPropertyPacket::~CSetActorPropertyPacket()
{

}

void CSetActorPropertyPacket::AddSetShort(uint32 valueId, uint16 value)
{
	m_modificationStream.Write8(0x02);
	m_modificationStream.Write32(valueId);
	m_modificationStream.Write16(value);
}

void CSetActorPropertyPacket::AddTargetProperty(const std::string& name)
{
	//Why 0x82?
	m_modificationStream.Write8(0x82 + name.length());
	m_modificationStream.Write(name.c_str(), name.length());
}

PacketData CSetActorPropertyPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	auto innerPacketData = result.data() + PACKET_HEADER_SIZE;
	assert(m_modificationStream.GetSize() == (m_packetSize - PACKET_HEADER_SIZE - 1));
	innerPacketData[0x00] = const_cast<Framework::CMemStream&>(m_modificationStream).Tell();
	memcpy(innerPacketData + 0x01, m_modificationStream.GetBuffer(), m_modificationStream.GetSize());
	return result;
}
