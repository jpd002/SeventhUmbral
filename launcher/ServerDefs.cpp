#include "ServerDefs.h"
#include "xml/Parser.h"
#include "xml/FilteringNodeIterator.h"
#include "xml/Utils.h"
#include <memory>

CServerDefs::CServerDefs()
{

}

CServerDefs::~CServerDefs()
{

}

const CServerDefs::ServerMap& CServerDefs::GetServers() const
{
	return m_servers;
}

void CServerDefs::Read(Framework::CStream& inputStream)
{
	m_servers.clear();
	auto documentNode = std::unique_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(inputStream));
	for(const auto& serverNode : documentNode->SelectNodes("Servers/Server"))
	{
		std::string name = Framework::Xml::GetAttributeStringValue(serverNode, "Name");
		SERVER server;
		server.address = Framework::Xml::GetAttributeStringValue(serverNode, "Address");
		server.loginUrl = Framework::Xml::GetAttributeStringValue(serverNode, "LoginUrl");
		m_servers.insert(std::make_pair(name, server));
	}
}
