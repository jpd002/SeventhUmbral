#include <Windows.h>
#include <algorithm>
#include <tchar.h>
#include <psapi.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include "Launcher.h"
#include "Base64.h"
#include "../common/BLOWFISH.H"

static const uint32 g_encryptionTimePatchAddress = 0x9A15E3;
static const uint8 g_encryptionTimePatch[] = { 0xB8, 0x12, 0xE8, 0xE0, 0x50 };
static const uint32 g_lobbyHostNameAddress = 0xB90110;

static const unsigned int g_lobbyHostNamePatchSize = 0x14;

BOOL WINAPI InjectDll(__in LPCWSTR lpcwszDll, PROCESS_INFORMATION processInformation);
void PrintError(LPTSTR lpszFunction);

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

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	BOOL isMorethanSixteenThreads = false;
	KAFFINITY affinity = 0x00000000000000FF;


	if (siSysInfo.dwNumberOfProcessors >= 16 )
	{
		isMorethanSixteenThreads = true;
	}


	if (isMorethanSixteenThreads)
	{
		
		HANDLE currentProcess = GetCurrentProcess();
		SetProcessAffinityMask(currentProcess, affinity);
	}
	

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

	if (isMorethanSixteenThreads)
	{
		SetProcessAffinityMask(procInfo.hProcess, affinity);
		wchar_t selfdir[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, selfdir, MAX_PATH);
		PathRemoveFileSpec(selfdir);

		std::wstring dllPath = std::wstring(selfdir) + TEXT("\\AffinityInjector.dll");


		if (InjectDll(dllPath.c_str(), procInfo)) {
			printf("Dll was successfully injected.\n");
		}
		else {
			printf("Terminating the Injector app...");
		}
	}else
	{
		ResumeThread(procInfo.hThread);

		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);	
	}
	

	
}


BOOL WINAPI InjectDll(__in LPCWSTR lpcwszDll, PROCESS_INFORMATION processInformation)
{
	SIZE_T nLength;
	LPVOID lpLoadLibraryW = NULL;
	LPVOID lpRemoteString;
	STARTUPINFO             startupInfo;
	//PROCESS_INFORMATION     processInformation;

	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(STARTUPINFO);

	/*if (!CreateProcess(targetPath, NULL, NULL, NULL, FALSE,
		CREATE_SUSPENDED, NULL, NULL, &startupInfo, &processInformation))
	{
		PrintError(TEXT("CreateProcess"));
		return FALSE;
	}*/

	lpLoadLibraryW = GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), "LoadLibraryW");

	if (!lpLoadLibraryW)
	{
		PrintError(TEXT("GetProcAddress"));
		// close process handle
		CloseHandle(processInformation.hProcess);
		return FALSE;
	}

	nLength = wcslen(lpcwszDll) * sizeof(WCHAR);

	// allocate mem for dll name
	lpRemoteString = VirtualAllocEx(processInformation.hProcess, NULL, nLength + 1, MEM_COMMIT, PAGE_READWRITE);
	if (!lpRemoteString)
	{
		PrintError(TEXT("VirtualAllocEx"));

		// close process handle
		CloseHandle(processInformation.hProcess);

		return FALSE;
	}

	// write dll name
	if (!WriteProcessMemory(processInformation.hProcess, lpRemoteString, lpcwszDll, nLength, NULL)) {

		PrintError(TEXT("WriteProcessMemory"));
		// free allocated memory
		VirtualFreeEx(processInformation.hProcess, lpRemoteString, 0, MEM_RELEASE);

		// close process handle
		CloseHandle(processInformation.hProcess);

		return FALSE;
	}

	// call loadlibraryw
	HANDLE hThread = CreateRemoteThread(processInformation.hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)lpLoadLibraryW, lpRemoteString, NULL, NULL);

	if (!hThread) {
		PrintError(TEXT("CreateRemoteThread"));
	}
	else {
		WaitForSingleObject(hThread, 8000);

		//resume suspended process
		ResumeThread(processInformation.hThread);
	}

	//  free allocated memory
	VirtualFreeEx(processInformation.hProcess, lpRemoteString, 0, MEM_RELEASE);

	// close process handle
	CloseHandle(processInformation.hProcess);
	CloseHandle(processInformation.hThread);

	return TRUE;
}

void PrintError(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);

	wprintf(L"%s", lpDisplayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}