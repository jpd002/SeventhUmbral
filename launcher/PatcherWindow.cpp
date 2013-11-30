#include "PatcherWindow.h"
#include "resource.h"
#include "string_format.h"
#include "AppDef.h"

#define UNIT_KILOBYTE	(0x400)
#define TIMER_RATE		(400)

const char* g_patchUrlBase = "https://ffxivpatches.s3.amazonaws.com/";

const CPatcherWindow::DownloadInfoArray CPatcherWindow::m_downloads =
{
	{ "2d2a390f/patch/D2010.09.18.0000.patch", 0x00550467,	0x47DDE5ED },

	{ "48eca647/patch/D2010.09.19.0000.patch", 444398866,	0xD55C7ACD },
	{ "48eca647/patch/D2010.09.23.0000.patch", 6907277,		0xCA135D55 },
	{ "48eca647/patch/D2010.09.28.0000.patch", 18803280,	0xB19B32FE },

	{ "48eca647/patch/D2010.10.07.0001.patch", 19226330,	0xD6118CEE },
	{ "48eca647/patch/D2010.10.14.0000.patch", 19464329,	0x34BF6A99 },
	{ "48eca647/patch/D2010.10.22.0000.patch", 19778252,	0x2543DB5C },
	{ "48eca647/patch/D2010.10.26.0000.patch", 19778391,	0x20F94876 },

	{ "48eca647/patch/D2010.11.25.0002.patch", 250718651,	0x5FBB5B24 },
	{ "48eca647/patch/D2010.11.30.0000.patch", 6921623,		0xA5479111 },

	{ "48eca647/patch/D2010.12.06.0000.patch", 7158904,		0xCAD6BC31 },
	{ "48eca647/patch/D2010.12.13.0000.patch", 263311481,	0xE51EFC06 },
	{ "48eca647/patch/D2010.12.21.0000.patch", 7521358,		0x93EE1510 },

	{ "48eca647/patch/D2011.01.18.0000.patch", 9954265,		0x059E8900 },

	{ "48eca647/patch/D2011.02.01.0000.patch", 11632816,	0x9EE60B39 },
	{ "48eca647/patch/D2011.02.10.0000.patch", 11714096,	0x0ADE7243 },

	{ "48eca647/patch/D2011.03.01.0000.patch", 77464101,	0x7818B5BF },
	{ "48eca647/patch/D2011.03.24.0000.patch", 108923937,	0xF21852AD },
	{ "48eca647/patch/D2011.03.30.0000.patch", 109010880,	0x84CB2682 },

	{ "48eca647/patch/D2011.04.13.0000.patch", 341603850,	0xFF6C3DB0 },
	{ "48eca647/patch/D2011.04.21.0000.patch", 343579198,	0x57F4041C },

	{ "48eca647/patch/D2011.05.19.0000.patch", 344239925,	0xB16FF18C },

	{ "48eca647/patch/D2011.06.10.0000.patch", 344334860,	0xB1CAA88B },

	{ "48eca647/patch/D2011.07.20.0000.patch", 584926805,	0x2EA149A9 },
	{ "48eca647/patch/D2011.07.26.0000.patch", 7649141,		0x5670BA07 },

	{ "48eca647/patch/D2011.08.05.0000.patch", 152064532,	0x0D9E9FD8 },
	{ "48eca647/patch/D2011.08.09.0000.patch", 8573687,		0x9B54551A },
	{ "48eca647/patch/D2011.08.16.0000.patch", 6118907,		0x75231C57 },

	{ "48eca647/patch/D2011.10.04.0000.patch", 677633296,	0x95C15318 },
	{ "48eca647/patch/D2011.10.12.0001.patch", 28941655,	0xB37993E3 },
	{ "48eca647/patch/D2011.10.27.0000.patch", 29179764,	0x977480DC },

	{ "48eca647/patch/D2011.12.14.0000.patch", 374617428,	0xC6FE8FED },
	{ "48eca647/patch/D2011.12.23.0000.patch", 22363713,	0x93137C93 },

	{ "48eca647/patch/D2012.01.18.0000.patch", 48998794,	0x9E55EC7E },
	{ "48eca647/patch/D2012.01.24.0000.patch", 49126606,	0x3008D942 },
	{ "48eca647/patch/D2012.01.31.0000.patch", 49536396,	0x60FDBD0B },

	{ "48eca647/patch/D2012.03.07.0000.patch", 320630782,	0x885AD768 },
	{ "48eca647/patch/D2012.03.09.0000.patch", 8312819,		0xC0040D8C },
	{ "48eca647/patch/D2012.03.22.0000.patch", 22027738,	0xEABC501B },
	{ "48eca647/patch/D2012.03.29.0000.patch", 8322920,		0x63811C35 },

	{ "48eca647/patch/D2012.04.04.0000.patch", 8678570,		0xF6E43EEC },
	{ "48eca647/patch/D2012.04.23.0001.patch", 289511791,	0x6C3C0201 },

	{ "48eca647/patch/D2012.05.08.0000.patch", 27266546,	0xB6AABF18 },
	{ "48eca647/patch/D2012.05.15.0000.patch", 27416023,	0x2D428126 },
	{ "48eca647/patch/D2012.05.22.0000.patch", 27742726,	0x9163549D },

	{ "48eca647/patch/D2012.06.06.0000.patch", 129984024,	0x21DF7238 },
	{ "48eca647/patch/D2012.06.19.0000.patch", 133434217,	0x8280988A },
	{ "48eca647/patch/D2012.06.26.0000.patch", 133581048,	0x4CF33FC8 },

	{ "48eca647/patch/D2012.07.21.0000.patch", 253224781,	0xA8A42A32 },

	{ "48eca647/patch/D2012.08.10.0000.patch", 42851112,	0xD8ED4CE3 },

	{ "48eca647/patch/D2012.09.06.0000.patch", 20566711,	0x4235DF72 },
	{ "48eca647/patch/D2012.09.19.0001.patch", 20874726,	0x8A775526 }
};

