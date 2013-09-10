#include "LauncherWindow.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CoInitialize(NULL);
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox(NULL, _T("Failed to initialize WinSock."), NULL, MB_ICONERROR);
		return -1;
	}
	CLauncherWindow launcherWindow;
	launcherWindow.DoModal();
	WSACleanup();
	return 0;
}
