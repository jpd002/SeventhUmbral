#include "BattleActionPacket.h"

//Animation IDs
//0x11001000 = enemy attack?
//0x12001401 = Savage Blade?
//0x1200102B = Skull Sunder
//0x1200302A = Fracture
//0x1200302B = ?
//0x1200302C = ?
//0x1200302D = ? (looks like a bow skill)
//0x1200302E = ? (looks like a bow skill)
//0x19001000 = player attack?
//0x7C000062 = Sheath/Unsheath animations?

//Description IDs
//0x08105209 = Enter Battle Mode
//0x0810520A = Leave Battle Mode
//0x08105658 = Player Attack
//0x081059DD = Enemy Attack
//0x08106A37 = Skull Sunder
//0x08106A3E = Fracture 
//0x08106A3F = Overpower
//0x08106A40 = Enhanced Provoke

//Damage Types
//0x765D = Normal attack
//0x7662 = Partially Blocked?

//Feedback IDs
//0x08000604 = Normal?
//0x0800104C = Protect?

//Attack Sides
//0x0100 = normal?
//0x0101 = normal
//0x0102 = right

CBattleActionPacket::CBattleActionPacket()
{
	m_commandId = 0x139;
	m_packetSize = 0x58;
}

CBattleActionPacket::~CBattleActionPacket()
{

}

void CBattleActionPacket::SetActionSourceId(uint32 actionSourceId)
{
	m_info.actionSourceId = actionSourceId;
}

void CBattleActionPacket::SetAnimationId(uint32 animationId)
{
	m_info.animationId = animationId;
}

void CBattleActionPacket::SetDescriptionId(uint32 descriptionId)
{
	m_info.descriptionId = descriptionId;
}

void CBattleActionPacket::SetActionTargetId(uint32 actionTargetId)
{
	m_info.actionTargetId = actionTargetId;
}

void CBattleActionPacket::SetDamage(uint16 damage)
{
	m_info.damage = damage;
}

void CBattleActionPacket::SetDamageType(uint16 damageType)
{
	m_info.damageType = damageType;
}

void CBattleActionPacket::SetFeedbackId(uint32 feedbackId)
{
	m_info.feedbackId = feedbackId;
}

void CBattleActionPacket::SetAttackSide(uint32 attackSide)
{
	m_info.attackSide = attackSide;
}

PacketData CBattleActionPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	memcpy(result.data() + 0x20, &m_info, sizeof(BATTLE_ACTION_INFO));
	return result;
}
