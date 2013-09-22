#include <Windows.h>
#include <algorithm>
#include <tchar.h>
#include "Launcher.h"
#include "Base64.h"
#include "../common/BLOWFISH.H"

static const uint32 g_encryptionTimePatchAddress = 0x9A15E3;
static const uint8 g_encryptionTimePatch[] = { 0xB8, 0x12, 0xE8, 0xE0, 0x50 };
static const uint32 g_lobbyHostNameAddress = 0xB90110;

static const unsigned int g_lobbyHostNamePatchSize = 0x14;

static bool ApplyPatch(HANDLE hProcess, uint32 address, const uint8* patchBytes, uint32 patchSize)
{
	DWORD oldProtect = 0;

	auto remoteAddress = reinterpret_cast<void*>(address);

	if(!VirtualProtectEx(hProcess, remoteAddress, patchSize, PAGE_READWRITE, &oldProtect))
	{
		throw std::runtime_error("Failed to change page protection.");
	}

	SIZE_T numWritten = 0;

	if(!WriteProcessMemory(hProcess, remoteAddress, patchBytes, patchSize, &numWritten))
	{
		throw std::runtime_error("Failed to apply patch.");
	}

	if(numWritten != patchSize)
	{
		throw std::runtime_error("Failed to apply patch.");
	}

	if(!VirtualProtectEx(hProcess, remoteAddress, patchSize, oldProtect, &oldProtect))
	{
		throw std::runtime_error("Failed to restore page protection.");
	}

	return true;
}

static bool ApplyPatches(HANDLE hProcess, HANDLE hThread, const char* lobbyHostName)
{
	CONTEXT threadContext;
	memset(&threadContext, 0, sizeof(CONTEXT));
	threadContext.ContextFlags = CONTEXT_FULL;
	if(!GetThreadContext(hThread, &threadContext))
	{
		throw std::runtime_error("Failed to get thread context.");
	}

	void* imageBaseAddressPtr = reinterpret_cast<void*>(threadContext.Ebx + 8);
	DWORD imageBaseAddress = 0;
	SIZE_T numRead = 0;
	if(!ReadProcessMemory(hProcess, imageBaseAddressPtr, &imageBaseAddress, 4, &numRead))
	{
		throw std::runtime_error("Failed to get image base context.");
	}

	if(!ApplyPatch(hProcess, imageBaseAddress + g_encryptionTimePatchAddress,	g_encryptionTimePatch,								sizeof(g_encryptionTimePatch)))	return false;
	if(!ApplyPatch(hProcess, imageBaseAddress + g_lobbyHostNameAddress,			reinterpret_cast<const uint8*>(lobbyHostName),		strlen(lobbyHostName) + 1))		return false;

	return true;
}

void CLauncher::Launch(const char* workingDirectory, const char* lobbyHostName, const char* sessionId)
{
	if((strlen(lobbyHostName) + 1) > g_lobbyHostNamePatchSize)
	{
		throw std::runtime_error("Lobby host name too large.");
	}

	assert(strlen(sessionId) == 56);

	char commandLine[1024];
	uint32 currentTickCount = GetTickCount();
	sprintf(commandLine, " T =%d /LANG =en-us /REGION =2 /SERVER_UTC =1356916742 /SESSION_ID =%s", currentTickCount, sessionId);

	char encryptionKey[9];
	sprintf(encryptionKey, "%0.8x", currentTickCount & ~0xFFFF);
	InitializeBlowfish(encryptionKey, 8);

	size_t commandLineSize = strlen(commandLine) + 1;
	for(unsigned int i = 0; i < (commandLineSize & ~0x7); i += 8)
	{
		Blowfish_encipher(
			reinterpret_cast<uint32*>(commandLine + i), 
			reinterpret_cast<uint32*>(commandLine + i + 4));
	}

	auto encodedCommandLine = Framework::ToBase64(commandLine, commandLineSize);
	std::replace(std::begin(encodedCommandLine), std::end(encodedCommandLine), '+', '-');
	std::replace(std::begin(encodedCommandLine), std::end(encodedCommandLine), '/', '_');

	char completeCommandLine[1024];
	sprintf(completeCommandLine, "%s\\ffxivgame.exe sqex0002%s!////", workingDirectory, encodedCommandLine.c_str());

	STARTUPINFOA startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION procInfo = { 0 };

	BOOL processCreated = CreateProcessA(NULL, completeCommandLine, NULL, NULL, FALSE, CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS, NULL, workingDirectory, &startupInfo, &procInfo);
	if(!processCreated)
	{
		DWORD lastError = GetLastError();
		throw std::runtime_error("Failed to launch game executable.");
	}

	try
	{
		ApplyPatches(procInfo.hProcess, procInfo.hThread, lobbyHostName);
	}
	catch(...)
	{
		TerminateProcess(procInfo.hProcess, -1);
		throw;
	}

	ResumeThread(procInfo.hThread);

	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);
}
