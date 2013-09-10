#pragma once

#include "Stream.h"
#include <map>

class CServerDefs
{
public:
	struct SERVER
	{
		std::string		address;
		std::string		loginUrl;
	};
	typedef std::map<std::string, SERVER> ServerMap;

							CServerDefs();
	virtual					~CServerDefs();

	const ServerMap&		GetServers() const;

	void					Read(Framework::CStream&);

private:
	ServerMap				m_servers;
};
