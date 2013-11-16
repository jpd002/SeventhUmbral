#pragma once

#include "ResourceDefs.h"
#include "Singleton.h"
#include <map>

class CResourceManager : public CSingleton<CResourceManager>
{
public:
	void				LoadResource(uint32, const std::string&);
	ResourceNodePtr		GetResource(const std::string&) const;

private:
	typedef std::map<std::string, ResourceNodePtr> ResourceMap;

	ResourceMap		m_resources;
};
