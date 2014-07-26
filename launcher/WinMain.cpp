#include <tdemu.h>
#include "Utf8.h"
#include "LauncherWindow.h"
#include "AppDef.h"
#include "AppPreferences.h"
#include "AppConfig.h"
#include "Utils.h"
#include "string_format.h"
#include "PatchProcess.h"
#include "../common/GameInstallInfo.h"

void PrepareGameLocation()
{
	CAppConfig::GetInstance().RegisterPreferenceString(PREF_LAUNCHER_GAME_LOCATION, "");

	auto currentGameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_GAME_LOCATION));

	//Already setup, don't go any further
	if(!currentGameLocation.empty()) return;

	auto detectedGameLocation = CGameInstallInfo::GetInstallPath();
	if(!detectedGameLocation.empty())
	{
		int pressedButton = 0;
		std::tstring message = string_format(
			_T("The launcher has detected a Final Fantasy XIV installation at this location:\r\n\r\n")
			_T("%s\r\n\r\n")
			_T("Do you want the launcher to use that game location?\r\n\r\n")
			_T("(Selecting \"No\" will ask you for a different game location. You will also be able to change that setting later in the launcher.)"),
			detectedGameLocation.c_str());
		TASKDIALOGCONFIG tdc;
		memset(&tdc, 0, sizeof(tdc));
		tdc.cbSize = sizeof(TASKDIALOGCONFIG);
		tdc.pszWindowTitle		= APP_NAME;
		tdc.dwCommonButtons		= TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
		tdc.pszMainInstruction	= _T("Confirm Game Location");
		tdc.pszContent			= message.c_str();
		tdc.pszMainIcon			= TD_INFORMATION_ICON;
		HRESULT result = TaskDialogIndirectEmulate(&tdc, &pressedButton, NULL, NULL);
		if(SUCCEEDED(result))
		{
			if(pressedButton == IDYES)
			{
				CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_GAME_LOCATION, Framework::Utf8::ConvertTo(detectedGameLocation).c_str());
				return;
			}
		}
	}
	else
	{
		int pressedButton = 0;
		const TCHAR* message =
			_T("The launcher couldn't detect a Final Fantasy XIV installation. Do you want to select a game location now?\r\n\r\n")
			_T("(You will also be able to change that setting later in the launcher.)");
		TASKDIALOGCONFIG tdc;
		memset(&tdc, 0, sizeof(tdc));
		tdc.cbSize = sizeof(TASKDIALOGCONFIG);
		tdc.pszWindowTitle		= APP_NAME;
		tdc.dwCommonButtons		= TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
		tdc.pszMainInstruction	= _T("Select Game Location");
		tdc.pszContent			= message;
		tdc.pszMainIcon			= TD_INFORMATION_ICON;
		HRESULT result = TaskDialogIndirectEmulate(&tdc, &pressedButton, NULL, NULL);
		if(SUCCEEDED(result))
		{
			if(pressedButton == IDNO)
			{
				return;
			}
		}
	}

	//If we get here, then, the user wants to select a new game location
	auto newLocation = Utils::BrowseForGameLocation(NULL);
	if(!newLocation.empty())
	{
		CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_GAME_LOCATION, Framework::Utf8::ConvertTo(newLocation).c_str());
	}
}

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR commandLine, int)
{
	CoInitialize(NULL);
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox(NULL, _T("Failed to initialize WinSock."), NULL, MB_ICONERROR);
		return -1;
	}

	PrepareGameLocation();
	
	if(!_tcscmp(commandLine, PARAMETER_UPDATE_GAME))
	{
		CPatchProcess::PatchGame();
	}
	else
	{
		CPatchProcess::CheckGameVersionAndStartUpdate();
	}

	{
		CLauncherWindow launcherWindow;
		launcherWindow.DoModal();
	}

	WSACleanup();
	return 0;
}
