#include <WinSock2.h>
#include <Windows.h>
#include <Shlobj.h>
#include "LauncherWindow.h"
#include "Launcher.h"
#include "resource.h"
#include "string_cast.h"
#include "string_format.h"
#include "AppConfig.h"
#include "Utf8.h"
#include "StdStream.h"

#define APP_TITLE _T("Seventh Umbral FFXIV Launcher")

#define PREF_LAUNCHER_GAME_LOCATION		"launcher.game.location"
#define PREF_LAUNCHER_SERVER_NAME		"launcher.server.name"
#define PREF_LAUNCHER_SERVER_ADDRESS	"launcher.server.address"

#define GAME_INSTALL_REGKEY _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F2C4E6E0-EB78-4824-A212-6DF6AF0E8E82}")

CLauncherWindow::CLauncherWindow()
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_LAUNCHERWINDOW))
{
	CAppConfig::GetInstance().RegisterPreferenceString(PREF_LAUNCHER_GAME_LOCATION, Framework::Utf8::ConvertTo(GetGameLocationFromInstallInfo()).c_str());
	CAppConfig::GetInstance().RegisterPreferenceString(PREF_LAUNCHER_SERVER_NAME, "");
	CAppConfig::GetInstance().RegisterPreferenceString(PREF_LAUNCHER_SERVER_ADDRESS, "");

	SetClassPtr();

	{
		int smallIconSizeX = GetSystemMetrics(SM_CXSMICON);
		int smallIconSizeY = GetSystemMetrics(SM_CYSMICON);
		int bigIconSizeX = GetSystemMetrics(SM_CXICON);
		int bigIconSizeY = GetSystemMetrics(SM_CYICON);

		HICON smallIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, smallIconSizeX, smallIconSizeY, 0));
		HICON bigIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, bigIconSizeX, bigIconSizeY, 0));

		SetIcon(ICON_SMALL, smallIcon);
		SetIcon(ICON_BIG, bigIcon);
	}

	try
	{
		m_serverDefs.Read(Framework::CStdStream("servers.xml", "rb"));
	}
	catch(...)
	{

	}

	m_gameLocationEdit = Framework::Win32::CEdit(GetItem(IDC_GAMELOCATION_EDIT));
	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_GAME_LOCATION));
	m_gameLocationEdit.SetText(gameLocation.c_str());

	FillServerAddressComboBox();
	LoadServerAddressComboBoxSetting();
}

CLauncherWindow::~CLauncherWindow()
{

}

long CLauncherWindow::OnCommand(unsigned short cmdId, unsigned short, HWND)
{
	switch(cmdId)
	{
	case IDOK:
		LaunchGame();
		break;
	case IDCANCEL:
		DestroyWindow(m_hWnd);
		break;
	case IDC_BROWSE_GAMELOCATION:
		BrowseGameLocation();
		break;
	}
	return FALSE;
}

void CLauncherWindow::FillServerAddressComboBox()
{
	m_serverAddressComboBox = Framework::Win32::CComboBox(GetItem(IDC_SERVERADDRESS_COMBOBOX));
	m_serverAddressComboItemKeys.clear();
	uint32 currentKey = 1;
	for(const auto& server : m_serverDefs.GetServers())
	{
		const auto& serverName = server.first;
		std::string itemDescription = string_format("%s (%s)", server.second.address.c_str(), serverName.c_str());
		unsigned int itemIndex = m_serverAddressComboBox.AddString(string_cast<std::tstring>(itemDescription).c_str());
		m_serverAddressComboBox.SetItemData(itemIndex, currentKey);
		assert(m_serverAddressComboItemKeys.find(currentKey) == m_serverAddressComboItemKeys.end());
		m_serverAddressComboItemKeys[currentKey] = serverName;
		currentKey++;
	}
}

