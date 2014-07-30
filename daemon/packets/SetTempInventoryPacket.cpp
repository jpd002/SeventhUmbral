#include <assert.h>
#include "SetTempInventoryPacket.h"

CSetTempInventoryPacket::CSetTempInventoryPacket()
{
	m_commandId = 0x149;
	m_packetSize = 0;
}

CSetTempInventoryPacket::~CSetTempInventoryPacket()
{

}

PacketPtr CSetTempInventoryPacket::Clone() const
{
	return std::make_shared<CSetTempInventoryPacket>(*this);
}

void CSetTempInventoryPacket::SetItemCount(unsigned int itemCount)
{
	m_itemCount = itemCount;
	m_packetSize = (MAX_ITEMS * ITEM_DEF_SIZE) + PACKET_HEADER_SIZE + 8;
}

void CSetTempInventoryPacket::SetItemIndex(unsigned int itemIndex, uint32 itemInventoryIndex)
{
	assert(itemIndex < MAX_ITEMS);
	if(itemIndex >= MAX_ITEMS) return;
	m_items[itemIndex].index = itemInventoryIndex;
}

void CSetTempInventoryPacket::SetItemId(unsigned int itemIndex, uint32 itemId)
{
	assert(itemIndex < MAX_ITEMS);
	if(itemIndex >= MAX_ITEMS) return;
	m_items[itemIndex].id = itemId;
}

void CSetTempInventoryPacket::SetItemDefinitionId(unsigned int itemIndex, uint32 itemDefinitionId)
{
	assert(itemIndex < MAX_ITEMS);
	if(itemIndex >= MAX_ITEMS) return;
	m_items[itemIndex].definitionId = itemDefinitionId;
}

PacketData CSetTempInventoryPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();

	for(unsigned int i = 0; i < m_itemCount; i++)
	{
		const auto& srcItemBlock(m_items[i]);
		auto itemBlockPtr = result.data() + PACKET_HEADER_SIZE + (i * ITEM_DEF_SIZE);
		memcpy(itemBlockPtr, &srcItemBlock, ITEM_DEF_SIZE);
	}
	*reinterpret_cast<uint32*>(result.data() + PACKET_HEADER_SIZE + (MAX_ITEMS * ITEM_DEF_SIZE)) = m_itemCount;

	return result;
}
