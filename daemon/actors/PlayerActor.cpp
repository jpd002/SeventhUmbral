#include "PlayerActor.h"

#include "../Log.h"
#include "../Instance.h"
#include "../GameServer_Ingame.h"
#include "../packets/SetMusicPacket.h"
#include "../packets/SetActorStatePacket.h"
#include "../packets/SetActorPropertyPacket.h"
#include "../packets/BattleActionPacket.h"
#include "../packets/FinishScriptPacket.h"

#define LOG_NAME "PlayerActor"

#define AUTO_ATTACK_DELAY	5.0f

CPlayerActor::CPlayerActor()
{

}

CPlayerActor::~CPlayerActor()
{

}

void CPlayerActor::SetSelection(uint32 selectedActorId)
{
	m_lockOnId = selectedActorId;
}

void CPlayerActor::ProcessCommandForced(uint32 targetId)
{
	switch(targetId)
	{
	case 0xA0F05209:
		SwitchToActiveMode();
		break;
	case 0xA0F0520A:
		SwitchToPassiveMode();
		break;
	default:
		CLog::GetInstance().LogDebug(LOG_NAME, "Unknown commandForced target id (0x%0.8X).", targetId);
		break;
	}

	{
		auto packet = std::make_shared<CFinishScriptPacket>();
		packet->SetScriptSourceId(m_id);
		packet->SetScriptName("commandForced");
		LocalPacketReady(this, packet);
	}
}

void CPlayerActor::ProcessCommandDefault(uint32 targetId)
{
	switch(targetId)
	{
	case 0xA0F06A36:	//Heavy Swing
		ExecuteBattleSkill(CBattleActionPacket::ANIMATION_HEAVY_SWING, CBattleActionPacket::DESCRIPTION_HEAVY_SWING, 20);
		break;
	case 0xA0F06A37:	//Skull Sunder
		ExecuteBattleSkill(CBattleActionPacket::ANIMATION_SKULL_SUNDER, CBattleActionPacket::DESCRIPTION_SKULL_SUNDER, 30);
		break;
	case 0xA0F06A39:	//Brutal Swing
		ExecuteBattleSkill(CBattleActionPacket::ANIMATION_SAVAGE_BLADE, CBattleActionPacket::DESCRIPTION_BRUTAL_SWING, 40);
		break;
	case 0xA0F06A3E:	//Fracture
		ExecuteBattleSkill(CBattleActionPacket::ANIMATION_FRACTURE, CBattleActionPacket::DESCRIPTION_FRACTURE, 50);
		break;
	default:
		CLog::GetInstance().LogDebug(LOG_NAME, "Unknown commandDefault target id (0x%0.8X).", targetId);
		break;
	}

	{
		auto packet = std::make_shared<CFinishScriptPacket>();
		packet->SetScriptSourceId(m_id);
		packet->SetScriptName("commandDefault");
		LocalPacketReady(this, packet);
	}
}

void CPlayerActor::SwitchToActiveMode()
{
	{
		auto packet = std::make_shared<CSetActorStatePacket>();
		packet->SetState(CSetActorStatePacket::STATE_ACTIVE);
		GlobalPacketReady(this, packet);
	}

	{
		auto packet = std::make_shared<CSetActorPropertyPacket>();
		packet->AddSetShort(CSetActorPropertyPacket::VALUE_TP, 3000);
		packet->AddTargetProperty("charaWork/stateAtQuicklyForAll");
		GlobalPacketReady(this, packet);
	}

	{
		auto packet = std::make_shared<CBattleActionPacket>();
		packet->SetActionSourceId(m_id);
		packet->SetActionTargetId(m_id);
		packet->SetAnimationId(CBattleActionPacket::ANIMATION_SHEATH_UNSHEATH);
		packet->SetDescriptionId(CBattleActionPacket::DESCRIPTION_ENTER_BATTLE);
		packet->SetFeedbackId(1);
		packet->SetAttackSide(CBattleActionPacket::SIDE_NORMAL);
		GlobalPacketReady(this, packet);
	}

	{
		auto packet = std::make_shared<CSetMusicPacket>();
		packet->SetMusicId(CSetMusicPacket::MUSIC_BLACKSHROUD_BATTLE);
		LocalPacketReady(this, packet);
	}

	m_isActiveMode = true;
	m_autoAttackTimer = AUTO_ATTACK_DELAY;
}

