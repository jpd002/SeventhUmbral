#pragma once

#include <thread>

class CLobbyServer
{
public:
					CLobbyServer();
	virtual			~CLobbyServer();

	void			Start();

private:
	void			ServerThreadProc();

	std::thread		m_serverThread;
};
