#pragma once

#include "Actor.h"

class CEnemyActor : public CActor
{
public:
					CEnemyActor();
	virtual			~CEnemyActor();

	virtual void	Update(float) override;
	virtual void	TakeDamage(CActor*, uint32) override;

private:
	enum STATE
	{
		STATE_IDLE,
		STATE_ATTACKING,
		STATE_DEAD
	};

	STATE			m_state = STATE_IDLE;
	float			m_autoAttackTimer = 0;
	uint32			m_attackTargetId = 0;
};
