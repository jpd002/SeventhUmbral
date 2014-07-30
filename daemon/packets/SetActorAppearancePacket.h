#pragma once

#include "BasePacket.h"

class CSetActorAppearancePacket : public CBasePacket
{
public:
							CSetActorAppearancePacket();
	virtual					~CSetActorAppearancePacket();

	virtual PacketPtr		Clone() const override;

	void					SetAppearanceItem(unsigned int, uint32);

	virtual PacketData		ToPacketData() const override;

private:
	enum
	{
		APPEARANCE_ITEM_COUNT = 0x1C
	};

	uint32					m_appearanceData[0x42];
};
