#pragma once

#include "BasePacket.h"

class CSetInventoryPacket : public CBasePacket
{
public:
							CSetInventoryPacket();
	virtual					~CSetInventoryPacket();

	void					SetItemBase(unsigned int);
	void					SetItemCount(unsigned int);

	virtual PacketData		ToPacketData() const override;

private:
	unsigned int			m_itemBase = 0;
	unsigned int			m_itemCount = 0;
};
