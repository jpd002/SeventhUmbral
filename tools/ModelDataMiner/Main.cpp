#include <boost/filesystem.hpp>
#include <assert.h>
#include "StdStreamUtils.h"
#include "string_format.h"
#include "MapLayout.h"

bool TryOpenPwib(const boost::filesystem::path& filePath)
{
	auto inputStream = Framework::CreateInputStdStream(filePath.string());

	uint8 header[0x10];
	inputStream.Read(header, 0x10);
	if(header[0] == 0x4D && header[1] == 0x61 && header[2] == 0x70 && header[3] == 0x4C)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AnalyseDirectory(const boost::filesystem::path& directoryPath)
{
	for(boost::filesystem::directory_iterator pathIterator(directoryPath); 
		pathIterator != boost::filesystem::directory_iterator(); pathIterator++)
	{
		const auto& path = *pathIterator;
		if(boost::filesystem::is_directory(path))
		{
			AnalyseDirectory(path);
			continue;
		}
		bool result = TryOpenPwib(path);
		if(result)
		{
			printf("Found PWIB File: '%s'...\r\n", path.path().string().c_str());
		}
	}
}

int main(int argc, char** argv)
{
	boost::filesystem::path gamePath = "F:\\Games\\SquareEnix\\FINAL FANTASY XIV";
	auto dataPath = gamePath / "data";

	auto mapLayoutPath = dataPath / "A0/9B/00/02.DAT";
//	auto mapLayoutPath = dataPath / "92/23/00/00.DAT";

	CMapLayout mapLayout(Framework::CreateInputStdStream(mapLayoutPath.native()));

//	AnalyseDirectory(dataPath);


	return 0;
}
