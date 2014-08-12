#include "PlayerActor.h"
#include "Endian.h"
#include "string_format.h"

#include "../Log.h"
#include "../GlobalData.h"
#include "../Instance.h"
#include "../DatabaseConnectionManager.h"
#include "../GameServer_Ingame.h"
#include "../packets/BattleActionPacket.h"
#include "../packets/ChangeEquipmentSlotPacket.h"
#include "../packets/FinishScriptPacket.h"
#include "../packets/SetMusicPacket.h"
#include "../packets/SetActorAppearancePacket.h"
#include "../packets/SetActorStatePacket.h"
#include "../packets/SetActorPropertyPacket.h"
#include "../packets/SetTempInventoryPacket.h"
#include "../packets/UnknownInventoryPacket.h"
#include "../packets/CommandRequestReplyPacket.h"

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

#define JOBID_ARCHER		(7)
#define JOBID_MARAUDER		(4)
#define JOBID_GLADIATOR		(3)
#define JOBID_LANCER		(8)
#define JOBID_PUGILIST		(2)
#define JOBID_THAUMATURGE	(22)
#define JOBID_CONJURER		(23)

#define JOBID_ALCHEMIST		(35)
#define JOBID_ARMORER		(31)
#define JOBID_BLACKSMITH	(30)
#define JOBID_BOTANIST		(40)
#define JOBID_CARPENTER		(29)
#define JOBID_CULINARIAN	(36)
#define JOBID_FISHER		(41)
#define JOBID_GOLDSMITH		(32)
#define JOBID_LEATHERWORKER	(33)
#define JOBID_MINER			(39)
#define JOBID_WEAVER		(34)

#define SKILLID_STONE_THROW		(0xA0F05662)
#define SKILLID_PUMMEL			(0xA0F069E6)
#define SKILLID_FAST_BLADE		(0xA0F06A0E)
#define SKILLID_HEAVY_SWING		(0xA0F06A36)
#define SKILLID_HEAVY_SHOT		(0xA0F06A5C)
#define SKILLID_TRUE_THURST		(0xA0F06A85)
#define SKILLID_STONE			(0xA0F06ADB)
#define SKILLID_THUNDER			(0xA0F06AB1)

const CPlayerActor::WeaponJobMap CPlayerActor::m_weaponJobs =
{
	std::make_pair(ITEMDEFID_WEATHERED_SHORTBOW,		JOBID_ARCHER),
	std::make_pair(ITEMDEFID_WEATHERED_WARAXE,			JOBID_MARAUDER),
	std::make_pair(ITEMDEFID_WEATHERED_GLADIUS,			JOBID_GLADIATOR),
	std::make_pair(ITEMDEFID_WEATHERED_SPEAR,			JOBID_LANCER),
	std::make_pair(ITEMDEFID_WEATHERED_HORA,			JOBID_PUGILIST),
	std::make_pair(ITEMDEFID_WEATHERED_SCEPTER,			JOBID_THAUMATURGE),
	std::make_pair(ITEMDEFID_WEATHERED_CANE,			JOBID_CONJURER),

	std::make_pair(ITEMDEFID_WEATHERED_ALEMBIC,			JOBID_ALCHEMIST),
	std::make_pair(ITEMDEFID_WEATHERED_DOMINGHAMMER,	JOBID_ARMORER),
	std::make_pair(ITEMDEFID_WEATHERED_CROSSPEINHAMMER,	JOBID_BLACKSMITH),
	std::make_pair(ITEMDEFID_WEATHERED_HATCHET,			JOBID_BOTANIST),
	std::make_pair(ITEMDEFID_WEATHERED_SAW,				JOBID_CARPENTER),
	std::make_pair(ITEMDEFID_WEATHERED_SKILLET,			JOBID_CULINARIAN),
	std::make_pair(ITEMDEFID_WEATHERED_FISHINGROD,		JOBID_FISHER),
	std::make_pair(ITEMDEFID_WEATHERED_CHASERHAMMER,	JOBID_GOLDSMITH),
	std::make_pair(ITEMDEFID_WEATHERED_HEADKNIFE,		JOBID_LEATHERWORKER),
	std::make_pair(ITEMDEFID_WEATHERED_PICKAXE,			JOBID_MINER),
	std::make_pair(ITEMDEFID_RUSTY_NEEDLE,				JOBID_WEAVER),
};