CPatcherWindow::CPatcherWindow(const boost::filesystem::path& gamePath, const boost::filesystem::path& downloadPath)
: CDialog(MAKEINTRESOURCE(IDD_PATCHERWINDOW))
, m_gamePath(gamePath)
, m_downloadPath(downloadPath)
{
	SetClassPtr();

	m_downloadStatusLabel = Framework::Win32::CStatic(GetDlgItem(m_hWnd, IDC_DOWNLOADSTATUS_LABEL));
	m_downloadProgressLabel = Framework::Win32::CStatic(GetDlgItem(m_hWnd, IDC_DOWNLOADPROGRESS_LABEL));
	m_downloadProgress = Framework::Win32::CProgress(GetDlgItem(m_hWnd, IDC_DOWNLOAD_PROGRESS));

	m_patchStatusLabel = Framework::Win32::CStatic(GetDlgItem(m_hWnd, IDC_PATCHSTATUS_LABEL));
	m_patchProgressLabel = Framework::Win32::CStatic(GetDlgItem(m_hWnd, IDC_PATCHPROGRESS_LABEL));
	m_patchProgress = Framework::Win32::CProgress(GetDlgItem(m_hWnd, IDC_PATCH_PROGRESS));

	m_cancelButton = Framework::Win32::CButton(GetDlgItem(m_hWnd, IDCANCEL));

	SetTimer(m_hWnd, 0, TIMER_RATE, nullptr);

	m_patchStatusLabel.SetText(_T("Patcher waiting for download to complete..."));
	m_patchProgressLabel.SetText(_T("0%"));

	StepDownloader();
	UpdateDownloaderStatus();
}

CPatcherWindow::~CPatcherWindow()
{

}

long CPatcherWindow::OnTimer(WPARAM timerId)
{
	if((m_downloadComplete || m_patchComplete) && m_cancelPending)
	{
		Destroy();
		return FALSE;
	}
	if(m_downloadComplete)
	{
		m_downloadComplete = false;
		if(m_downloadResult == DOWNLOADER_SERVICE_RESULT_SUCCESS)
		{
			StepDownloader();
		}
		else
		{
			FinishProcess();
		}
		UpdateDownloaderStatus();
	}
	if(m_downloaderService.IsActive() && !m_cancelPending)
	{
		UpdateDownloaderStatus();
	}
	if(m_patchComplete)
	{
		m_patchComplete = false;
		if(m_patchResult == PATCHER_SERVICE_RESULT_SUCCESS)
		{
			StepPatcher();
		}
		else
		{
			FinishProcess();
		}
		UpdatePatcherStatus();
	}
	return FALSE;
}

