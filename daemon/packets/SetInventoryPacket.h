#pragma once

#include "BasePacket.h"
#include "ItemBlock.h"

class CSetInventoryPacket : public CBasePacket
{
public:
							CSetInventoryPacket();
	virtual					~CSetInventoryPacket();

	virtual PacketPtr		Clone() const override;

	void					SetItemCount(unsigned int);

	void					SetItemIndex(unsigned int, uint32);
	void					SetItemId(unsigned int, uint32);
	void					SetItemDefinitionId(unsigned int, uint32);

	virtual PacketData		ToPacketData() const override;

private:
	enum
	{
		ITEM_DEF_SIZE = sizeof(ITEMBLOCK),
		MAX_ITEMS = 0x20,
	};

	unsigned int			m_itemCount = 0;
	ITEMBLOCK				m_items[MAX_ITEMS];
};
