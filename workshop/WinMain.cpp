#include <Windows.h>
#include "MainWindow.h"
#include "../launcher/Utils.h"
#include "../dataobjects/FileManager.h"
#include "AppConfig.h"
#include "AppPreferences.h"
#include "Utf8.h"

void PrepareGameLocation()
{
	CAppConfig::GetInstance().RegisterPreferenceString(PREF_WORKSHOP_GAME_LOCATION, "");

	auto currentGameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));

	//Already setup, don't go any further
	if(!currentGameLocation.empty()) return;

	auto detectedGameLocation = Utils::GetGameLocationFromInstallInfo();
	CAppConfig::GetInstance().SetPreferenceString(PREF_WORKSHOP_GAME_LOCATION, Framework::Utf8::ConvertTo(detectedGameLocation).c_str());

	CAppConfig::GetInstance().Save();
}

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR commandLine, int)
{
	PrepareGameLocation();

	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));
	CFileManager::SetGamePath(gameLocation);

	CoInitialize(NULL);
	{
		CMainWindow mainWindow;
		Framework::Win32::CWindow::DlgMsgLoop(mainWindow);
	}
	CoUninitialize();
	return 0;
}
