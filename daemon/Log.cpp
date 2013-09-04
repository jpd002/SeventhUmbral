#include "Log.h"
#include <stdio.h>
#include <stdarg.h>

CLog::CLog()
{

}

CLog::~CLog()
{

}

void CLog::LogDebug(const char* serviceName, const char* format, ...)
{
#ifdef _DEBUG
	printf("[Debug] %s: ", serviceName);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	puts("");
#endif
}

void CLog::LogMessage(const char* serviceName, const char* format, ...)
{
	printf("[Message] %s: ", serviceName);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	puts("");
}

void CLog::LogError(const char* serviceName, const char* format, ...)
{
	printf("[Error] %s: ", serviceName);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	puts("");
}
