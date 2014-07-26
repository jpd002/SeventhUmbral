#include "GameInstallInfo.h"

#define GAME_INSTALL_REGKEY _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F2C4E6E0-EB78-4824-A212-6DF6AF0E8E82}")

std::tstring CGameInstallInfo::GetInstallPath()
{
	HKEY gameInstallKey = NULL;
	LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, GAME_INSTALL_REGKEY, 0, KEY_QUERY_VALUE, &gameInstallKey);
	if(result != ERROR_SUCCESS)
	{
		return std::tstring();
	}

	TCHAR installPath[MAX_PATH];
	TCHAR displayName[MAX_PATH];
	DWORD installPathLength = sizeof(installPath);
	DWORD displayNameLength = sizeof(displayName);

	LSTATUS installPathResult = RegQueryValueEx(gameInstallKey, _T("InstallLocation"), 0, NULL, reinterpret_cast<LPBYTE>(installPath), &installPathLength);
	LSTATUS displayNameResult = RegQueryValueEx(gameInstallKey, _T("DisplayName"), 0, NULL, reinterpret_cast<LPBYTE>(displayName), &displayNameLength);

	RegCloseKey(gameInstallKey);

	if((installPathResult == ERROR_SUCCESS) && (displayNameResult == ERROR_SUCCESS))
	{
		return std::tstring(installPath) + _T("\\") + std::tstring(displayName);
	}
	else
	{
		return std::tstring();
	}
}
