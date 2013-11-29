#include "Utf8.h"
#include "LauncherWindow.h"
#include "AppDef.h"
#include "AppPreferences.h"
#include "AppConfig.h"
#include "Utils.h"
#include "string_format.h"
#include "PatchProcess.h"

void PrepareGameLocation()
{
	CAppConfig::GetInstance().RegisterPreferenceString(PREF_LAUNCHER_GAME_LOCATION, "");

	bool isFreshGameLocation = false;
	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_GAME_LOCATION));
	if(gameLocation.empty())
	{
		gameLocation = Utils::GetGameLocationFromInstallInfo();
		CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_GAME_LOCATION, Framework::Utf8::ConvertTo(gameLocation).c_str());
		isFreshGameLocation = true;
	}

	if(isFreshGameLocation)
	{
		int pressedButton = 0;
		std::tstring message = string_format(
			_T("The launcher has detected a Final Fantasy XIV installation at this location:\r\n\r\n")
			_T("%s\r\n\r\n")
			_T("Is this location correct?\r\n\r\n")
			_T("(Selecting \"No\" will ask you for a different game location. You will also be able to change that setting later in the launcher if you want.)"),
			gameLocation.c_str());
		TASKDIALOGCONFIG tdc;
		memset(&tdc, 0, sizeof(tdc));
		tdc.cbSize = sizeof(TASKDIALOGCONFIG);
		tdc.pszWindowTitle		= APP_NAME;
		tdc.dwCommonButtons		= TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
		tdc.pszMainInstruction	= _T("Confirm Game Location");
		tdc.pszContent			= message.c_str();
		tdc.pszMainIcon			= TD_INFORMATION_ICON;
		HRESULT result = TaskDialogIndirect(&tdc, &pressedButton, NULL, NULL);
		if(SUCCEEDED(result))
		{
			if(pressedButton == IDNO)
			{
				auto newLocation = Utils::BrowseForGameLocation(NULL);
				if(!newLocation.empty())
				{
					CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_GAME_LOCATION, Framework::Utf8::ConvertTo(newLocation).c_str());
				}
			}
		}
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
