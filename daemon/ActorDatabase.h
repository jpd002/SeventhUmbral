#pragma once

#include <vector>
#include <tuple>
#include "Stream.h"

typedef std::tuple<float, float, float> ActorVector3;

struct ACTOR_DEFINITION
{
	uint32			id = 0;
	uint32			nameStringId = 0;
	uint32			baseModelId = 0;
	uint32			topModelId = 0;
	ActorVector3	pos = ActorVector3(0.f, 0.f, 0.f);
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
