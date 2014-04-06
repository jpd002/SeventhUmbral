#pragma once

#include "Singleton.h"
#include "ActorDatabase.h"

class CGlobalData : public CSingleton<CGlobalData>
{
public:
	void						Prepare();
	const CActorDatabase&		GetActorDatabase() const;

private:
	void						LoadActorDatabase();

	CActorDatabase				m_actorDatabase;
};
