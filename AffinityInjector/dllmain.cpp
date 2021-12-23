// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "framework.h"

static BOOL (WINAPI* NativeSetProcessAffinityMask)(HANDLE hProcess, DWORD_PTR dwProcessAffinityMask) =
	SetProcessAffinityMask;
static DWORD_PTR (WINAPI* NativeSetThreadAffinityMask)(HANDLE hThread, DWORD_PTR dwThreadAffinityMask) =
	SetThreadAffinityMask;

static KAFFINITY affinity = 0x00000000000000FF;


BOOL WINAPI FakeSetProcessAffinityMask(HANDLE hProcess, DWORD_PTR dwProcessAffinityMask)
{
	return NativeSetProcessAffinityMask(hProcess, affinity);
}

DWORD_PTR WINAPI FakeSetThreadAffinityMask(HANDLE hThread, DWORD_PTR dwThreadAffinityMask)
{
	return NativeSetThreadAffinityMask(hThread, affinity);
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
)
{
	if (DetourIsHelperProcess())
	{
		return TRUE;
	}


	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)NativeSetProcessAffinityMask, FakeSetProcessAffinityMask);
		DetourAttach(&(PVOID&)NativeSetThreadAffinityMask, FakeSetThreadAffinityMask);
		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)NativeSetProcessAffinityMask, FakeSetProcessAffinityMask);
		DetourDetach(&(PVOID&)NativeSetThreadAffinityMask, FakeSetThreadAffinityMask);
		DetourTransactionCommit();
		break;
	}
	return TRUE;
}
