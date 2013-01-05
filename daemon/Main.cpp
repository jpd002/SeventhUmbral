#include <stdio.h>
#include <WinSock2.h>
#include "LobbyServer.h"

int main(int argc, char** argv)
{
	WSADATA wsaData;
	if(WSAStartup(WINSOCK_VERSION, &wsaData))
	{
		printf("Failed to initialize WSA.\r\n");
		return -1;
	}

	CLobbyServer lobbyServer;
	lobbyServer.Start();

	Sleep(INFINITE);

	WSACleanup();
}
