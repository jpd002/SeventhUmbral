#include <memory>
#include "xml/Parser.h"
#include "xml/Writer.h"
#include "xml/Utils.h"
#include "math/MathStringUtils.h"
#include "ZoneDefinition.h"

#define ZONE_ATTRIB_BACKGROUNDMUSICID	"BackgroundMusicId"
#define ZONE_ATTRIB_BATTLEMUSICID		"BattleMusicId"

#define ACTOR_ATTRIB_ID					"Id"
#define ACTOR_ATTRIB_NAMESTRING			"NameString"
#define ACTOR_ATTRIB_BASEMODEL			"BaseModel"
#define ACTOR_ATTRIB_TOPMODEL			"TopModel"
#define ACTOR_ATTRIB_NAMESTRING			"NameString"
#define ACTOR_ATTRIB_POSITION			"Position"

CZoneDefinition::CZoneDefinition(Framework::CStream& stream)
{
	Load(stream);
}

void CZoneDefinition::AddActor(const ACTOR_DEFINITION& actor)
{
	m_actors.push_back(actor);
}

const CZoneDefinition::ActorArray& CZoneDefinition::GetActors() const
{
	return m_actors;
}

void CZoneDefinition::Load(Framework::CStream& stream)
{
	auto documentNode = std::unique_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(stream));
	auto zoneNode = documentNode->Select("Zone");
	if(zoneNode == nullptr)
	{
		throw std::runtime_error("Zone definition file doesn't contain a 'Zone' node.");
	}

	m_backgroundMusicId = Framework::Xml::GetAttributeIntValue(zoneNode, ZONE_ATTRIB_BACKGROUNDMUSICID);
	m_battleMusicId = Framework::Xml::GetAttributeIntValue(zoneNode, ZONE_ATTRIB_BATTLEMUSICID);

	auto actorNodes = zoneNode->SelectNodes("Actors/Actor");
	m_actors.reserve(actorNodes.size());
	for(const auto& actorNode : actorNodes)
	{
		ACTOR_DEFINITION actor;
		actor.id = Framework::Xml::GetAttributeIntValue(actorNode, ACTOR_ATTRIB_ID);
		actor.nameStringId = Framework::Xml::GetAttributeIntValue(actorNode, ACTOR_ATTRIB_NAMESTRING);
		actor.baseModelId = Framework::Xml::GetAttributeIntValue(actorNode, ACTOR_ATTRIB_BASEMODEL);
		actor.topModelId = Framework::Xml::GetAttributeIntValue(actorNode, ACTOR_ATTRIB_TOPMODEL);
		actor.position = MathStringUtils::ParseVector3(Framework::Xml::GetAttributeStringValue(actorNode, ACTOR_ATTRIB_POSITION));
		m_actors.push_back(actor);
	}
}

void CZoneDefinition::Save(Framework::CStream& stream)
{
	auto documentNode = std::make_unique<Framework::Xml::CNode>("", true);
	auto zoneNode = documentNode->InsertNode(new Framework::Xml::CNode("Zone", true));

	zoneNode->InsertAttribute(Framework::Xml::CreateAttributeIntValue(ZONE_ATTRIB_BACKGROUNDMUSICID, m_backgroundMusicId));
	zoneNode->InsertAttribute(Framework::Xml::CreateAttributeIntValue(ZONE_ATTRIB_BATTLEMUSICID, m_battleMusicId));

	auto actorsNode = zoneNode->InsertNode(new Framework::Xml::CNode("Actors", true));
	for(const auto& actor : m_actors)
	{
		auto actorNode = actorsNode->InsertNode(new Framework::Xml::CNode("Actor", true));
		actorNode->InsertAttribute(Framework::Xml::CreateAttributeIntValue(ACTOR_ATTRIB_ID, actor.id));
		actorNode->InsertAttribute(Framework::Xml::CreateAttributeIntValue(ACTOR_ATTRIB_NAMESTRING, actor.nameStringId));
		actorNode->InsertAttribute(Framework::Xml::CreateAttributeIntValue(ACTOR_ATTRIB_BASEMODEL, actor.baseModelId));
		actorNode->InsertAttribute(Framework::Xml::CreateAttributeIntValue(ACTOR_ATTRIB_TOPMODEL, actor.topModelId));
		actorNode->InsertAttribute(Framework::Xml::CreateAttributeStringValue(ACTOR_ATTRIB_POSITION, MathStringUtils::ToString(actor.position).c_str()));
	}

	Framework::Xml::CWriter::WriteDocument(stream, documentNode.get());
}
