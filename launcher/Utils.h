#pragma once

#include <boost/filesystem.hpp>

namespace Utils
{
	uint32			ComputeFileCrc32(const boost::filesystem::path&);
	std::tstring	GetGameLocationFromInstallInfo();
	std::tstring	BrowseForFolder(HWND, const TCHAR*);
	std::tstring	BrowseForGameLocation(HWND);
	bool			IsValidGameLocationPath(const boost::filesystem::path&);
	bool			IsProcessElevated();
}
