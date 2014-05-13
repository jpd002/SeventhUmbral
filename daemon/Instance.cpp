#include <assert.h>
#include <time.h>
#include "Instance.h"

CInstance::CInstance()
{
	m_lastUpdateTime = clock();
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
	clock_t currentTime = clock();
	float deltaTime = static_cast<float>(currentTime - m_lastUpdateTime) / static_cast<float>(CLOCKS_PER_SEC);
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
