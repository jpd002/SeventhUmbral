#include <boost/filesystem.hpp>
#include "StdStreamUtils.h"
#include <assert.h>
#include "string_format.h"

void ScrambleBuffer(uint8* buffer, size_t bufferSize)
{
	uint8* ptr = buffer;
	uint8* end = buffer + bufferSize - 1;
	while(ptr < end)
	{
		std::swap(*ptr, *end);
		ptr += 2;
		end -= 2;
	}
}

std::vector<uint8> TryOpenXml(const boost::filesystem::path& filePath)
{
	auto inputStream = Framework::CreateInputStdStream(filePath.string());

	uint32 srcLength = inputStream.GetLength();
	std::vector<uint8> srcData;

	srcData.resize(srcLength);
	inputStream.Read(srcData.data(), srcLength);

	if(srcData.size() == 0) return srcData;

	if(srcData[srcLength - 1] != 0xF1)
	{
		return srcData;
	}

	std::vector<uint8> dstData;
	uint32 dstLength = srcLength - 1;
	dstData.resize(dstLength);

	int32 encodedLength = srcLength - 1;
	memcpy(dstData.data(), srcData.data(), encodedLength);
	ScrambleBuffer(dstData.data(), encodedLength);

	int16 xA = static_cast<int16>(encodedLength * 7);
	int16 xB = *reinterpret_cast<int16*>(dstData.data() + 6) ^ 0x6C6D;

	int16* dstEnd = reinterpret_cast<int16*>(dstData.data() + encodedLength - 1);

	{
		int16* ptr = reinterpret_cast<int16*>(dstData.data());
		while(ptr < dstEnd)
		{
			(*ptr) ^= xA;
			ptr += 2;
		}
	}

	{
		int16* ptr = reinterpret_cast<int16*>(dstData.data() + 2);
		while(ptr < dstEnd)
		{
			(*ptr) ^= xB;
			ptr += 2;
		}
	}

	if(encodedLength & 1)
	{
		*(dstData.data() + encodedLength - 1) ^= static_cast<uint8>(xA);
		*(dstData.data() + encodedLength - 1) ^= static_cast<uint8>(xB);
	}

	return dstData;
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
		auto result = TryOpenXml(path);
		if((result.size() > 4) && 
			(result[0] == 0xEF) && (result[1] == 0xBB) && (result[2] == 0xBF) && (result[3] == 0x3C))
		{
			static int fileId = 0;
			printf("Found XML File: '%s'...\r\n", path.path().string().c_str());
			auto outputFileName = string_format("./output/file_%0.8d.xml", fileId++);
			auto outputStream = Framework::CStdStream(outputFileName.c_str(), "wb");
			outputStream.Write(result.data(), result.size());
		}
	}
}

int main(int argc, char** argv)
{
	boost::filesystem::path gamePath = "F:\\Games\\SquareEnix\\FINAL FANTASY XIV";
	auto dataPath = gamePath / "data";
	boost::filesystem::path xmlFilePath = dataPath / "03/A2/00/BC.DAT";
//	boost::filesystem::path xmlFilePath = dataPath / "01/03/00/42.DAT";
	TryOpenXml(xmlFilePath);

	return 0;
}
