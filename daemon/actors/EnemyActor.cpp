#include "EnemyActor.h"

#include "../packets/SetActorStatePacket.h"
#include "../packets/BattleActionPacket.h"

#define AUTO_ATTACK_DELAY		7.0f

CEnemyActor::CEnemyActor()
{

}

CEnemyActor::~CEnemyActor()
{

}

void CEnemyActor::TakeDamage(CActor* sourceActor, uint32 amount)
{
	CActor::TakeDamage(sourceActor, amount);

	if(m_hp == 0)
	{
		if(m_state != STATE_DEAD)
		{
			auto packet = std::make_shared<CSetActorStatePacket>();
			packet->SetState(CSetActorStatePacket::STATE_DEAD);
			GlobalPacketReady(this, packet);
			//If we set the dead state here, the client state won't be updated properly till
			//another packet is sent
			//Need to send "you have defeated x" message.
			m_state = STATE_DEAD;
		}
	}
	else
	{
		if(m_state != STATE_ATTACKING)
		{
			{
				auto packet = std::make_shared<CSetActorStatePacket>();
				packet->SetState(CSetActorStatePacket::STATE_ACTIVE);
				GlobalPacketReady(this, packet);
			}
			m_autoAttackTimer = AUTO_ATTACK_DELAY;
			m_state = STATE_ATTACKING;
			m_attackTargetId = sourceActor->GetId();
		}
	}
}

void CEnemyActor::Update(float dt)
{
	if(m_state == STATE_ATTACKING)
	{
		assert(m_hp != 0);

		m_autoAttackTimer -= dt;
		if(m_autoAttackTimer < 0)
		{
			{
				auto packet = std::make_shared<CBattleActionPacket>();
				packet->SetActionSourceId(m_id);
				packet->SetActionTargetId(m_attackTargetId);
				packet->SetAnimationId(CBattleActionPacket::ANIMATION_ENEMY_ATTACK);
				packet->SetDescriptionId(CBattleActionPacket::DESCRIPTION_ENEMY_ATTACK);
				packet->SetDamageType(CBattleActionPacket::DAMAGE_NORMAL);
				packet->SetDamage(0);
				packet->SetFeedbackId(CBattleActionPacket::FEEDBACK_NORMAL);
				packet->SetAttackSide(CBattleActionPacket::SIDE_FRONT);
				GlobalPacketReady(this, packet);
			}

			//Should deal damage here
			m_autoAttackTimer += AUTO_ATTACK_DELAY;
		}
	}
//	if(m_state == STATE_DEAD)
//	{
//		auto packet = std::make_shared<CSetActorStatePacket>();
//		packet->SetState(CSetActorStatePacket::STATE_PASSIVE);
//		GlobalPacketReady(this, packet);
//		m_state = STATE_IDLE;
//	}
}
