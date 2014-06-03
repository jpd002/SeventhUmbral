#pragma once

#include <boost/filesystem.hpp>
#include "Types.h"

class CFileManager
{
public:
	static boost::filesystem::path	GetGamePath();
	static boost::filesystem::path	GetResourcePath(uint32);
};
