#pragma once

#include "Singleton.h"

class CLog : public CSingleton<CLog>
{
public:
				CLog();
	virtual		~CLog();

	void		LogDebug(const char*, const char*, ...);
	void		LogMessage(const char*, const char*, ...);
	void		LogError(const char*, const char*, ...);
};