long CPatcherWindow::OnCommand(unsigned short cmdId, unsigned short cmdType, HWND hwndFrom)
{
	switch(cmdId)
	{
	case IDCANCEL:
		if(m_processComplete)
		{
			Destroy();
		}
		else
		{
			if(MessageBox(m_hWnd, _T("Are you sure that you want to cancel the update process?"), APP_NAME, MB_YESNO | MB_ICONWARNING) == IDYES)
			{
				SetText(_T("Cancelling, please wait..."));
				m_cancelPending = true;
				m_downloaderService.CancelDownload();
			}
		}
		break;
	}
	return FALSE;
}

void CPatcherWindow::StepDownloader()
{
	if(m_downloadIdx == m_downloads.size())
	{
		for(const auto& downloadInfo : m_downloads)
		{
			auto patchPath = m_downloadPath / boost::filesystem::path(downloadInfo.path);
			m_patchPaths.push_back(patchPath);
		}

		std::sort(std::begin(m_patchPaths), std::end(m_patchPaths), 
			[] (const boost::filesystem::path& p1, const boost::filesystem::path& p2)
			{
				return p1.leaf() < p2.leaf();
			}
		);

		StepPatcher();
	}
	else
	{
		const auto& downloadInfo = m_downloads[m_downloadIdx];
		auto downloadDstPath = m_downloadPath / boost::filesystem::path(downloadInfo.path);
		auto downloadUrl = std::string(g_patchUrlBase) + std::string(downloadInfo.path);
		m_downloadSizeHistoryIndex = 0;
		memset(m_downloadSizeHistory, 0, sizeof(m_downloadSizeHistory));

		m_downloaderService.Download(downloadUrl, downloadDstPath, downloadInfo.size, downloadInfo.crc, 
			[&] (const DOWNLOADER_SERVICE_RESULT& result) 
			{
				m_downloadResult = result;
				m_downloadIdx++;
				m_downloadComplete = true;
			}
		);
	}
}

void CPatcherWindow::StepPatcher()
{
	if(m_patchIdx == m_patchPaths.size())
	{
		FinishProcess();
	}
	else
	{
		auto nextPatchPath = m_patchPaths[m_patchIdx];

		m_patcherService.Patch(nextPatchPath, m_gamePath,
			[&](const PATCHER_SERVICE_RESULT& result)
			{
				m_patchResult = result;
				m_patchIdx++;
				m_patchComplete = true;
			}
		);
	}
}

void CPatcherWindow::FinishProcess()
{
	m_cancelButton.SetText(_T("Close"));
	m_processComplete = true;
}

