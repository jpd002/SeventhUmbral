#pragma once

#include "Singleton.h"
#include "AppearanceDatabase.h"
#include "ZoneDatabase.h"

class CGlobalData : public CSingleton<CGlobalData>
{
public:
	void						Prepare();
	const CZoneDatabase&		GetZoneDatabase() const;
	const CAppearanceDatabase&	GetWeaponAppearanceDatabase() const;

private:
	void						LoadWeaponAppearanceDatabase();

	CZoneDatabase				m_zoneDatabase;
	CAppearanceDatabase			m_weaponAppearanceDatabase;
};
