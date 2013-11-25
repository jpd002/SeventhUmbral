#include <stdexcept>
#include <thread>
#include <zlib.h>
#include "PatchFile.h"
#include "PathUtils.h"
#include "StdStreamUtils.h"
#include "string_format.h"

CPatchFile::CPatchFile(const boost::filesystem::path& gameLocationPath)
: m_gameLocationPath(gameLocationPath)
{

}

CPatchFile::~CPatchFile()
{

}

PATCH_RESULT CPatchFile::Execute(Framework::CStream& stream, const boost::filesystem::path& gameLocationPath)
{
	try
	{
		CPatchFile patchFile(gameLocationPath);
		patchFile.DoExecute(stream);
		return patchFile.GetResult();
	}
	catch(...)
	{
		PATCH_RESULT result;
		result.succeeded = false;
		return result;
	}
}

const PATCH_RESULT& CPatchFile::GetResult() const
{
	return m_result;
}

void CPatchFile::DoExecute(Framework::CStream& stream)
{
	uint8 patchHeader[0x10];
	stream.Read(patchHeader, sizeof(patchHeader));

	if(
		patchHeader[0] != 0x91 && 
		patchHeader[1] != 'Z' && 
		patchHeader[2] != 'I' &&
		patchHeader[3] != 'P' &&
		patchHeader[4] != 'A' && 
		patchHeader[5] != 'T' &&
		patchHeader[6] != 'C' &&
		patchHeader[7] != 'H')
	{
		throw std::runtime_error("Invalid patch file.");
	}

	while(1)
	{
		char command[5];
		stream.Read(command, 4);
		command[4] = 0;
		if(stream.IsEOF()) break;
		if(!strcmp(command, "FHDR"))
		{
			ExecuteFHDR(stream);
		}
		else if(!strcmp(command, "DIFF"))
		{
			ExecuteDIFF(stream);
		}
		else if(!strcmp(command, "HIST"))
		{
			ExecuteHIST(stream);
		}
		else if(!strcmp(command, "APLY"))
		{
			ExecuteAPLY(stream);
		}
		else if(!strcmp(command, "ADIR"))
		{
			ExecuteADIR(stream);
		}
		else if(!strcmp(command, "DELD"))
		{
			ExecuteDELD(stream);
		}
		else if(!strcmp(command, "ETRY"))
		{
			ExecuteETRY(stream);
		}
		else
		{
			throw std::runtime_error("Unhandled command encountered.");
		}
		if(stream.IsEOF()) break;
	}

	m_result.succeeded = true;
}

void CPatchFile::ExecuteFHDR(Framework::CStream& stream)
{
	//Version number?
	uint32 unknown = stream.Read32_MSBF();
	assert(unknown == 0x0200);
}

void CPatchFile::ExecuteDIFF(Framework::CStream& stream)
{
	uint32 values[5];
	stream.Read(values, sizeof(values));
}

void CPatchFile::ExecuteHIST(Framework::CStream& stream)
{
	uint32 values[5];
	stream.Read(values, sizeof(values));
}

void CPatchFile::ExecuteAPLY(Framework::CStream& stream)
{
	uint32 values[5];
	stream.Read(values, sizeof(values));
}

void CPatchFile::ExecuteADIR(Framework::CStream& stream)
{
	uint32 pathSize = stream.Read32_MSBF();
	std::vector<char> pathData(pathSize);
	stream.Read(pathData.data(), pathSize);
	std::string path(std::begin(pathData), std::end(pathData));

	uint32 otherData[4];
	stream.Read(otherData, sizeof(otherData));

	auto fullDirPath = m_gameLocationPath / path;
	fullDirPath.make_preferred();

	if(boost::filesystem::exists(fullDirPath))
	{
		m_result.messages.push_back(string_format("Warning: Directory '%s' creation requested but directory already exists.", 
			fullDirPath.string().c_str()
		));
	}
	else
	{
		boost::filesystem::create_directories(fullDirPath);
	}
}

void CPatchFile::ExecuteDELD(Framework::CStream& stream)
{
	uint32 pathSize = stream.Read32_MSBF();
	std::vector<char> pathData(pathSize);
	stream.Read(pathData.data(), pathSize);
	std::string path(std::begin(pathData), std::end(pathData));

	uint32 otherData[4];
	stream.Read(otherData, sizeof(otherData));

	auto fullDirPath = m_gameLocationPath / path;
	fullDirPath.make_preferred();

	if(!boost::filesystem::exists(fullDirPath))
	{
		m_result.messages.push_back(string_format("Warning: Directory '%s' deletion requested but directory doesn't exist.", 
			fullDirPath.string().c_str()
		));
	}
	else
	{
		boost::filesystem::remove_all(fullDirPath);
	}
}

