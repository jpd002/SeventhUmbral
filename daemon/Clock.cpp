#ifdef __unix__
#include <time.h>
#endif
#include "Clock.h"

CClock::CClock()
{
#ifdef WIN32
	QueryPerformanceFrequency(&m_frequency);
#endif
}

CClock::~CClock()
{

}

double CClock::GetCurrentTime() const
{
#ifdef WIN32
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return static_cast<double>(count.QuadPart) / static_cast<double>(m_frequency.QuadPart);
#endif
#ifdef __unix__
	timespec timeSpec;
	clock_gettime(CLOCK_MONOTONIC, &timeSpec);
	return static_cast<double>(timeSpec.tv_sec) + (static_cast<double>(timeSpec.tv_nsec) / (1000.f * 1000.f * 1000.f));
#endif
}
