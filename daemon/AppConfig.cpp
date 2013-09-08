#include "AppConfig.h"
#include "PathUtils.h"

#define BASE_DATA_PATH			(L"Seventh Umbral Server Data Files")
#define CONFIG_FILENAME			(L"config.xml")

CAppConfig::CAppConfig() 
: CConfig(BuildConfigPath(), true)
{

}

CAppConfig::~CAppConfig()
{

}

bool CAppConfig::IsConfigAvailable() const
{
	auto configFilePath = BuildConfigPath();
	return boost::filesystem::exists(configFilePath);
}

boost::filesystem::path CAppConfig::GetBasePath()
{
#ifdef __unix__
	return boost::filesystem::path("/usr/local/etc/ffxivd");
#else
	return Framework::PathUtils::GetPersonalDataPath() / BASE_DATA_PATH;
#endif
}

Framework::CConfig::PathType CAppConfig::BuildConfigPath()
{
	return GetBasePath() / CONFIG_FILENAME;
}
