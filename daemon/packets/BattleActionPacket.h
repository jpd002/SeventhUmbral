#pragma once

#include "BasePacket.h"

class CBattleActionPacket : public CBasePacket
{
public:
	enum ANIMATION_ID
	{
		ANIMATION_ENEMY_ATTACK		= 0x11001000,
		ANIMATION_SAVAGE_BLADE		= 0x12001401,
		ANIMATION_SKULL_SUNDER		= 0x1200102B,
		ANIMATION_FRACTURE			= 0x1200302A,
		//0x1200302B = ?
		//0x1200302C = ?
		//0x1200302D = ? (looks like a bow skill)
		//0x1200302E = ? (looks like a bow skill)
		ANIMATION_PLAYER_ATTACK		= 0x19001000,
		ANIMATION_SHEATH_UNSHEATH	= 0x7C000062,
	};

	enum DESCRIPTION_ID
	{
		DESCRIPTION_ENTER_BATTLE		= 0x08105209,
		DESCRIPTION_LEAVE_BATTLE		= 0x0810520A,
		DESCRIPTION_PLAYER_ATTACK		= 0x08105658,
		DESCRIPTION_ENEMY_ATTACK		= 0x081059DD,
		DESCRIPTION_SKULL_SUNDER		= 0x08106A37,
		DESCRIPTION_FRACTURE			= 0x08106A3E,
		DESCRIPTION_OVERPOWER			= 0x08106A3F,
		DESCRIPTION_ENHANCED_PROVOKE	= 0x08106A40,
	};

	enum SIDE_ID
	{
		SIDE_NORMAL		= 0x0100,	//Huh, what's the difference with front?
		SIDE_FRONT		= 0x0101,
		SIDE_RIGHT		= 0x0102
	};

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
