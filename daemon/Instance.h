#pragma once

#include <memory>
#include <unordered_map>
#include <ctime>
#include "actors/Actor.h"

class CInstance
{
public:
	typedef std::unique_ptr<CActor> ActorPtr;

					CInstance();
	virtual			~CInstance();

	void			Update();

	void			AddActor(ActorPtr&&);
	ActorPtr		RemoveActor(uint32);
	void			ClearAllActors();

	template <typename ActorType>
	ActorType* GetActor(uint32 actorId) const
	{
		auto actorIterator = m_actors.find(actorId);
		if(actorIterator == std::end(m_actors))
		{
			return nullptr;
		}
		return dynamic_cast<ActorType*>(actorIterator->second.get());
	}

private:
	typedef std::unordered_map<uint32, ActorPtr> ActorMap;

	ActorMap		m_actors;
	clock_t			m_lastUpdateTime = 0;
};
