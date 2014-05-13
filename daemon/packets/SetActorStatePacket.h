#pragma once

#include "BasePacket.h"

class CSetActorStatePacket : public CBasePacket
{
public:
	enum STATE_ID
	{
		STATE_NONE		= 0x0000,
		STATE_DEAD		= 0x0303,
		STATE_PASSIVE	= 0xBF00,
		STATE_ACTIVE	= 0xBF02,
	};
							CSetActorStatePacket();
	virtual					~CSetActorStatePacket();

	virtual PacketPtr		Clone() const override;

	void					SetState(uint32);

	virtual PacketData		ToPacketData() const override;

private:
	uint32					m_state = 0;
};
