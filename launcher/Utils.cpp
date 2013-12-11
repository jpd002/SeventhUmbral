#include <zlib.h>
#include "Utils.h"
#include "StdStreamUtils.h"
#include "Utf8.h"
#include "AppConfig.h"
#include "AppPreferences.h"

#define GAME_INSTALL_REGKEY _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F2C4E6E0-EB78-4824-A212-6DF6AF0E8E82}")

uint32 Utils::ComputeFileCrc32(const boost::filesystem::path& filePath)
{
	auto inputStream = Framework::CreateInputStdStream(filePath.native());
	auto crc = crc32(0, Z_NULL, 0);
	while(1)
	{
		const uint32 bufferSize = 0x4000;
		uint8 buffer[bufferSize];
		auto actualRead = inputStream.Read(buffer, bufferSize);
		if(actualRead == 0)
		{
			break;
		}
		crc = crc32(crc, buffer, static_cast<uInt>(actualRead));
		if(inputStream.IsEOF())
		{
			break;
		}
	}
	return crc;
}

boost::filesystem::path Utils::GetGameLocationPathFromSettings()
{
	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_GAME_LOCATION));
	if(gameLocation.empty())
	{
		throw std::runtime_error("No game location specified.");
	}

	auto gameLocationPath = boost::filesystem::path(gameLocation);
	return gameLocationPath;
}

std::tstring Utils::GetGameLocationFromInstallInfo()
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

std::tstring Utils::BrowseForFolder(HWND parentWnd, const TCHAR* title)
{
	std::tstring folder;
	BROWSEINFO browseInfo = {};
	browseInfo.hwndOwner = parentWnd;
	browseInfo.lpszTitle = title;
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	PIDLIST_ABSOLUTE result = SHBrowseForFolder(&browseInfo);
	if(result != NULL)
	{
		TCHAR selectedPath[MAX_PATH];
		if(SHGetPathFromIDList(result, selectedPath))
		{
			folder = selectedPath;
		}
		CoTaskMemFree(result);
	}
	return folder;
}

std::tstring Utils::BrowseForGameLocation(HWND parentWnd)
{
	return BrowseForFolder(parentWnd, _T("Specify FFXIV folder"));
}

bool Utils::IsValidGameLocationPath(const boost::filesystem::path& gameLocationPath)
{
	auto bootExecutablePath = gameLocationPath / "ffxivboot.exe";
	return boost::filesystem::exists(bootExecutablePath);
}