void CPlayerActor::SwitchToPassiveMode()
{
	{
		auto packet = std::make_shared<CSetActorStatePacket>();
		packet->SetState(CSetActorStatePacket::STATE_PASSIVE);
		GlobalPacketReady(this, packet);
	}

	{
		auto packet = std::make_shared<CBattleActionPacket>();
		packet->SetActionSourceId(m_id);
		packet->SetActionTargetId(m_id);
		packet->SetAnimationId(CBattleActionPacket::ANIMATION_SHEATH_UNSHEATH);
		packet->SetDescriptionId(CBattleActionPacket::DESCRIPTION_LEAVE_BATTLE);
		packet->SetFeedbackId(1);
		packet->SetAttackSide(CBattleActionPacket::SIDE_NORMAL);
		GlobalPacketReady(this, packet);
	}

	{
		auto packet = std::make_shared<CSetMusicPacket>();
		packet->SetMusicId(CSetMusicPacket::MUSIC_SHROUD);
		LocalPacketReady(this, packet);
	}

	m_isActiveMode = false;
}

void CPlayerActor::ExecuteBattleSkill(uint32 animationId, uint32 descriptionId, uint32 damage)
{
	{
		auto packet = std::make_shared<CBattleActionPacket>();
		packet->SetActionSourceId(m_id);
		packet->SetActionTargetId(m_lockOnId);
		packet->SetAnimationId(animationId);
		packet->SetDescriptionId(descriptionId);
		packet->SetDamageType(CBattleActionPacket::DAMAGE_NORMAL);
		packet->SetDamage(damage);
		packet->SetFeedbackId(CBattleActionPacket::FEEDBACK_NORMAL);
		packet->SetAttackSide(CBattleActionPacket::SIDE_FRONT);
		GlobalPacketReady(this, packet);
	}

	DealDamageToTarget(damage);

	//Reset auto attack timer
	m_autoAttackTimer = AUTO_ATTACK_DELAY;
}

void CPlayerActor::DealDamageToTarget(uint32 damage)
{
	auto targetActor = m_instance->GetActor<CActor>(m_lockOnId);
	if(targetActor == nullptr)
	{
		CLog::GetInstance().LogError(LOG_NAME, "Couldn't find target actor.");
	}
	else
	{
		targetActor->TakeDamage(this, damage);
	}
}

void CPlayerActor::Update(float dt)
{
	if(m_isActiveMode && m_lockOnId != EMPTY_LOCKON_ID)
	{
		m_autoAttackTimer -= dt;
		if(m_autoAttackTimer < 0)
		{
			static const uint32 autoAttackDamage = 10;

			{
				auto packet = std::make_shared<CBattleActionPacket>();
				packet->SetActionSourceId(m_id);
				packet->SetActionTargetId(m_lockOnId);
				packet->SetAnimationId(CBattleActionPacket::ANIMATION_PLAYER_ATTACK);
				packet->SetDescriptionId(CBattleActionPacket::DESCRIPTION_PLAYER_ATTACK);
				packet->SetDamageType(CBattleActionPacket::DAMAGE_NORMAL);
				packet->SetDamage(autoAttackDamage);
				packet->SetFeedbackId(CBattleActionPacket::FEEDBACK_NORMAL);
				packet->SetAttackSide(CBattleActionPacket::SIDE_FRONT);
				GlobalPacketReady(this, packet);
			}

			m_autoAttackTimer += AUTO_ATTACK_DELAY;
			DealDamageToTarget(autoAttackDamage);
		}
	}
}
