#include "FileManager.h"
#include "string_format.h"

boost::filesystem::path CFileManager::GetGamePath()
{
//	boost::filesystem::path gamePath = "F:\\Games\\SquareEnix\\FINAL FANTASY XIV";
	boost::filesystem::path gamePath = "C:\\Program Files (x86)\\SquareEnix\\FINAL FANTASY XIV";
	return gamePath;
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
