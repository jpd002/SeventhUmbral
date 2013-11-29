#include <WinInet.h>
#include <zlib.h>
#include "Utils.h"
#include "DownloaderService.h"
#include "StdStreamUtils.h"
#include "string_cast.h"

class CInternetHandle
{
public:
	CInternetHandle(HINTERNET handle)
		: m_handle(handle)
	{
	
	}

	~CInternetHandle()
	{
		if(m_handle != NULL)
		{
			InternetCloseHandle(m_handle);
		}
	}

	bool IsEmpty() const
	{
		return m_handle == NULL;
	}

	operator HINTERNET() const
	{
		return m_handle;
	}

private:
	CInternetHandle(const CInternetHandle&);
	CInternetHandle& operator =(const CInternetHandle&);

	HINTERNET m_handle;
};

CDownloaderService::CDownloaderService()
{

}

CDownloaderService::~CDownloaderService()
{

}

void CDownloaderService::Download(const std::string& srcUrl, const boost::filesystem::path& dstPath, uint32 fileSize, uint32 crc, const ContinuationFunction& continuation)
{
	DOWNLOADER_SERVICE_COMMAND command;
	command.srcUrl		= srcUrl;
	command.dstPath		= dstPath;
	command.fileSize	= fileSize;
	command.crc			= crc;
	m_downloadedSize = 0;
	m_downloadCancelled = false;
	LaunchCommand(command, continuation);
}

uint32 CDownloaderService::GetDownloadedSize() const
{
	return m_downloadedSize;
}

void CDownloaderService::CancelDownload()
{
	m_downloadCancelled = true;
}

DOWNLOADER_SERVICE_RESULT CDownloaderService::Execute(const DOWNLOADER_SERVICE_COMMAND& command)
{
	DOWNLOADER_SERVICE_RESULT result;

	try
	{
		if(!IsDownloadRequired(command))
		{
			result.succeeded = true;
			return result;
		}
	}
	catch(...)
	{
		//If there's an exception, just assume a download is required
	}

	try
	{
		{
			//Make sure output directory exists
			auto dstPathDir = command.dstPath;
			dstPathDir.remove_leaf();
			if(!boost::filesystem::exists(dstPathDir))
			{
				boost::filesystem::create_directories(dstPathDir);
			}
		}

		CInternetHandle inet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if(inet.IsEmpty()) return result;
		CInternetHandle inetUrl = InternetOpenUrl(inet, string_cast<std::tstring>(command.srcUrl).c_str(), NULL, 0, 0, NULL);
		if(inetUrl.IsEmpty()) return result;

		assert(m_downloadedSize == 0);
		auto downloadCrc = crc32(0, Z_NULL, 0);
		auto outputStream = Framework::CreateOutputStdStream(command.dstPath.native());
		while(!m_downloadCancelled)
		{
			const uint32 bufferSize = 0x10000;
			uint8 buffer[bufferSize];
			DWORD toRead = bufferSize;
			DWORD actualRead = 0;
			BOOL readResult = InternetReadFile(inetUrl, buffer, toRead, &actualRead);
			if(readResult == FALSE)
			{
				return result;
			}
			if(actualRead == 0)
			{
				result.succeeded = true;
				break;
			}
			outputStream.Write(buffer, actualRead);
			m_downloadedSize += actualRead;
			downloadCrc = crc32(downloadCrc, buffer, actualRead);
		}

		if(m_downloadCancelled)
		{
			result.succeeded = false;
		}

		if(m_downloadedSize != command.fileSize)
		{
			result.succeeded = false;
		}

		if(downloadCrc != command.crc)
		{
			result.succeeded = false;
		}
	}
	catch(...)
	{
	
	}
	return result;
}

bool CDownloaderService::IsDownloadRequired(const DOWNLOADER_SERVICE_COMMAND& command)
{
	if(!boost::filesystem::exists(command.dstPath))
	{
		return true;
	}
	auto fileSize = boost::filesystem::file_size(command.dstPath);
	if(fileSize != command.fileSize)
	{
		return true;
	}
	auto crc = Utils::ComputeFileCrc32(command.dstPath);
	if(crc != command.crc)
	{
		return true;
	}
	return false;
}
