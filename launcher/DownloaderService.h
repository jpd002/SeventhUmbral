#pragma once

#include "AsyncService.h"
#include <boost/filesystem.hpp>

struct DOWNLOADER_SERVICE_COMMAND
{
	std::string						srcUrl;
	boost::filesystem::path			dstPath;
	uint32							fileSize;
	uint32							crc;
};

struct DOWNLOADER_SERVICE_RESULT
{
	bool succeeded = false;
};

class CDownloaderService : public CAsyncService<DOWNLOADER_SERVICE_COMMAND, DOWNLOADER_SERVICE_RESULT>
{
public:
											CDownloaderService();
	virtual									~CDownloaderService();
	
	void									Download(const std::string&, const boost::filesystem::path&, uint32, uint32, const ContinuationFunction&);
	
	uint32									GetDownloadedSize() const;
	void									CancelDownload();

protected:
	virtual DOWNLOADER_SERVICE_RESULT		Execute(const DOWNLOADER_SERVICE_COMMAND&) override;

private:
	bool									IsDownloadRequired(const DOWNLOADER_SERVICE_COMMAND&);

	uint32									m_downloadedSize = 0;
	bool									m_downloadCancelled = false;
};
