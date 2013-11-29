#include "PatchProcess.h"
#include "PathUtils.h"
#include "PatcherWindow.h"
#include "string_format.h"
#include "AppDef.h"
#include "AppConfig.h"
#include "AppPreferences.h"
#include "Utils.h"
#include "Utf8.h"

void CPatchProcess::PatchGame()
{
	auto patchDownloadLocationPath = Framework::PathUtils::GetPersonalDataPath() / "ffxiv_patches";

	{
		const int diffLocationButtonId = 0xBEEF;

		TASKDIALOG_BUTTON btn;
		btn.nButtonID = diffLocationButtonId;
		btn.pszButtonText = _T("Yes with a different location");

		int pressedButton = 0;
		std::tstring message = string_format(
			_T("The launcher needs to download 5.89GB of data to update the game. The files will be stored at this location:\r\n\r\n")
			_T("%s\r\n\r\n")
			_T("Do you want to continue?"),
			patchDownloadLocationPath.c_str());
		TASKDIALOGCONFIG tdc;
		memset(&tdc, 0, sizeof(tdc));
		tdc.cbSize = sizeof(TASKDIALOGCONFIG);
		tdc.pszWindowTitle		= APP_NAME;
		tdc.dwCommonButtons		= TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
		tdc.nDefaultButton		= IDYES;
		tdc.cButtons			= 1;
		tdc.pButtons			= &btn;
		tdc.pszMainInstruction	= _T("Patch Download Location");
		tdc.pszContent			= message.c_str();
		tdc.pszMainIcon			= TD_INFORMATION_ICON;
		HRESULT result = TaskDialogIndirect(&tdc, &pressedButton, NULL, NULL);
		if(SUCCEEDED(result))
		{
			if(pressedButton == IDNO)
			{
				return;
			}
			if(pressedButton == diffLocationButtonId)
			{
				auto newLocation = Utils::BrowseForFolder(NULL, _T("Specify patch download location."));
				if(newLocation.empty())
				{
					return;
				}
				patchDownloadLocationPath = newLocation;
			}
		}
	}

	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_GAME_LOCATION));
	auto gameLocationPath = boost::filesystem::path(gameLocation);

	CPatcherWindow patcherWindow(gameLocationPath, patchDownloadLocationPath);
	patcherWindow.DoModal();
}

bool CPatchProcess::IsGameUpToDate()
{
	static const uint32 bootExeCrcRef = 0xB63645E4;

	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_GAME_LOCATION));
	if(gameLocation.empty())
	{
		throw std::runtime_error("No game location specified.");
	}

	auto gameLocationPath = boost::filesystem::path(gameLocation);
	auto bootExePath = gameLocationPath / "ffxivboot.exe";

	auto bootExeCrc = Utils::ComputeFileCrc32(bootExePath);

	return (bootExeCrcRef == bootExeCrc);
}

HRESULT CALLBACK OutdatedGameTaskDialogCallback(HWND hwnd, UINT msg, WPARAM, LPARAM, LONG_PTR)
{
	switch(msg)
	{
	case TDN_CREATED:
		SendMessage(hwnd, TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE, IDYES, 1);
		break;
	}
	return S_OK;
}

void CPatchProcess::CheckGameVersionAndStartUpdate()
{
	try
	{
		auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_GAME_LOCATION));
		auto gameLocationPath = boost::filesystem::path(gameLocation);

		if(!Utils::IsValidGameLocationPath(gameLocationPath))
		{
			return;
		}

		if(!IsGameUpToDate())
		{
			int pressedButton = 0;
			TASKDIALOGCONFIG tdc;
			memset(&tdc, 0, sizeof(tdc));
			tdc.cbSize = sizeof(TASKDIALOGCONFIG);
			tdc.pszWindowTitle		= APP_NAME;
			tdc.dwCommonButtons		= TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
			tdc.pszMainInstruction	= _T("Outdated Game Installation");
			tdc.pszContent			= _T("The launcher has detected that your game is not up to date. Would you like to update it to v1.23b?");
			tdc.pszMainIcon			= TD_WARNING_ICON;
			tdc.pfCallback			= &OutdatedGameTaskDialogCallback;
			HRESULT result = TaskDialogIndirect(&tdc, &pressedButton, NULL, NULL);
			if(SUCCEEDED(result))
			{
				if(pressedButton == IDYES)
				{
					CAppConfig::GetInstance().Save();

					TCHAR moduleName[MAX_PATH];
					GetModuleFileName(GetModuleHandle(NULL), moduleName, MAX_PATH);
					SHELLEXECUTEINFO sei;
					memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
					sei.cbSize = sizeof(SHELLEXECUTEINFO);
					sei.lpVerb = _T("runas");
					sei.nShow = SW_SHOWNORMAL;
					sei.lpParameters = PARAMETER_UPDATE_GAME;
					sei.lpFile = moduleName;
					ShellExecuteEx(&sei);

					ExitProcess(0);
				}
			}
		}
	}
	catch(...)
	{
		//Failed to update the game somehow
	}
}
