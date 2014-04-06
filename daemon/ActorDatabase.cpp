#include <memory>
#include "ActorDatabase.h"
#include "xml/Parser.h"
#include "xml/Utils.h"

//////////////////////////////////////
//NOTE: This was pulled from 3D engine, we need to find a better place for it
//////////////////////////////////////

#include <sstream>
#include <boost/algorithm/string.hpp>

float ParseFloat(const std::string& valueString)
{
	float result = 0;
	std::stringstream ss;
	ss.imbue(std::locale::classic());
	ss << valueString;
	ss >> result;
	return result;
}

ActorVector3 ParseVector3(const std::string& vectorString)
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

CActorDatabase::CActorDatabase()
{

}

CActorDatabase::~CActorDatabase()
{

}

CActorDatabase CActorDatabase::CreateFromXml(Framework::CStream& stream)
{
	CActorDatabase result;
	auto documentNode = std::unique_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(stream));
	auto actorNodes = documentNode->SelectNodes("Actors/Actor");
	for(const auto& actorNode : actorNodes)
	{
		ACTOR_DEFINITION actor;
		actor.id = Framework::Xml::GetAttributeIntValue(actorNode, "Id");
		actor.nameStringId = Framework::Xml::GetAttributeIntValue(actorNode, "NameString");
		actor.baseModelId = Framework::Xml::GetAttributeIntValue(actorNode, "BaseModel");
		actor.topModelId = Framework::Xml::GetAttributeIntValue(actorNode, "TopModel");
		actor.pos = ParseVector3(Framework::Xml::GetAttributeStringValue(actorNode, "Position"));
		result.AddActor(actor);
	}
	return result;
}

const CActorDatabase::ActorArray& CActorDatabase::GetActors() const
{
	return m_actors;
}

void CActorDatabase::AddActor(const ACTOR_DEFINITION& actor)
{
	m_actors.push_back(actor);
}
