#pragma once

#include "BasePacket.h"

class CBattleActionPacket : public CBasePacket
{
public:
							CBattleActionPacket();
	virtual					~CBattleActionPacket();

	void					SetActionSourceId(uint32);
	void					SetAnimationId(uint32);
	void					SetDescriptionId(uint32);
	void					SetActionTargetId(uint32);
	void					SetDamage(uint16);
	void					SetDamageType(uint16);
	void					SetFeedbackId(uint32);
	void					SetAttackSide(uint32);

	virtual PacketData		ToPacketData() const override;

private:
	struct BATTLE_ACTION_INFO
	{
		uint32	actionSourceId = 0;
		uint32	animationId = 0;
		uint32	unknown0 = 0;
		uint32	unknown1 = 0;
		uint32	unknown2 = 0;
		uint32	unknown3 = 0;
		uint32	unknown4 = 0;
		float	unknown5 = 1.0f;
		uint32	unknown6 = 1;
		uint32	descriptionId = 0;
		uint32	actionTargetId = 0;
		uint16	damage = 0;
		uint16	damageType = 0;
		uint32	feedbackId = 0;
		uint32	attackSide = 0;
	};
	static_assert(sizeof(BATTLE_ACTION_INFO) == 0x38, "Size of BATTLE_ACTION_INFO must be 56 bytes.");

	BATTLE_ACTION_INFO		m_info;
};
