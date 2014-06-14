#pragma once

#include <boost/filesystem.hpp>
#include "Types.h"

class CFileManager
{
public:
	static boost::filesystem::path	GetGamePath();
	static void						SetGamePath(const boost::filesystem::path&);

	static boost::filesystem::path	GetResourcePath(uint32);

private:
	static boost::filesystem::path	m_gamePath;
};
