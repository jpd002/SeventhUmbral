#include "Actor.h"

#include "../packets/SetActorStatePacket.h"
#include "../packets/SetActorPropertyPacket.h"

CActor::CActor()
{

}

CActor::~CActor()
{

}

CInstance* CActor::GetInstance() const
{
	return m_instance;
}

void CActor::SetInstance(CInstance* instance)
{
	m_instance = instance;
}

uint32 CActor::GetId() const
{
	return m_id;
}

void CActor::SetId(uint32 id)
{
	m_id = id;
}

void CActor::SetZoneId(uint32 zoneId)
{
	m_zoneId = zoneId;
}

void CActor::SetHp(uint32 hp)
{
	m_hp = hp;
}

void CActor::Update(float)
{

}

void CActor::TakeDamage(CActor*, uint32 amount)
{
	m_hp = std::max<int32>(0, m_hp - amount);
	SendHpUpdate();
}

void CActor::SendHpUpdate()
{
	auto packet = std::make_shared<CSetActorPropertyPacket>();
	packet->AddSetShort(CSetActorPropertyPacket::VALUE_HP, m_hp);
	packet->AddTargetProperty("charaWork/stateAtQuicklyForAll");
	GlobalPacketReady(this, packet);
}
