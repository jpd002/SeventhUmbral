#include <zlib.h>
#include "Utils.h"
#include "StdStreamUtils.h"

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

std::tstring Utils::GetGameLocationFromInstallInfo()
{
	LSTATUS result = ERROR_SUCCESS;
	TCHAR installPath[256];
	TCHAR displayName[256];
	{
		DWORD keyType = REG_SZ;
		DWORD dataSize = _countof(installPath);
		result = RegGetValue(HKEY_LOCAL_MACHINE, GAME_INSTALL_REGKEY, _T("InstallLocation"), RRF_RT_REG_SZ, &keyType, installPath, &dataSize);
		if(result != ERROR_SUCCESS) return std::tstring();
	}
	{
		DWORD keyType = REG_SZ;
		DWORD dataSize = _countof(displayName);
		result = RegGetValue(HKEY_LOCAL_MACHINE, GAME_INSTALL_REGKEY, _T("DisplayName"), RRF_RT_REG_SZ, &keyType, displayName, &dataSize);
		if(result != ERROR_SUCCESS) return std::tstring();
	}
	return std::tstring(installPath) + _T("\\") + std::tstring(displayName);
}

std::tstring Utils::BrowseForFolder(HWND parentWnd, const TCHAR* title)
{
	std::tstring folder;
	BROWSEINFO browseInfo = {};
	browseInfo.hwndOwner = parentWnd;
	browseInfo.lpszTitle = title;
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
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

bool Utils::IsProcessElevated()
{
	HANDLE tokenHandle = NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
	{
		return false;
	}

	TOKEN_ELEVATION tokenElevation;
	DWORD tokenSize = 0;
	if(!GetTokenInformation(tokenHandle, TokenElevation, &tokenElevation, sizeof(tokenElevation), &tokenSize))
	{
		CloseHandle(tokenHandle);
		return false;
	}

	bool result = (tokenElevation.TokenIsElevated == TRUE);
	CloseHandle(tokenHandle);

	return result;
}
