#include <stdarg.h>
#include "Log.h"
#include "string_format.h"

CLog::CLog()
{

}

CLog::~CLog()
{

}

void CLog::LogDebug(const char* serviceName, const char* format, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, format);
	auto body = string_format(format, args);
	va_end(args);
	auto message = string_format("[D] %s: %s", serviceName, body.c_str());
	WriteToLog(message.c_str());
#endif
}

void CLog::LogMessage(const char* serviceName, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	auto body = string_format(format, args);
	va_end(args);
	auto message = string_format("[M] %s: %s", serviceName, body.c_str());
	WriteToLog(message.c_str());
}

void CLog::LogError(const char* serviceName, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	auto body = string_format(format, args);
	va_end(args);
	auto message = string_format("[E] %s: %s", serviceName, body.c_str());
	WriteToLog(message.c_str());
}

void CLog::WriteToLog(const char* message)
{
	puts(message);
}
