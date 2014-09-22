#include "ZoneDatabase.h"
#include "AppConfig.h"
#include "string_format.h"
#include "StdStreamUtils.h"
#include "Log.h"
#include "xml/Parser.h"
#include "xml/Utils.h"

#define LOG_NAME "ZoneDatabase"

//////////////////////////////////////
//NOTE: This was pulled from 3D engine, we need to find a better place for it
//////////////////////////////////////

#include <sstream>
#include <boost/algorithm/string.hpp>

static float ParseFloat(const std::string& valueString)
{
	float result = 0;
	std::stringstream ss;
	ss.imbue(std::locale::classic());
	ss << valueString;
	ss >> result;
	return result;
}

static ActorVector3 ParseVector3(const std::string& vectorString)
{
	std::vector<std::string> components;
	boost::split(components, vectorString, boost::is_any_of(", "), boost::algorithm::token_compress_on);
	ActorVector3 result(0.f, 0.f, 0.f);
	if(components.size() != 3) return result;
	return std::make_tuple(
		ParseFloat(components[0]), 
		ParseFloat(components[1]),
		ParseFloat(components[2]));
}

//////////////////////////////////////
//////////////////////////////////////

const CZoneDatabase::ZONE_DEF_LOCATION CZoneDatabase::m_zoneLocations[] =
{
	{ 101,	"lanoscea"		},
	{ 102,	"coerthas"		},
	{ 103,	"blackshroud"	},
	{ 104,	"thanalan"		},
	{ 105,	"mordhona"		},
	{ 109,	"rivenroad"		}
};

CZoneDatabase::CZoneDatabase()
{
	m_defaultZone.backgroundMusicId = 0x39;
	m_defaultZone.battleMusicId = 0x0D;
}

CZoneDatabase::~CZoneDatabase()
{

}

void CZoneDatabase::Load()
{
	auto configPath = CAppConfig::GetInstance().GetBasePath();
	for(const auto& zoneLocation : m_zoneLocations)
	{
		auto zoneDefFilename = string_format("ffxivd_zone_%s.xml", zoneLocation.name);
		try
		{
			auto zoneDefPath = configPath / zoneDefFilename;
			if(boost::filesystem::exists(zoneDefPath))
			{
				auto inputStream = Framework::CreateInputStdStream(zoneDefPath.native());
				auto zone = LoadZoneDefinition(inputStream);
				m_zones.insert(std::make_pair(zoneLocation.zoneId, zone));
			}
			else
			{
				CLog::GetInstance().LogMessage(LOG_NAME, "File '%s' doesn't exist. Not loading any data for that zone.", zoneDefPath.string().c_str());
			}
		}
		catch(const std::exception& exception)
		{
			CLog::GetInstance().LogError(LOG_NAME, "Failed to load zone definition from '%s'. Reason: %s.", zoneDefFilename.c_str(), exception.what());
		}
	}
}

const ZONE_DEFINITION* CZoneDatabase::GetZone(uint32 zoneId) const
{
	auto zoneIterator = m_zones.find(zoneId);
	if(zoneIterator == std::end(m_zones))
	{
		return nullptr;
	}
	else
	{
		return &zoneIterator->second;
	}
}

const ZONE_DEFINITION* CZoneDatabase::GetDefaultZone() const
{
	return &m_defaultZone;
}

const ZONE_DEFINITION* CZoneDatabase::GetZoneOrDefault(uint32 zoneId) const
{
	if(auto zone = GetZone(zoneId))
	{
		return zone;
	}
	else
	{
		return GetDefaultZone();
	}
}

ZONE_DEFINITION CZoneDatabase::LoadZoneDefinition(Framework::CStream& stream)
{
	ZONE_DEFINITION result;
	auto documentNode = std::unique_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(stream));
	auto zoneNode = documentNode->Select("Zone");
	if(zoneNode == nullptr)
	{
		CLog::GetInstance().LogMessage(LOG_NAME, "Zone definition file doesn't contain a 'Zone' node.");
		return result;
	}

	result.backgroundMusicId = Framework::Xml::GetAttributeIntValue(zoneNode, "BackgroundMusicId");
	result.battleMusicId = Framework::Xml::GetAttributeIntValue(zoneNode, "BattleMusicId");

	auto actorNodes = zoneNode->SelectNodes("Actors/Actor");
	result.actors.reserve(actorNodes.size());
	for(const auto& actorNode : actorNodes)
	{
		ACTOR_DEFINITION actor;
		actor.id = Framework::Xml::GetAttributeIntValue(actorNode, "Id");
		actor.nameStringId = Framework::Xml::GetAttributeIntValue(actorNode, "NameString");
		actor.baseModelId = Framework::Xml::GetAttributeIntValue(actorNode, "BaseModel");
		actor.topModelId = Framework::Xml::GetAttributeIntValue(actorNode, "TopModel");
		actor.pos = ParseVector3(Framework::Xml::GetAttributeStringValue(actorNode, "Position"));
		result.actors.push_back(actor);
	}
	return result;
}
