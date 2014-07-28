#include <assert.h>
#include "SetInventoryPacket.h"

CSetInventoryPacket::CSetInventoryPacket()
{
	m_commandId = 0x14B;
	m_packetSize = 0;
}

CSetInventoryPacket::~CSetInventoryPacket()
{

}

PacketPtr CSetInventoryPacket::Clone() const
{
	return std::make_shared<CSetInventoryPacket>(*this);
}

void CSetInventoryPacket::SetItemCount(unsigned int itemCount)
{
	m_itemCount = itemCount;
	m_packetSize = (itemCount * ITEM_DEF_SIZE) + PACKET_HEADER_SIZE;
}

void CSetInventoryPacket::SetItemIndex(unsigned int itemIndex, uint32 itemInventoryIndex)
{
	assert(itemIndex < MAX_ITEMS);
	if(itemIndex >= MAX_ITEMS) return;
	m_items[itemIndex].index = itemInventoryIndex;
}

void CSetInventoryPacket::SetItemId(unsigned int itemIndex, uint32 itemId)
{
	assert(itemIndex < MAX_ITEMS);
	if(itemIndex >= MAX_ITEMS) return;
	m_items[itemIndex].id = itemId;
}

void CSetInventoryPacket::SetItemDefinitionId(unsigned int itemIndex, uint32 itemDefinitionId)
{
	assert(itemIndex < MAX_ITEMS);
	if(itemIndex >= MAX_ITEMS) return;
	m_items[itemIndex].definitionId = itemDefinitionId;
}

PacketData CSetInventoryPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();

	for(unsigned int i = 0; i < m_itemCount; i++)
	{
		const auto& srcItemBlock(m_items[i]);
		auto itemBlockPtr = result.data() + PACKET_HEADER_SIZE + (i * ITEM_DEF_SIZE);
		memcpy(itemBlockPtr, &srcItemBlock, ITEM_DEF_SIZE);
	}

	return result;
}
