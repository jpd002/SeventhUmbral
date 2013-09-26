#pragma once

#include "Singleton.h"
#include "mysql/Client.h"
#include "mysql/AccessInfo.h"

class CDatabaseConnectionManager : public CSingleton<CDatabaseConnectionManager>
{
public:
										CDatabaseConnectionManager();
	virtual								~CDatabaseConnectionManager();

	Framework::MySql::CClient			CreateConnection();

private:
	Framework::MySql::CAccessInfo		m_accessInfo;
};