void CLauncherWindow::LoadServerAddressComboBoxSetting()
{
	//Try with the server name setting first
	{
		const auto& selectedServerName = std::string(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_SERVER_NAME));
		if(!selectedServerName.empty())
		{
			const auto searchPredicate =
				[&] (const ServerAddressComboItemKeyMap::value_type& item)
				{
					return item.second == selectedServerName;
				};

			const auto& comboItemKey = std::find_if(m_serverAddressComboItemKeys.begin(), m_serverAddressComboItemKeys.end(), searchPredicate);
			if(comboItemKey != std::end(m_serverAddressComboItemKeys))
			{
				int itemIndex = m_serverAddressComboBox.FindItemData(comboItemKey->first);
				if(itemIndex != -1)
				{
					m_serverAddressComboBox.SetSelection(itemIndex);
				}
				return;
			}
		}
	}

	//Otherwise, just use the server address setting
	const auto& selectedServerAddress = std::string(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_SERVER_ADDRESS));
	m_serverAddressComboBox.SetText(string_cast<std::tstring>(selectedServerAddress).c_str());
}

void CLauncherWindow::LaunchGame()
{
	try
	{
		std::string savedServerName;
		std::string savedServerAddress;

		std::string serverAddress = string_cast<std::string>(m_serverAddressComboBox.GetText());
		int serverAddressSelection = m_serverAddressComboBox.GetSelection();
		if(serverAddressSelection != -1)
		{
			uint32 itemKey = m_serverAddressComboBox.GetItemData(serverAddressSelection);
			if(m_serverAddressComboItemKeys.find(itemKey) != std::end(m_serverAddressComboItemKeys))
			{
				auto serverName = m_serverAddressComboItemKeys[itemKey];
				auto serverIterator = m_serverDefs.GetServers().find(serverName);
				if(serverIterator != std::end(m_serverDefs.GetServers()))
				{
					serverAddress = serverIterator->second.address;
					savedServerName = serverName;
				}
				else
				{
					serverAddress = serverName;
				}
			}
		}

		savedServerAddress = serverAddress;
		if(serverAddress.empty())
		{
			throw std::runtime_error("No server address specified.");
		}

		std::string serverIpAddress = serverAddress;
		hostent* hostAddress = gethostbyname(serverAddress.c_str());
		if(hostAddress)
		{
			char** addrList = hostAddress->h_addr_list;
			if(*addrList != NULL)
			{
				char* addr = (*addrList);
				serverIpAddress = string_format("%u.%u.%u.%u", 
					static_cast<uint8>(addr[0]), 
					static_cast<uint8>(addr[1]), 
					static_cast<uint8>(addr[2]),
					static_cast<uint8>(addr[3])
					);
			}
		}

		auto gameLocation = m_gameLocationEdit.GetText();
		if(gameLocation.empty())
		{
			throw std::runtime_error("No game location specified.");
		}

		auto gameLocationPath = boost::filesystem::path(gameLocation);
		if(!IsValidGameLocationPath(gameLocationPath))
		{
			throw std::runtime_error("Specified location doesn't contain the game.");
		}

		CLauncher::Launch(gameLocationPath.string().c_str(), serverIpAddress.c_str());

		//Save and we're done
		CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_SERVER_NAME, savedServerName.c_str());
		CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_SERVER_ADDRESS, savedServerAddress.c_str());

		DestroyWindow(m_hWnd);
	}
	catch(const std::exception& except)
	{
		auto message = _T("Failed to launch game: ") + string_cast<std::tstring>(except.what());
		MessageBox(m_hWnd, message.c_str(), APP_TITLE, MB_ICONERROR);
		return;
	}
}

void CLauncherWindow::BrowseGameLocation()
{
	BROWSEINFO browseInfo = {};
	browseInfo.hwndOwner = m_hWnd;
	browseInfo.lpszTitle = _T("Specify FFXIV folder");
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	PIDLIST_ABSOLUTE result = SHBrowseForFolder(&browseInfo);
	if(result != NULL)
	{
		TCHAR selectedPath[MAX_PATH];
		if(SHGetPathFromIDList(result, selectedPath))
		{
			CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_GAME_LOCATION, Framework::Utf8::ConvertTo(selectedPath).c_str());
			m_gameLocationEdit.SetText(selectedPath);
		}
		CoTaskMemFree(result);
	}
}

std::tstring CLauncherWindow::GetGameLocationFromInstallInfo()
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

bool CLauncherWindow::IsValidGameLocationPath(const boost::filesystem::path& gameLocationPath)
{
	auto bootExecutablePath = gameLocationPath / "ffxivboot.exe";
	return boost::filesystem::exists(bootExecutablePath);
}
