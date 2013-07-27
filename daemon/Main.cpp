#include <stdio.h>
#include "SocketDef.h"
#include "LobbyServer.h"
#include "GameServer.h"

int main(int argc, char** argv)
{
#ifdef WIN32
	WSADATA wsaData;
	if(WSAStartup(WINSOCK_VERSION, &wsaData))
	{
		printf("Failed to initialize WSA.\r\n");
		return -1;
	}
#endif

	CLobbyServer lobbyServer;
	lobbyServer.Start();

	CGameServer gameServer;
	gameServer.Start();

	while(1) std::this_thread::sleep_for(std::chrono::milliseconds(10000));

#ifdef WIN32
	WSACleanup();
#endif
}
