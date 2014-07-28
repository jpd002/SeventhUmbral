#include "PlayerActor.h"
#include "Endian.h"
#include "string_format.h"

#include "../Log.h"
#include "../Instance.h"
#include "../DatabaseConnectionManager.h"
#include "../GameServer_Ingame.h"
#include "../packets/BattleActionPacket.h"
#include "../packets/FinishScriptPacket.h"
#include "../packets/SetMusicPacket.h"
#include "../packets/SetActorStatePacket.h"
#include "../packets/SetActorPropertyPacket.h"

#define LOG_NAME "PlayerActor"

#define ITEMDEFID_WEATHERED_SHORTBOW		4070001
#define ITEMDEFID_WEATHERED_WARAXE			4040001
#define ITEMDEFID_WEATHERED_GLADIUS			4030010
#define ITEMDEFID_WEATHERED_SPEAR			4080201
#define ITEMDEFID_WEATHERED_HORA			4020001
#define ITEMDEFID_WEATHERED_SCEPTER			5020001
#define ITEMDEFID_WEATHERED_CANE			5030101
#define ITEMDEFID_WEATHERED_ALEMBIC			6070001
#define ITEMDEFID_WEATHERED_DOMINGHAMMER	6030001
#define ITEMDEFID_WEATHERED_CROSSPEINHAMMER	6020001
#define ITEMDEFID_WEATHERED_HATCHET			7020002
#define ITEMDEFID_WEATHERED_SAW				6010001
#define ITEMDEFID_WEATHERED_SKILLET			6080001
#define ITEMDEFID_WEATHERED_FISHINGROD		7030002
#define ITEMDEFID_WEATHERED_CHASERHAMMER	6040001
#define ITEMDEFID_WEATHERED_HEADKNIFE		6050003
#define ITEMDEFID_WEATHERED_PICKAXE			7010005
#define ITEMDEFID_RUSTY_NEEDLE				6060006

#define AUTO_ATTACK_DELAY	5.0f

CPlayerActor::CPlayerActor(uint32 characterId)
: m_characterId(characterId)
{
	m_dbConnection = CDatabaseConnectionManager::GetInstance().CreateConnection();

	try
	{
		auto query = string_format("SELECT * FROM ffxiv_characters WHERE id = %d", m_characterId);
		auto result = m_dbConnection.Query(query.c_str());
		if(result.GetRowCount() != 0)
		{
			m_character = CCharacter(result);
		}
	}
	catch(const std::exception& exception)
	{
		CLog::GetInstance().LogError(LOG_NAME, "Failed to fetch character (id = %d): %s", m_characterId, exception.what());
	}

	//Add some items in the inventory
	m_inventory.push_back(INVENTORY_ITEM(0x4000, ITEMDEFID_WEATHERED_SHORTBOW));
	m_inventory.push_back(INVENTORY_ITEM(0x4001, ITEMDEFID_WEATHERED_WARAXE));
	m_inventory.push_back(INVENTORY_ITEM(0x4002, ITEMDEFID_WEATHERED_GLADIUS));
	m_inventory.push_back(INVENTORY_ITEM(0x4003, ITEMDEFID_WEATHERED_SPEAR));
	m_inventory.push_back(INVENTORY_ITEM(0x4004, ITEMDEFID_WEATHERED_HORA));
	m_inventory.push_back(INVENTORY_ITEM(0x4005, ITEMDEFID_WEATHERED_SCEPTER));
	m_inventory.push_back(INVENTORY_ITEM(0x4006, ITEMDEFID_WEATHERED_CANE));

	m_inventory.push_back(INVENTORY_ITEM(0x4007, ITEMDEFID_WEATHERED_ALEMBIC));
	m_inventory.push_back(INVENTORY_ITEM(0x4008, ITEMDEFID_WEATHERED_DOMINGHAMMER));
	m_inventory.push_back(INVENTORY_ITEM(0x4009, ITEMDEFID_WEATHERED_CROSSPEINHAMMER));
	m_inventory.push_back(INVENTORY_ITEM(0x400A, ITEMDEFID_WEATHERED_HATCHET));
	m_inventory.push_back(INVENTORY_ITEM(0x400B, ITEMDEFID_WEATHERED_SAW));
	m_inventory.push_back(INVENTORY_ITEM(0x400C, ITEMDEFID_WEATHERED_SKILLET));
	m_inventory.push_back(INVENTORY_ITEM(0x400D, ITEMDEFID_WEATHERED_FISHINGROD));
	m_inventory.push_back(INVENTORY_ITEM(0x400E, ITEMDEFID_WEATHERED_CHASERHAMMER));
	m_inventory.push_back(INVENTORY_ITEM(0x400F, ITEMDEFID_WEATHERED_HEADKNIFE));
	m_inventory.push_back(INVENTORY_ITEM(0x4010, ITEMDEFID_WEATHERED_PICKAXE));
	m_inventory.push_back(INVENTORY_ITEM(0x4011, ITEMDEFID_RUSTY_NEEDLE));
}

