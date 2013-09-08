#pragma once

#include "Singleton.h"
#include "Config.h"
#include <boost/filesystem.hpp>

class CAppConfig : public Framework::CConfig, public CSingleton<CAppConfig>
{
public:
									CAppConfig();
	virtual							~CAppConfig();

	bool							IsConfigAvailable() const;

	static CConfig::PathType		GetBasePath();

private:
	static CConfig::PathType		BuildConfigPath();
};
