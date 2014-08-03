#pragma once

#include "Actor.h"
#include "mysql/Client.h"
#include "../Character.h"

struct INVENTORY_ITEM
{
	INVENTORY_ITEM(uint32 itemId, uint32 itemDefId)
		: itemId(itemId), itemDefId(itemDefId)
	{
	
	}

	uint32	itemId = 0;
	uint32	itemDefId = 0;
};
typedef std::vector<INVENTORY_ITEM> Inventory;

class CPlayerActor : public CActor
{
public:
								CPlayerActor(uint32);
	virtual						~CPlayerActor();

	void						Update(float) override;

	const CCharacter&			GetCharacter() const;
	const Inventory&			GetInventory() const;

	void						SetSelection(uint32);

	void						ProcessCommandRequest(uint32, const PacketData&);
	void						ProcessCommandForced(uint32);
	void						ProcessCommandDefault(uint32);

private:
	typedef std::map<uint32, uint8> WeaponJobMap;

	enum
	{
		EMPTY_LOCKON_ID = 0xE0000000,
	};

	void						EquipItem(const PacketData&);
	void						DoEmote(const PacketData&);
	void						TrashItem(const PacketData&);

	void						SwitchToActiveMode();
	void						SwitchToPassiveMode();
	void						ExecuteBattleSkill(uint32, uint32, uint32);
	
	void						DealDamageToTarget(uint32);

	Framework::MySql::CClient	m_dbConnection;

	bool						m_isActiveMode = false;
	float						m_autoAttackTimer = 0;
	uint32						m_lockOnId = EMPTY_LOCKON_ID;
	CCharacter					m_character;
	Inventory					m_inventory;
	uint32						m_characterId = 0;
	static WeaponJobMap			m_weaponJobs;
};
