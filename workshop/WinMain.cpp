#include <Windows.h>
#include "MainWindow.h"
#include "../dataobjects/FileManager.h"
#include "AppConfig.h"
#include "AppPreferences.h"
#include "Utf8.h"

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR commandLine, int)
{
	CAppConfig::GetInstance().PrepareGameLocation();
	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));
	CFileManager::SetGamePath(gameLocation);

	CoInitialize(NULL);
	{
		CMainWindow mainWindow;
		mainWindow.DoMessageLoop();
	}
	CoUninitialize();
	return 0;
}
