#pragma once

#include <map>
#include <vector>
#include "Types.h"
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

struct ZONE_DEFINITION
{
	typedef std::vector<ACTOR_DEFINITION> ActorArray;

	uint32		backgroundMusicId = 0;
	uint32		battleMusicId = 0;
	ActorArray	actors;
};

class CZoneDatabase
{
public:
									CZoneDatabase();
	virtual							~CZoneDatabase();

	void							Load();

	const ZONE_DEFINITION*			GetZone(uint32) const;
	const ZONE_DEFINITION*			GetDefaultZone() const;

	const ZONE_DEFINITION*			GetZoneOrDefault(uint32) const;

private:
	struct ZONE_DEF_LOCATION
	{
		uint32			zoneId;
		const char*		name;
	};

	typedef std::map<uint32, ZONE_DEFINITION> ZoneDefinitionMap;

	ZONE_DEFINITION					LoadZoneDefinition(Framework::CStream&);

	ZONE_DEFINITION					m_defaultZone;
	ZoneDefinitionMap				m_zones;
	static const ZONE_DEF_LOCATION	m_zoneLocations[];
};
