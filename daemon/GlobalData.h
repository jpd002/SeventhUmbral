#pragma once

#include "Singleton.h"
#include "ActorDatabase.h"
#include "AppearanceDatabase.h"

class CGlobalData : public CSingleton<CGlobalData>
{
public:
	void						Prepare();
	const CActorDatabase&		GetActorDatabase() const;
	const CAppearanceDatabase&	GetWeaponAppearanceDatabase() const;

private:
	void						LoadActorDatabase();
	void						LoadWeaponAppearanceDatabase();

	CActorDatabase				m_actorDatabase;
	CAppearanceDatabase			m_weaponAppearanceDatabase;
};
