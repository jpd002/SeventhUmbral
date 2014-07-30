#include "GlobalData.h"
#include "StdStreamUtils.h"
#include "AppConfig.h"
#include "Log.h"

#define LOG_NAME "GlobalData"

void CGlobalData::Prepare()
{
	LoadActorDatabase();
	LoadWeaponAppearanceDatabase();
}

const CActorDatabase& CGlobalData::GetActorDatabase() const
{
	return m_actorDatabase;
}

const CAppearanceDatabase& CGlobalData::GetWeaponAppearanceDatabase() const
{
	return m_weaponAppearanceDatabase;
}

void CGlobalData::LoadActorDatabase()
{
	auto configPath = CAppConfig::GetInstance().GetBasePath();
	auto actorDatabasePath = configPath / "ffxivd_actors.xml";
	if(boost::filesystem::exists(actorDatabasePath))
	{
		auto inputStream = Framework::CreateInputStdStream(actorDatabasePath.native());
		m_actorDatabase = CActorDatabase::CreateFromXml(inputStream);
	}
	else
	{
		CLog::GetInstance().LogMessage(LOG_NAME, "File '%s' doesn't exist. Not loading any actor data.", actorDatabasePath.string().c_str());
	}
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
