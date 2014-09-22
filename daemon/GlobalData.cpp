#include "GlobalData.h"
#include "StdStreamUtils.h"
#include "AppConfig.h"
#include "Log.h"

#define LOG_NAME "GlobalData"

void CGlobalData::Prepare()
{
	m_zoneDatabase.Load();
	LoadWeaponAppearanceDatabase();
}

const CZoneDatabase& CGlobalData::GetZoneDatabase() const
{
	return m_zoneDatabase;
}

const CAppearanceDatabase& CGlobalData::GetWeaponAppearanceDatabase() const
{
	return m_weaponAppearanceDatabase;
}

void CGlobalData::LoadWeaponAppearanceDatabase()
{
	auto configPath = CAppConfig::GetInstance().GetBasePath();
	auto weaponAppearanceDatabasePath = configPath / "ffxivd_weapon_appearances.xml";
	if(boost::filesystem::exists(weaponAppearanceDatabasePath))
	{
		auto inputStream = Framework::CreateInputStdStream(weaponAppearanceDatabasePath.native());
		m_weaponAppearanceDatabase = CAppearanceDatabase::CreateFromXml(inputStream);
	}
	else
	{
		CLog::GetInstance().LogMessage(LOG_NAME, "File '%s' doesn't exist. Not loading any weapon appearance data.", weaponAppearanceDatabasePath.string().c_str());
	}
}