void CPatcherWindow::UpdateDownloaderStatus()
{
	uint64 totalDownloadSize = 0;
	uint64 downloadedSize = 0;
	for(unsigned int i = 0; i < m_downloads.size(); i++)
	{
		const auto& downloadInfo = m_downloads[i];
		if(i < m_downloadIdx)
		{
			downloadedSize += downloadInfo.size;
		}
		totalDownloadSize += downloadInfo.size;
	}

	m_downloadProgress.SetRange(0, static_cast<uint32>(totalDownloadSize / UNIT_KILOBYTE));

	if(m_downloadResult != DOWNLOADER_SERVICE_RESULT_SUCCESS)
	{
		switch(m_downloadResult)
		{
		case DOWNLOADER_SERVICE_RESULT_ERROR_BADCHECKSUM:
			m_downloadStatusLabel.SetText(_T("Download failed. Wrong checksum."));
			break;
		case DOWNLOADER_SERVICE_RESULT_ERROR_BADFILESIZE:
			m_downloadStatusLabel.SetText(_T("Download failed. Wrong file size."));
			break;
		case DOWNLOADER_SERVICE_RESULT_ERROR_NETWORK:
			m_downloadStatusLabel.SetText(_T("Download failed. Network error."));
			break;
		default:
			m_downloadStatusLabel.SetText(_T("Download failed."));
			break;
		}
		m_downloadProgress.SetPosition(static_cast<uint32>(totalDownloadSize / UNIT_KILOBYTE));
		m_downloadProgress.SetState(PBST_ERROR);
	}
	else if(m_downloadIdx == m_downloads.size())
	{
		m_downloadStatusLabel.SetText(_T("Download Completed!"));
		m_downloadProgressLabel.SetText(_T("100%"));
		m_downloadProgress.SetPosition(static_cast<uint32>(totalDownloadSize / UNIT_KILOBYTE));
	}
	else
	{
		uint32 currentDownloadSize = m_downloaderService.GetDownloadedSize();
		downloadedSize += currentDownloadSize;

		m_downloadSizeHistory[m_downloadSizeHistoryIndex++] = currentDownloadSize;
		m_downloadSizeHistoryIndex %= MAX_DOWNLOAD_SIZE_HISTORY;
		unsigned int downloadRateSamples = 0;
		float downloadRate = 0;
		for(unsigned int i = 0; i < MAX_DOWNLOAD_SIZE_HISTORY - 1; i++)
		{
			uint32 currDownloadSizeIndex = (m_downloadSizeHistoryIndex + i + 0) % MAX_DOWNLOAD_SIZE_HISTORY;
			uint32 nextDownloadSizeIndex = (m_downloadSizeHistoryIndex + i + 1) % MAX_DOWNLOAD_SIZE_HISTORY;
			uint32 currDownloadSize = m_downloadSizeHistory[currDownloadSizeIndex];
			uint32 nextDownloadSize = m_downloadSizeHistory[nextDownloadSizeIndex];
			if(nextDownloadSize < currDownloadSize)
			{
				break;
			}
			downloadRate += (nextDownloadSize - currDownloadSize);
			downloadRateSamples++;
		}
		
		if(downloadRateSamples != 0)
		{
			downloadRate /= static_cast<float>(downloadRateSamples);
		}
		downloadRate /= static_cast<float>(UNIT_KILOBYTE);
		downloadRate *= 1000.f / static_cast<float>(TIMER_RATE);
		
		const auto& downloadInfo = m_downloads[m_downloadIdx];
		auto downloadDstPath = m_downloadPath / boost::filesystem::path(downloadInfo.path);

		unsigned int downloadPercent = static_cast<unsigned int>(static_cast<float>(downloadedSize) / static_cast<float>(totalDownloadSize) * 100.f);

		m_downloadProgress.SetPosition(static_cast<uint32>(downloadedSize / UNIT_KILOBYTE));
		m_downloadStatusLabel.SetText(string_format(_T("Downloading '%s'... (%d/%d KB) @ %0.2f KB/s"), 
			downloadDstPath.leaf().native().c_str(), currentDownloadSize / UNIT_KILOBYTE, downloadInfo.size / UNIT_KILOBYTE, downloadRate).c_str());
		m_downloadProgressLabel.SetText(string_format(_T("%d%%"), downloadPercent).c_str());
	}
}

void CPatcherWindow::UpdatePatcherStatus()
{
	m_patchProgress.SetRange(0, m_patchPaths.size());

	if(m_patchResult != PATCHER_SERVICE_RESULT_SUCCESS)
	{
		m_patchStatusLabel.SetText(_T("Patch failed."));
		m_patchProgress.SetPosition(m_patchPaths.size());
		m_patchProgress.SetState(PBST_ERROR);
	}
	else if(m_patchIdx == m_patchPaths.size())
	{
		//Complete
		m_patchStatusLabel.SetText(_T("Complete!"));
		m_patchProgressLabel.SetText(_T("100%"));
		m_patchProgress.SetPosition(m_patchIdx);
	}
	else
	{
		auto nextPatchPath = m_patchPaths[m_patchIdx];
		unsigned int patchPercent = static_cast<unsigned int>(static_cast<float>(m_patchIdx) / static_cast<float>(m_patchPaths.size()) * 100.f);
		m_patchProgress.SetPosition(m_patchIdx);
		m_patchStatusLabel.SetText(string_format(_T("Applying '%s'..."), nextPatchPath.leaf().native().c_str()).c_str());
		m_patchProgressLabel.SetText(string_format(_T("%d%%"), patchPercent).c_str());
	}
}
