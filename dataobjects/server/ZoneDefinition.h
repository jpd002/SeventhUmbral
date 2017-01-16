#pragma once

#include <vector>
#include "Types.h"
#include "math/Vector3.h"
#include "Stream.h"

struct ACTOR_DEFINITION
{
	uint32			id = 0;
	uint32			nameStringId = 0;
	uint32			baseModelId = 0;
	uint32			topModelId = 0;
	CVector3		position = CVector3(0, 0, 0);
};

class CZoneDefinition
{
public:
	typedef std::vector<ACTOR_DEFINITION> ActorArray;

						CZoneDefinition() = default;
						CZoneDefinition(Framework::CStream&);
	virtual				~CZoneDefinition() {}

	void				Save(Framework::CStream&);

	void				AddActor(const ACTOR_DEFINITION&);

	const ActorArray&	GetActors() const;

private:
	void				Load(Framework::CStream&);

	uint32				m_backgroundMusicId = 0;
	uint32				m_battleMusicId = 0;
	ActorArray			m_actors;
};
