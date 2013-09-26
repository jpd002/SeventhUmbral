#include "DatabaseConnectionManager.h"
#include "AppConfig.h"
#include "Log.h"

#define LOG_NAME "DatabaseConnectionManager"

#define PREF_FFXIVD_DATABASE_ADDRESS	"ffxivd.database.address"
#define PREF_FFXIVD_DATABASE_USERNAME	"ffxivd.database.username"
#define PREF_FFXIVD_DATABASE_PASSWORD	"ffxivd.database.password"
#define PREF_FFXIVD_DATABASE_NAME		"ffxivd.database.name"

CDatabaseConnectionManager::CDatabaseConnectionManager()
{
	m_accessInfo.address		= CAppConfig::GetInstance().GetPreferenceString(PREF_FFXIVD_DATABASE_ADDRESS);
	m_accessInfo.userName		= CAppConfig::GetInstance().GetPreferenceString(PREF_FFXIVD_DATABASE_USERNAME);
	m_accessInfo.password		= CAppConfig::GetInstance().GetPreferenceString(PREF_FFXIVD_DATABASE_PASSWORD);
	m_accessInfo.databaseName	= CAppConfig::GetInstance().GetPreferenceString(PREF_FFXIVD_DATABASE_NAME);
}

CDatabaseConnectionManager::~CDatabaseConnectionManager()
{

}

Framework::MySql::CClient CDatabaseConnectionManager::CreateConnection()
{
	Framework::MySql::CClient result;
	try
	{
		result = Framework::MySql::CClient(
			m_accessInfo.address.c_str(), m_accessInfo.userName.c_str(), 
			m_accessInfo.password.c_str(), m_accessInfo.databaseName.c_str());
	}
	catch(const std::exception& excep)
	{
		CLog::GetInstance().LogError(LOG_NAME, "Failed to connect to database server: %s.", excep.what());
	}
	return result;
}
