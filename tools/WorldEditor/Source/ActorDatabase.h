#pragma once

#include <vector>
#include "Stream.h"
#include "AthenaEngine.h"

struct ACTOR_DEFINITION
{
	unsigned int	baseModelId = 0;
	unsigned int	topModelId = 0;
	CVector3		position = CVector3(0, 0, 0);
};

class CActorDatabase
{
public:
	typedef std::vector<ACTOR_DEFINITION> ActorArray;

							CActorDatabase();
	virtual					~CActorDatabase();

	static CActorDatabase	CreateFromXml(Framework::CStream&);

	const ActorArray&		GetActors() const;

	void					AddActor(const ACTOR_DEFINITION&);

private:
	ActorArray				m_actors;
};
