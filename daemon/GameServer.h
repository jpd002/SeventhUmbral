#pragma once

#include <thread>

class CGameServer
{
public:
					CGameServer();
	virtual			~CGameServer();

	void			Start();

	enum
	{
		GAME_SERVER_PORT = 54992
	};

private:
	void			ServerThreadProc();

	std::thread		m_serverThread;
};
