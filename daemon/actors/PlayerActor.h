#pragma once

#include "Actor.h"

class CPlayerActor : public CActor
{
public:
						CPlayerActor();
	virtual				~CPlayerActor();

	void				Update(float) override;

	void				SetSelection(uint32);

	void				ProcessCommandForced(uint32);
	void				ProcessCommandDefault(uint32);

private:
	enum
	{
		EMPTY_LOCKON_ID = 0xE0000000,
	};

	void				SwitchToActiveMode();
	void				SwitchToPassiveMode();
	void				ExecuteBattleSkill(uint32, uint32, uint32);
	
	void				DealDamageToTarget(uint32);

	bool				m_isActiveMode = false;
	float				m_autoAttackTimer = 0;
	uint32				m_lockOnId = EMPTY_LOCKON_ID;
};
