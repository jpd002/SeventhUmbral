#include <cassert>
#include "Clock.h"
#include "Instance.h"

CInstance::CInstance()
{
	m_lastUpdateTime = CClock::GetInstance().GetCurrentTime();
}

CInstance::~CInstance()
{

}

void CInstance::AddActor(ActorPtr&& actor)
{
	assert(actor->GetInstance() == nullptr);
	assert(m_actors.find(actor->GetId()) == std::end(m_actors));
	actor->SetInstance(this);
	m_actors.insert(std::make_pair(actor->GetId(), std::move(actor)));
}

CInstance::ActorPtr CInstance::RemoveActor(uint32 actorId)
{
	ActorPtr result;
	auto actorIterator = m_actors.find(actorId);
	assert(actorIterator != std::end(m_actors));
	result = std::move(actorIterator->second);
	result->SetInstance(nullptr);
	m_actors.erase(actorIterator);
	return std::move(result);
}

void CInstance::ClearAllActors()
{
	m_actors.clear();
}

void CInstance::Update()
{
	double currentTime = CClock::GetInstance().GetCurrentTime();
	float deltaTime = currentTime - m_lastUpdateTime;
	//Really bad wrap around check
	if(deltaTime < 0)
	{
		deltaTime = 16.f / 1000.f;
	}
	m_lastUpdateTime = currentTime;
	for(const auto& actorPair : m_actors)
	{
		actorPair.second->Update(deltaTime);
	}
}
