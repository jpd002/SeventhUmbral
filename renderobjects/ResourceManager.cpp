#include "ResourceManager.h"
#include "../dataobjects/FileManager.h"
#include "StdStreamUtils.h"

void CResourceManager::LoadResource(uint32 resourceId, const std::string& resourceName)
{
	auto resourcePath = CFileManager::GetResourcePath(resourceId);
	auto inputStream = Framework::CreateInputStdStream(resourcePath.native());
	auto resourceNode = CSectionLoader::ReadSection(ResourceNodePtr(), inputStream);
	m_resources.insert(std::make_pair(resourceName, resourceNode));
}

ResourceNodePtr CResourceManager::GetResource(const std::string& resourceName) const
{
	auto resourceIterator = m_resources.find(resourceName);
	if(resourceIterator == std::end(m_resources)) return ResourceNodePtr();
	return resourceIterator->second;
}
