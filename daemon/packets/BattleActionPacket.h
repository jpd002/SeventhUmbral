#pragma once

#include "BasePacket.h"

class CBattleActionPacket : public CBasePacket
{
public:
	enum ANIMATION_ID
	{
		//0x01001032 = ? (Some kind of dispell?)
		//0x01001033 = ? (Some kind of dispell?)
		//0x01001035 = ? (Smoke and bomb effect?)
		ANIMATION_CURE				= 0x01001065,
		ANIMATION_RAISE				= 0x01001066,
		ANIMATION_CURA				= 0x01001067,
		//0x0100106A = ?
		//0x0100106B = ? (same as above but stronger)
		//0x01001072 = Some ice?
		//0x01001075 = Some cure?
		//0x01001076 = Some cure? (Same as above but bigger)
		//0x01001077 = Some cure? (Same as above but even bigger)
		//0x01001085 = Stonewall
		//0x01001086 = Stonewall (bigger)
		//0x01001087 = Stonewall (even bigger)
		//0x0100108E = Some kind of raise
		//0x01001092 = Cure (but bigger)
		//0x01001093 = Cure (but bigger)
		//0x01001094 = ?
		//0x01001095 = ?
		//0x01001096 = Bard effect
		//0x01001097 = Sleep (unsure)
		//0x01001098 = ?
		ANIMATION_THUNDER			= 0x010011F7,
		ANIMATION_STONE				= 0x01001201,
		ANIMATION_ENEMY_ATTACK		= 0x11001000,
		ANIMATION_SAVAGE_BLADE		= 0x12001401,
		ANIMATION_HEAVY_SWING		= 0x1200100E,
		ANIMATION_SKULL_SUNDER		= 0x1200102B,
		ANIMATION_FRACTURE			= 0x1200302A,
		//0x1200302B = ?
		//0x1200302C = ?
		//0x1200302D = ? (looks like a bow skill)
		//0x1200302E = ? (looks like a bow skill)
		ANIMATION_PLAYER_ATTACK		= 0x19001000,
		ANIMATION_CAST_THAUMATURGE	= 0x6F000002,
		ANIMATION_CAST_CONJURER		= 0x6F000003,
		ANIMATION_SHEATH_UNSHEATH	= 0x7C000062,
		ANIMATION_FAILED			= 0x7F000002,		//Used when spell casting failed
	};

	enum DESCRIPTION_ID
	{
		DESCRIPTION_ENTER_BATTLE		= 0x08105209,
		DESCRIPTION_LEAVE_BATTLE		= 0x0810520A,
		DESCRIPTION_PLAYER_ATTACK		= 0x08105658,
		DESCRIPTION_ENEMY_ATTACK		= 0x081059DD,
		DESCRIPTION_HEAVY_SWING			= 0x08106A36,
		DESCRIPTION_SKULL_SUNDER		= 0x08106A37,
		DESCRIPTION_BRUTAL_SWING		= 0x08106A39,
		DESCRIPTION_FRACTURE			= 0x08106A3E,
		DESCRIPTION_OVERPOWER			= 0x08106A3F,
		DESCRIPTION_ENHANCED_PROVOKE	= 0x08106A40,
	};

	enum DAMAGE_ID
	{
		DAMAGE_PREPARESPELL				= 0x75B0,
		DAMAGE_NORMAL					= 0x765D,
		DAMAGE_PARTIALLY_BLOCKED		= 0x7662,
		DAMAGE_USEDSPELL_GAINHP			= 0x7670
	};
	
	enum FEEDBACK_ID
	{
		FEEDBACK_NORMAL					= 0x08000604,
		FEEDBACK_PROTECT				= 0x0800104C,
		FEEDBACK_GAINHP					= 0x30000103,
	};

	enum SIDE_ID
	{
		SIDE_NORMAL		= 0x0100,	//Huh, what's the difference with front?
		SIDE_FRONT		= 0x0101,
		SIDE_RIGHT		= 0x0102
	};

							CBattleActionPacket();
	virtual					~CBattleActionPacket();

	virtual PacketPtr		Clone() const override;

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
