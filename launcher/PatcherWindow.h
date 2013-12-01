#pragma once

#include "win32/Dialog.h"
#include "win32/Static.h"
#include "win32/Progress.h"
#include "win32/Button.h"
#include "PatcherService.h"
#include "DownloaderService.h"

class CPatcherWindow : public Framework::Win32::CDialog
{
public:
									CPatcherWindow(const boost::filesystem::path&, const boost::filesystem::path&);
	virtual							~CPatcherWindow();

protected:
	long							OnTimer(WPARAM) override;
	long							OnCommand(unsigned short, unsigned short, HWND) override;

private:
	struct DOWNLOAD_INFO
	{
		const char*		path;
		uint32			size;
		uint32			crc;
	};
	typedef std::vector<DOWNLOAD_INFO> DownloadInfoArray;

	typedef std::vector<boost::filesystem::path> PatchPathArray;

	void							StepDownloader();
	void							StepPatcher();
	void							FinishProcess();

	void							UpdateDownloaderStatus();
	void							UpdatePatcherStatus();

	void							WriteVersionFiles();

	Framework::Win32::CStatic		m_downloadStatusLabel;
	Framework::Win32::CStatic		m_downloadProgressLabel;
	Framework::Win32::CProgress		m_downloadProgress;

	Framework::Win32::CStatic		m_patchStatusLabel;
	Framework::Win32::CStatic		m_patchProgressLabel;
	Framework::Win32::CProgress		m_patchProgress;

	Framework::Win32::CButton		m_cancelButton;

	enum
	{
		MAX_DOWNLOAD_SIZE_HISTORY = 0x20
	};

	bool							m_cancelPending = false;

	boost::filesystem::path			m_downloadPath;
	boost::filesystem::path			m_gamePath;

	CDownloaderService				m_downloaderService;
	static const DownloadInfoArray	m_downloads;
	unsigned int					m_downloadIdx = 0;
	unsigned int					m_downloadSizeHistoryIndex = 0;
	uint32							m_downloadSizeHistory[MAX_DOWNLOAD_SIZE_HISTORY];
	DOWNLOADER_SERVICE_RESULT		m_downloadResult = DOWNLOADER_SERVICE_RESULT_SUCCESS;
	std::atomic<bool>				m_downloadComplete = false;

	CPatcherService					m_patcherService;
	PatchPathArray					m_patchPaths;
	unsigned int					m_patchIdx = 0;
	PATCHER_SERVICE_RESULT			m_patchResult = PATCHER_SERVICE_RESULT_SUCCESS;
	std::atomic<bool>				m_patchComplete = false;

	bool							m_processComplete = false;
};
