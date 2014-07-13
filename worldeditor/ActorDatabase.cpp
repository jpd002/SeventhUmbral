#include <memory>
#include "ActorDatabase.h"
#include "xml/Parser.h"
#include "xml/Utils.h"

CActorDatabase::CActorDatabase()
{

}

CActorDatabase::~CActorDatabase()
{

}

CActorDatabase CActorDatabase::CreateFromXml(Framework::CStream& stream)
{
	CActorDatabase result;
	auto documentNode = std::shared_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(stream));
	auto actorNodes = documentNode->SelectNodes("Actors/Actor");
	for(const auto& actorNode : actorNodes)
	{
		std::string positionString = Framework::Xml::GetAttributeStringValue(actorNode, "Position");

		ACTOR_DEFINITION actor;
		actor.baseModelId = Framework::Xml::GetAttributeIntValue(actorNode, "BaseModel");
		actor.topModelId = Framework::Xml::GetAttributeIntValue(actorNode, "TopModel");
		actor.position = Palleon::ParseVector3(positionString);
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
