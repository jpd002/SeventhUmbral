#pragma once

#include "Singleton.h"
#include "Types.h"

#ifdef WIN32
#include <Windows.h>
#endif

class CClock : public CSingleton<CClock>
{
public:
					CClock();
	virtual			~CClock();

	double			GetCurrentTime() const;

private:
#ifdef WIN32
	LARGE_INTEGER	m_frequency;
#endif
};
