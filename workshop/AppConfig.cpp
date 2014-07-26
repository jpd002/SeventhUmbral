#include "AppConfig.h"
#include "PathUtils.h"
#include "AppPreferences.h"
#include "Utf8.h"
#include "../common/GameInstallInfo.h"

#define BASE_DATA_PATH			(L"Seventh Umbral Workshop Data Files")
#define CONFIG_FILENAME			(L"config.xml")

CAppConfig::CAppConfig() 
: CConfig(BuildConfigPath())
{

}

CAppConfig::~CAppConfig()
{

}

Framework::CConfig::PathType CAppConfig::GetBasePath()
{
	auto result = Framework::PathUtils::GetPersonalDataPath() / BASE_DATA_PATH;
	return result;
}

Framework::CConfig::PathType CAppConfig::BuildConfigPath()
{
	auto userPath(GetBasePath());
	Framework::PathUtils::EnsurePathExists(userPath);
	return userPath / CONFIG_FILENAME;
}

void CAppConfig::PrepareGameLocation()
{
	CAppConfig::GetInstance().RegisterPreferenceString(PREF_WORKSHOP_GAME_LOCATION, "");

	auto currentGameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));

	//Already setup, don't go any further
	if(!currentGameLocation.empty()) return;

	auto detectedGameLocation = CGameInstallInfo::GetInstallPath();
	CAppConfig::GetInstance().SetPreferenceString(PREF_WORKSHOP_GAME_LOCATION, Framework::Utf8::ConvertTo(detectedGameLocation).c_str());

	CAppConfig::GetInstance().Save();
}