void CPatchFile::ExecuteETRY(Framework::CStream& inputStream)
{
	uint32 pathSize = inputStream.Read32_MSBF();
	std::vector<char> pathData(pathSize);
	inputStream.Read(pathData.data(), pathSize);
	std::string path(std::begin(pathData), std::end(pathData));

	auto fullFilePath = m_gameLocationPath / path;
	auto fullFileDirectory = fullFilePath;
	fullFileDirectory.remove_leaf();
	fullFileDirectory.make_preferred();
	fullFilePath.make_preferred();

	if(!boost::filesystem::exists(fullFileDirectory))
	{
		m_result.messages.push_back(string_format("Warning: Directory '%s' doesn't exist. Creating.", 
			fullFileDirectory.string().c_str()
		));
		boost::filesystem::create_directories(fullFileDirectory);
	}

	if(!boost::filesystem::exists(fullFilePath))
	{
		m_result.messages.push_back(string_format("Warning: File '%s' doesn't exist. Creating.", fullFilePath.string().c_str()));
	}

	uint32 itemCount = inputStream.Read32_MSBF();
	for(unsigned int i = 0; i < itemCount; i++)
	{
		//0x41 = last hash, 0x44 = first hash, 0x4D = both hashes?
		uint32 hashMode = inputStream.Read32();
		assert(hashMode == 0x41 || hashMode == 0x44 || hashMode == 0x4D);

		uint8 srcFileHash[0x14];
		uint8 dstFileHash[0x14];
		inputStream.Read(srcFileHash, sizeof(srcFileHash));
		inputStream.Read(dstFileHash, sizeof(dstFileHash));

		//4E is no compression
		//5A is zlib compression
		uint32 compressionMode = inputStream.Read32();
		assert((compressionMode == 0x4E) || (compressionMode == 0x5A));

		uint32 compressedFileSize = inputStream.Read32_MSBF();
		uint32 previousFileSize = inputStream.Read32_MSBF();
		uint32 newFileSize = inputStream.Read32_MSBF();

		if(i != (itemCount - 1))
		{
			assert(compressedFileSize == 0);
		}
		if(compressedFileSize == 0) continue;

		//Data starts here
		{
			//Retrying here because explorer.exe can sometimes open the ffxiv*.exe files to load
			//the icons making the open operation fail if we need to patch it again.
			
			auto outputStream = CreateOutputStdStreamWithRetry(fullFilePath.native());
			if(compressionMode == 0x4E)
			{
				ExtractUncompressed(outputStream, inputStream, compressedFileSize);
			}
			else if(compressionMode == 0x5A)
			{
				ExtractCompressed(outputStream, inputStream, compressedFileSize);
			}
			else
			{
				throw std::runtime_error("Unknown compression type.");
			}
		}
	}

	inputStream.Seek(0x08, Framework::STREAM_SEEK_CUR);
}

Framework::CStdStream CPatchFile::CreateOutputStdStreamWithRetry(const boost::filesystem::path& filePath)
{
	int retryCount = 0;
	while(1)
	{
		try
		{
			auto result = Framework::CreateOutputStdStream(filePath.native());
			return result;
		}
		catch(...)
		{
			if(retryCount == 5)
			{
				throw;
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
			retryCount++;
		}
	}
}

void CPatchFile::ExtractUncompressed(Framework::CStream& outputStream, Framework::CStream& inputStream, uint32 compressedSize)
{
	const uint32 bufferSize = 0x4000;
	uint8 buffer[bufferSize];
	while(compressedSize != 0)
	{
		uint32 toRead = std::min<uint32>(bufferSize, compressedSize);
		inputStream.Read(buffer, toRead);
		outputStream.Write(buffer, toRead);
		compressedSize -= toRead;
	}
}

void CPatchFile::ExtractCompressed(Framework::CStream& outputStream, Framework::CStream& inputStream, uint32 compressedSize)
{
	z_stream zStream;
	zStream.zalloc = Z_NULL;
	zStream.zfree = Z_NULL;
	zStream.opaque = Z_NULL;
	zStream.avail_in = 0;
	zStream.next_in = Z_NULL;
	if(inflateInit(&zStream) != Z_OK)
	{
		throw std::runtime_error("zlib stream initialization error.");
	}

	const uint32 bufferSize = 0x4000;
	uint8 inBuffer[bufferSize];
	uint8 outBuffer[bufferSize];

	while(1)
	{
		if(zStream.avail_in == 0)
		{
			if(compressedSize == 0)
			{
				//EOF
				break;
			}
			uint32 toRead = std::min<uint32>(bufferSize, compressedSize);
			inputStream.Read(inBuffer, toRead);
			compressedSize -= toRead;
			zStream.avail_in = toRead;
			zStream.next_in = inBuffer;
		}

		zStream.avail_out = bufferSize;
		zStream.next_out = outBuffer;

		int ret = inflate(&zStream, Z_NO_FLUSH);
		if(ret < 0)
		{
			throw std::runtime_error("Error occured while inflating.");
			break;
		}

		int have = bufferSize - zStream.avail_out;
		outputStream.Write(outBuffer, have);
		if(ret == Z_STREAM_END)
		{
			break;
		}
	} 

	inflateEnd(&zStream);
}
