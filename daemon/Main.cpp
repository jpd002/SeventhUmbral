#include <stdio.h>
#include "SocketDef.h"
#include "LobbyServer.h"
#include "GameServer.h"

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

	CGameServer gameServer;
	gameServer.Start();

	Sleep(INFINITE);

	WSACleanup();
}
