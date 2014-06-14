#include "FileManager.h"
#include "string_format.h"

boost::filesystem::path CFileManager::m_gamePath;

boost::filesystem::path CFileManager::GetGamePath()
{
	return m_gamePath;
}

void CFileManager::SetGamePath(const boost::filesystem::path& gamePath)
{
	m_gamePath = gamePath;
}

boost::filesystem::path CFileManager::GetResourcePath(uint32 resourceId)
{
	auto gamePath = GetGamePath();
	auto dataPath = gamePath / "data";
	auto resourceIdName = string_format("%0.2X/%0.2X/%0.2X/%0.2X.DAT",
		(resourceId >> 24) & 0xFF,
		(resourceId >> 16) & 0xFF,
		(resourceId >>  8) & 0xFF,
		(resourceId >>  0) & 0xFF);
	return dataPath / resourceIdName;
}
