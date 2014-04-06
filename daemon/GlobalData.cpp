#include "GlobalData.h"
#include "StdStreamUtils.h"
#include "AppConfig.h"
#include "Log.h"

#define LOG_NAME "GlobalData"

void CGlobalData::Prepare()
{
	LoadActorDatabase();
}

const CActorDatabase& CGlobalData::GetActorDatabase() const
{
	return m_actorDatabase;
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