CPlayerActor::~CPlayerActor()
{

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

const CCharacter& CPlayerActor::GetCharacter() const
{
	return m_character;
}

const Inventory& CPlayerActor::GetInventory() const
{
	return m_inventory;
}

void CPlayerActor::SetSelection(uint32 selectedActorId)
{
	m_lockOnId = selectedActorId;
}

void CPlayerActor::ProcessCommandRequest(uint32 targetId, const PacketData& commandPacket)
{
	switch(targetId)
	{
	case 0xA0F02EE9:
		EquipItem(commandPacket);
		break;
	case 0xA0F05E26:
		DoEmote(commandPacket);
		break;
	case 0xA0F05EA2:
		TrashItem(commandPacket);
		break;
	default:
		CLog::GetInstance().LogDebug(LOG_NAME, "Unknown target id (0x%0.8X).", targetId);
		break;
	}
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

void CPlayerActor::EquipItem(const PacketData& commandPacket)
{
	CLog::GetInstance().LogDebug(LOG_NAME, "EquipItem");

//	uint32 itemId = *reinterpret_cast<const uint32*>(&subPacket[0x6E]);

//	CLog::GetInstance().LogDebug(LOG_NAME, "Equipping Item: 0x%0.8X", itemId);
	
//	CCompositePacket packet;
//	packet.AddPacket(PacketData(std::begin(unknownPacket1), std::end(unknownPacket1)));
//	packet.AddPacket(PacketData(std::begin(changeAppearancePacket), std::end(changeAppearancePacket)));
//	packet.AddPacket(PacketData(std::begin(unknownPacket), std::end(unknownPacket)));
//	QueuePacket(packet.ToPacketData());
}

void CPlayerActor::TrashItem(const PacketData& commandPacket)
{
	uint32 itemId = *reinterpret_cast<const uint32*>(&commandPacket[0x6A]);
	CLog::GetInstance().LogDebug(LOG_NAME, "Trashing Item: 0x%0.8X", itemId);
}

void CPlayerActor::DoEmote(const PacketData& commandPacket)
{
#if 0
	uint8 emoteId = subPacket[0x55];

	CLog::GetInstance().LogDebug(LOG_NAME, "Executing Emote 0x%0.2X", emoteId);

	uint8 commandRequestPacket[0x40] =
	{
		0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x52, 0xE2, 0xA4, 0xEE, 0x3B, 0x01, 0x00, 0x00,
		0x30, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9b, 0x02, 0x41, 0x29, 0x9b, 0x02, 0x00, 0xe0, 0xd2, 0xfe,
		0x14, 0x00, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd1, 0xee, 0xe0, 0x50, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xb0, 0x00, 0x05, 0x41, 0x29, 0x9b, 0x02, 0x6e, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};

	//In: 0x6F, Out: (0x0500B000, 0x526E) -> Dance
	//In: 0x??, Out: (0x5000C000, 0x????) -> Angry Pointing
	//In: 0x??, Out: (0x5000D000, 0x????) -> Snooze
	//In: 0x??, Out: (0x5000E000, 0x????) -> Frustrated
	//In: 0x??, Out: (0x5000F000, 0x????) -> Military Sign
	//In: 0x??, Out: (0x50011000, 0x????) -> Shrug
	//In: 0x??, Out: (0x50012000, 0x????) -> Success Baby
	//In: 0x77, Out: (0x05013000, 0x52BE) -> Kneel
	//In: 0x??, Out: (0x50014000, 0x????) -> Chuckle
	//In: 0x??, Out: (0x50015000, 0x????) -> Laugh
	//In: 0x??, Out: (0x50016000, 0x????) -> Look
	//In: 0x??, Out: (0x50018000, 0x????) -> No
	//In: 0x??, Out: (0x50019000, 0x????) -> Never
					
	uint32 animationId = 0x0500B000;
	uint32 descriptionId = 0x526E;

	//Wrong emotes
	//gcsalute		-> grovel
	//grovel		-> serpent salute
	//blowkiss		-> disappointed
	//pray			-> firedance
	//airquote		-> pray
	//pose			-> blowkiss
	//happy			-> maelstorm salute
	//disappointed	-> pose

	if(emoteId >= 0x64 && emoteId < 0xA0)
	{
		animationId = 0x05000000 + ((emoteId - 0x64) << 12);
	}
/*
	switch(emoteId)
	{
	case 0x6A:		//Cheer
		animationId = 0x05006000;
		break;
	case 0x6F:		//Dance
		animationId = 0x0500B000;
		break;
	case 0x71:		//Doze
		animationId = 0x0500D000;
		break;
	case 0x75:		//Huh
		animationId = 0x05011000;
		break;
	case 0x78:		//Chuckle
		animationId = 0x05014000;
		break;
	case 0x79:		//Laugh
		animationId = 0x05015000;
		break;
	}
*/

	*reinterpret_cast<uint32*>(&commandRequestPacket[0x28]) = clientTime;
	*reinterpret_cast<uint32*>(&commandRequestPacket[0x30]) = animationId;
	*reinterpret_cast<uint32*>(&commandRequestPacket[0x38]) = descriptionId;

//	printf("Anim Id = 0x%0.8X, Desc Id = 0x%0.8X\r\n", animationId, descriptionId);
//	animationId += 0x1000;
//	descriptionId += 1;
	QueuePacket(PacketData(std::begin(commandRequestPacket), std::end(commandRequestPacket)));
#endif
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
