#include "BattleActionPacket.h"

//Damage Types
//0x765D = Normal attack
//0x7662 = Partially Blocked?

//Feedback IDs
//0x08000604 = Normal?
//0x0800104C = Protect?

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