const CPlayerActor::JobSkillMap CPlayerActor::m_jobSkills =
{
	std::make_pair(JOBID_ARCHER,		SKILLID_HEAVY_SHOT),
	std::make_pair(JOBID_MARAUDER,		SKILLID_HEAVY_SWING),
	std::make_pair(JOBID_GLADIATOR,		SKILLID_FAST_BLADE),
	std::make_pair(JOBID_LANCER,		SKILLID_TRUE_THURST),
	std::make_pair(JOBID_PUGILIST,		SKILLID_PUMMEL),
	std::make_pair(JOBID_THAUMATURGE,	SKILLID_THUNDER),
	std::make_pair(JOBID_CONJURER,		SKILLID_STONE),
};

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
	case SKILLID_STONE_THROW:
	case SKILLID_PUMMEL:
	case SKILLID_FAST_BLADE:
	case SKILLID_HEAVY_SWING:
	case SKILLID_HEAVY_SHOT:
	case SKILLID_TRUE_THURST:
	case SKILLID_STONE:
	case SKILLID_THUNDER:
		ExecuteBattleSkill(CBattleActionPacket::ANIMATION_HEAVY_SWING, 0x08100000 | (targetId & 0xFFFF), 20);
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
	uint32 itemId = Framework::CEndian::FromMSBF32(*reinterpret_cast<const uint32*>(&commandPacket[0x6E]));
	CLog::GetInstance().LogDebug(LOG_NAME, "Equipping item 0x%0.8X.", itemId);

	//itemId will be 0 if player wants to unequip an item
	if(itemId == 0)
	{
		CLog::GetInstance().LogDebug(LOG_NAME, "Unequip: %s.", CPacketUtils::DumpPacket(commandPacket).c_str());
	}

	auto inventoryItemIterator = std::find_if(std::begin(m_inventory), std::end(m_inventory), 
		[itemId](const INVENTORY_ITEM& item) { return item.itemId == itemId; });

	if(inventoryItemIterator == std::end(m_inventory)) return;
	const auto& inventoryItem = *inventoryItemIterator;
	size_t itemIndex = inventoryItemIterator - std::begin(m_inventory);

	auto itemAppearance = CGlobalData::GetInstance().GetWeaponAppearanceDatabase().GetAppearanceForItemId(inventoryItem.itemDefId);
	if(itemAppearance)
	{
		auto weapon1AttrIterator = itemAppearance->attributes.find("Weapon1");
		auto weapon2AttrIterator = itemAppearance->attributes.find("Weapon2");
		if(weapon1AttrIterator != std::end(itemAppearance->attributes))
		{
			m_character.weapon1 = weapon1AttrIterator->second;
		}
		if(weapon2AttrIterator != std::end(itemAppearance->attributes))
		{
			m_character.weapon2 = weapon2AttrIterator->second;
		}
	}

	uint8 newJob = JOBID_GLADIATOR;	//Default is Gladiator
	auto weaponJobIterator = m_weaponJobs.find(inventoryItem.itemDefId);
	if(weaponJobIterator != std::end(m_weaponJobs))
	{
		newJob = weaponJobIterator->second;
	}

	uint32 jobSkill = SKILLID_STONE_THROW;
	auto jobSkillIterator = m_jobSkills.find(newJob);
	if(jobSkillIterator != std::end(m_jobSkills))
	{
		jobSkill = jobSkillIterator->second;
	}

	{
		auto packet = std::make_shared<CSetActorAppearancePacket>();
		packet->SetAppearanceItem(0x00, CCharacter::GetModelFromTribe(m_character.tribe));
		packet->SetAppearanceItem(0x01, m_character.size);
		packet->SetAppearanceItem(0x02, m_character.GetColorInfo());
		packet->SetAppearanceItem(0x03, m_character.GetFaceInfo());
		packet->SetAppearanceItem(0x04, m_character.hairStyle << 10);
		packet->SetAppearanceItem(0x05, m_character.voice);
		packet->SetAppearanceItem(0x06, m_character.weapon1);
		packet->SetAppearanceItem(0x07, m_character.weapon2);
		packet->SetAppearanceItem(0x08, 0);
		packet->SetAppearanceItem(0x09, 0);
		packet->SetAppearanceItem(0x0A, 0);
		packet->SetAppearanceItem(0x0B, 0);
		packet->SetAppearanceItem(0x0C, 0);
		packet->SetAppearanceItem(0x0D, m_character.headGear);
		packet->SetAppearanceItem(0x0E, m_character.bodyGear);
		packet->SetAppearanceItem(0x0F, m_character.legsGear);
		packet->SetAppearanceItem(0x10, m_character.handsGear);
		packet->SetAppearanceItem(0x11, m_character.feetGear);
		packet->SetAppearanceItem(0x12, m_character.waistGear);
		packet->SetAppearanceItem(0x13, 0);
		packet->SetAppearanceItem(0x14, m_character.rightEarGear);
		packet->SetAppearanceItem(0x15, m_character.leftEarGear);
		packet->SetAppearanceItem(0x16, 0);
		packet->SetAppearanceItem(0x17, 0);
		packet->SetAppearanceItem(0x18, m_character.rightFingerGear);
		packet->SetAppearanceItem(0x19, m_character.leftFingerGear);
		packet->SetAppearanceItem(0x1A, 0);
		packet->SetAppearanceItem(0x1B, 0);
		GlobalPacketReady(this, packet);
	}

	{
		{
			auto packet = std::make_shared<CUnknownInventoryPacket_016D>();
			LocalPacketReady(this, packet);
		}

		{
			auto packet = std::make_shared<CUnknownInventoryPacket_0146>();
			packet->SetActorId(m_id);
			packet->SetUnknown0(200);		//Inventory size?
			LocalPacketReady(this, packet);
		}

		{
			auto packet = std::make_shared<CSetTempInventoryPacket>();
			packet->SetItemCount(1);
			packet->SetItemIndex(0, itemIndex + 1);
			packet->SetItemId(0, inventoryItem.itemId);
			packet->SetItemDefinitionId(0, inventoryItem.itemDefId);
			LocalPacketReady(this, packet);
		}

		{
			auto packet = std::make_shared<CUnknownInventoryPacket_0146>();
			packet->SetActorId(m_id);
			packet->SetUnknown0(0x23);
			packet->SetUnknown1(0xFE);
			LocalPacketReady(this, packet);
		}

		{
			auto packet = std::make_shared<CChangeEquipmentSlotPacket>();
			packet->SetSlotId(CChangeEquipmentSlotPacket::SLOT_MAINHAND);
			packet->SetItemIndex(itemIndex + 1);
			LocalPacketReady(this, packet);
		}

		for(unsigned int i = 0; i < 2; i++)
		{
			auto packet = std::make_shared<CUnknownInventoryPacket_0147>();
			LocalPacketReady(this, packet);
		}

		{
			auto packet = std::make_shared<CUnknownInventoryPacket_016E>();
			LocalPacketReady(this, packet);
		}
	}

	//Update job and level
	{
		auto packet = std::make_shared<CSetActorPropertyPacket>();
		packet->AddSetByte(CSetActorPropertyPacket::VALUE_JOB, newJob);
		packet->AddSetByte(CSetActorPropertyPacket::VALUE_LEVEL, 0x01);
		packet->AddTargetProperty("charaWork/stateForAll");
		GlobalPacketReady(this, packet);
	}

	//Update skill bar
	{
			auto packet = std::make_shared<CSetActorPropertyPacket>();
			packet->AddSetWord(0xCA132BC5, jobSkill);		//Action Bar(1, 1) -> New Skill
			packet->AddTargetProperty("charaWork/command");
			GlobalPacketReady(this, packet);
	}

	//This seems to update the UI for level and job
	{
		auto packet = std::make_shared<CSetActorPropertyPacket>();
		packet->AddSetWord(0xE98BFFBF, 0);
		packet->AddTargetProperty("charaWork/battleStateForSelf");
		GlobalPacketReady(this, packet);
	}
}

void CPlayerActor::TrashItem(const PacketData& commandPacket)
{
	uint32 itemId = *reinterpret_cast<const uint32*>(&commandPacket[0x6A]);
	CLog::GetInstance().LogDebug(LOG_NAME, "Trashing Item: 0x%0.8X", itemId);
}

void CPlayerActor::DoEmote(const PacketData& commandPacket)
{
	uint8 emoteId = commandPacket[0x55];

	CLog::GetInstance().LogDebug(LOG_NAME, "Executing Emote 0x%0.2X", emoteId);

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

	{
		auto packet = std::make_shared<CCommandRequestReplyPacket>();
		packet->SetAnimationId(animationId);
		packet->SetActorId(m_id);
		packet->SetDescriptionId(descriptionId);
		GlobalPacketReady(this, packet);
	}

//	printf("Anim Id = 0x%0.8X, Desc Id = 0x%0.8X\r\n", animationId, descriptionId);
//	animationId += 0x1000;
//	descriptionId += 1;
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
