#include <Windows.h>
#include "../common/BLOWFISH.H"
#include "Base64.h"
#include <algorithm>
#include <tchar.h>

#define APP_TITLE	_T("FFXIV Launcher")

static const uint32 g_encryptionTimePatchAddress = 0x9A15E3;
static const uint8 g_encryptionTimePatch[] = { 0xB8, 0x12, 0xE8, 0xE0, 0x50 };
static const uint32 g_lobbyHostNameAddress = 0xB90110;
static const uint8 g_lobbyHostNamePatch[] = { 'l', 'o', 'c', 'a', 'l', 'h', 'o', 's', 't', 0x00 };

bool ApplyPatch(HANDLE hProcess, uint32 address, const uint8* patchBytes, uint32 patchSize)
{
	DWORD oldProtect = 0;

	auto remoteAddress = reinterpret_cast<void*>(address);

	if(!VirtualProtectEx(hProcess, remoteAddress, patchSize, PAGE_READWRITE, &oldProtect))
	{
		MessageBox(NULL, _T("Failed to change page protection."), APP_TITLE, 16);
		return false;
	}

	SIZE_T numWritten = 0;

	if(!WriteProcessMemory(hProcess, remoteAddress, patchBytes, patchSize, &numWritten))
	{
		MessageBox(NULL, _T("Failed to apply patch."), APP_TITLE, 16);
		return false;
	}

	if(numWritten != patchSize)
	{
		MessageBox(NULL, _T("Failed to apply patch."), APP_TITLE, 16);
		return false;
	}

	if(!VirtualProtectEx(hProcess, remoteAddress, patchSize, oldProtect, &oldProtect))
	{
		MessageBox(NULL, _T("Failed to restore page protection."), APP_TITLE, 16);
		return false;
	}

	return true;
}

bool ApplyPatches(HANDLE hProcess, HANDLE hThread)
{
	CONTEXT threadContext;
	memset(&threadContext, 0, sizeof(CONTEXT));
	threadContext.ContextFlags = CONTEXT_FULL;
	if(!GetThreadContext(hThread, &threadContext))
	{
		MessageBox(NULL, _T("Failed to get thread context."), APP_TITLE, 16);
		return false;
	}

	void* imageBaseAddressPtr = reinterpret_cast<void*>(threadContext.Ebx + 8);
	DWORD imageBaseAddress = 0;
	SIZE_T numRead = 0;
	if(!ReadProcessMemory(hProcess, imageBaseAddressPtr, &imageBaseAddress, 4, &numRead))
	{
		MessageBox(NULL, _T("Failed to get image base context."), APP_TITLE, 16);
		return false;
	}

	if(!ApplyPatch(hProcess, imageBaseAddress + g_encryptionTimePatchAddress,	g_encryptionTimePatch,	sizeof(g_encryptionTimePatch)))	return false;
	if(!ApplyPatch(hProcess, imageBaseAddress + g_lobbyHostNameAddress,			g_lobbyHostNamePatch,	sizeof(g_lobbyHostNamePatch)))	return false;

	return true;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	char commandLine[1024];
	uint32 currentTickCount = GetTickCount();
	sprintf(commandLine, " T =%d /LANG =en-us /REGION =2 /SERVER_UTC =1356916742 /SESSION_ID =c45c427aabbf5bfeec6eb317678f324d7716e22e1e51c5682b03debd", currentTickCount);

	char encryptionKey[9];
	sprintf(encryptionKey, "%0.8x", currentTickCount & ~0xFFFF);
	InitializeBlowfish(encryptionKey, 8);

	size_t commandLineSize = strlen(commandLine) + 1;
	for(unsigned int i = 0; i < (commandLineSize & ~0x7); i += 8)
	{
		Blowfish_encipher(
			reinterpret_cast<unsigned long*>(commandLine + i), 
			reinterpret_cast<unsigned long*>(commandLine + i + 4));
	}

	auto encodedCommandLine = Framework::ToBase64(commandLine, commandLineSize);
	std::replace(std::begin(encodedCommandLine), std::end(encodedCommandLine), '+', '-');
	std::replace(std::begin(encodedCommandLine), std::end(encodedCommandLine), '/', '_');

	char completeCommandLine[1024];
	sprintf(completeCommandLine, "ffxivgame.exe sqex0002%s!////", encodedCommandLine.c_str());

	STARTUPINFOA startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION procInfo = { 0 };

	BOOL processCreated = CreateProcessA(NULL, completeCommandLine, NULL, NULL, FALSE, CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &procInfo);
	if(!processCreated)
	{
		DWORD lastError = GetLastError();
		MessageBox(NULL, _T("Failed to launch FFXIV."), APP_TITLE, 16);
		return -1;
	}

	if(!ApplyPatches(procInfo.hProcess, procInfo.hThread))
	{
		TerminateProcess(procInfo.hProcess, -1);
		return -1;
	}

	ResumeThread(procInfo.hThread);

	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);

	return 0;
}
