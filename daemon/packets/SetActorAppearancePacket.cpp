#include <assert.h>
#include "SetActorAppearancePacket.h"

CSetActorAppearancePacket::CSetActorAppearancePacket()
{
	static const uint32 packetSize = 0x128;
	m_commandId = 0xD6;
	m_packetSize = packetSize;
	static_assert(sizeof(m_appearanceData) + PACKET_HEADER_SIZE == packetSize, "Wrong packet size");
	memset(&m_appearanceData, 0, sizeof(m_appearanceData));
	for(unsigned int i = 0; i < APPEARANCE_ITEM_COUNT; i++)
	{
		m_appearanceData[(i * 2) + 1] = i;
	}
	m_appearanceData[0x40] = APPEARANCE_ITEM_COUNT;
}

CSetActorAppearancePacket::~CSetActorAppearancePacket()
{

}

PacketPtr CSetActorAppearancePacket::Clone() const
{
	return std::make_shared<CSetActorAppearancePacket>(*this);
}

void CSetActorAppearancePacket::SetAppearanceItem(unsigned int index, uint32 value)
{
	assert(index < APPEARANCE_ITEM_COUNT);
	if(index >= APPEARANCE_ITEM_COUNT)
	{
		return;
	}
	m_appearanceData[index * 2] = value;
}

PacketData CSetActorAppearancePacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	memcpy(result.data() + PACKET_HEADER_SIZE, &m_appearanceData, sizeof(m_appearanceData));
	return result;
}
