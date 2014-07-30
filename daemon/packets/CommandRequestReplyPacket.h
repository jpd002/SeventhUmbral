#pragma once

#include "BasePacket.h"

class CCommandRequestReplyPacket : public CBasePacket
{
public:
							CCommandRequestReplyPacket();
	virtual					~CCommandRequestReplyPacket();

	void					SetAnimationId(uint32);
	void					SetActorId(uint32);
	void					SetDescriptionId(uint32);

	virtual PacketPtr		Clone() const override;
	virtual PacketData		ToPacketData() const override;

private:
	struct PACKET_INFO
	{
		uint32	animationId = 0;
		uint32	actorId = 0;
		uint32	descriptionId = 0;
		uint32	unknown = 0;
	};
	static_assert(sizeof(PACKET_INFO) == 0x10, "PACKET_INFO must be 16 bytes.");

	PACKET_INFO				m_info;
};
