#include <WinSock2.h>
#include <Windows.h>
#include <Shlobj.h>
#include <WinInet.h>
#include "LauncherWindow.h"
#include "GameSettingsWindow.h"
#include "Launcher.h"
#include "resource.h"
#include "string_cast.h"
#include "string_format.h"
#include "AppConfig.h"
#include "AppPreferences.h"
#include "AppDef.h"
#include "Utf8.h"
#include "StdStream.h"
#include "Utils.h"

#define PAGE_LOAD_TIMER_ID					0xBEEF

CLauncherWindow::CLauncherWindow()
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_LAUNCHERWINDOW))
, m_pageLoaded(false)
, m_pageLoadTimerId(NULL)
{
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

	{
		auto webBrowserPlaceholder = Framework::Win32::CStatic(GetItem(IDC_WEBBROWSER_PLACEHODLER));

		RECT placeHolderRect = webBrowserPlaceholder.GetWindowRect();
		ScreenToClient(m_hWnd, reinterpret_cast<LPPOINT>(&placeHolderRect) + 0);
		ScreenToClient(m_hWnd, reinterpret_cast<LPPOINT>(&placeHolderRect) + 1);

		m_webBrowser = Framework::Win32::CWebBrowser(m_hWnd, placeHolderRect);
	}

	m_serverAddressComboBox = Framework::Win32::CComboBox(GetItem(IDC_SERVERADDRESS_COMBOBOX));
	m_versionInfoLabel = Framework::Win32::CStatic(GetItem(IDC_VERSIONINFO_LABEL));

	FillServerAddressComboBox();
	LoadServerAddressComboBoxSetting();
	SetVersionInfo();
	LoadLoginPage();
}

CLauncherWindow::~CLauncherWindow()
{

}

long CLauncherWindow::OnCommand(unsigned short cmdId, unsigned short cmdType, HWND hwndFrom)
{
	switch(cmdId)
	{
	case IDCANCEL:
		Destroy();
		break;
	case IDC_GAMESETTINGS_BUTTON:
		ShowGameSettings();
		break;
	}
	if(CWindow::IsCommandSource(&m_serverAddressComboBox, hwndFrom))
	{
		switch(cmdType)
		{
		case CBN_EDITCHANGE:
			if(m_pageLoadTimerId != NULL)
			{
				KillTimer(m_hWnd, m_pageLoadTimerId);
			}
			m_pageLoadTimerId = SetTimer(m_hWnd, PAGE_LOAD_TIMER_ID, 1000, nullptr);
			ResetLoginPage();
			break;
		case CBN_SELCHANGE:
			LoadLoginPage();
			break;
		}
	}
	return FALSE;
}

long CLauncherWindow::OnNotify(WPARAM wparam, NMHDR* hdr)
{
	if(CWindow::IsNotifySource(&m_webBrowser, hdr))
	{
		switch(hdr->code)
		{
		case Framework::Win32::CWebBrowser::NOTIFICATION_BEFORENAVIGATE:
			BeforeNavigate(static_cast<Framework::Win32::CWebBrowser::BEFORENAVIGATE_INFO*>(hdr));
			break;
		}
	}
	return FALSE;
}

long CLauncherWindow::OnTimer(WPARAM wparam)
{
	if(wparam == PAGE_LOAD_TIMER_ID)
	{
		KillTimer(m_hWnd, m_pageLoadTimerId);
		m_pageLoadTimerId = NULL;
		LoadLoginPage();
	}
	return FALSE;
}

long CLauncherWindow::OnCtlColorStatic(HDC dc, HWND wnd)
{
	if(wnd == m_versionInfoLabel)
	{
		HBRUSH result = reinterpret_cast<HBRUSH>(GetSysColorBrush(COLOR_BTNFACE));
		SetTextColor(dc, RGB(0x80, 0x80, 0x80));
		SetBkMode(dc, TRANSPARENT);
		return reinterpret_cast<long>(result);
	}
	else
	{
		return 0;
	}
}

void CLauncherWindow::FillServerAddressComboBox()
{
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

void CLauncherWindow::SetVersionInfo()
{
	auto versionString = string_format(_T("Version: %s"), APP_VERSIONSTR);
	m_versionInfoLabel.SetText(versionString.c_str());
}

void CLauncherWindow::ShowGameSettings()
{
	CGameSettingsWindow gameSettingsWindow(m_hWnd);
	gameSettingsWindow.DoModal();
}

void CLauncherWindow::LaunchGame(const char* sessionId)
{
	std::string savedServerName;
	std::string savedServerAddress;

	auto serverAddress = string_cast<std::string>(m_serverAddressComboBox.GetText());
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

	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_GAME_LOCATION));
	if(gameLocation.empty())
	{
		throw std::runtime_error("No game location specified.");
	}

	auto gameLocationPath = boost::filesystem::path(gameLocation);
	if(!Utils::IsValidGameLocationPath(gameLocationPath))
	{
		throw std::runtime_error("Specified location doesn't contain the game.");
	}

	CLauncher::Launch(gameLocationPath.string().c_str(), serverIpAddress.c_str(), sessionId);

	//Save and we're done
	CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_SERVER_NAME, savedServerName.c_str());
	CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_SERVER_ADDRESS, savedServerAddress.c_str());

	Destroy();
}

void CLauncherWindow::LoadLoginPage()
{
	m_webBrowser.Stop();
	m_pageLoaded = true;

	int serverAddressSelection = m_serverAddressComboBox.GetSelection();
	if(serverAddressSelection == -1)
	{
		auto serverAddressText = m_serverAddressComboBox.GetText();
		if(serverAddressText.empty())
		{
			m_webBrowser.Navigate(_T("about:blank"));
		}
		else
		{
			LoadNullLoginPage();
		}
		return;
	}

	uint32 itemKey = m_serverAddressComboBox.GetItemData(serverAddressSelection);
	if(m_serverAddressComboItemKeys.find(itemKey) == std::end(m_serverAddressComboItemKeys))
	{
		LoadNullLoginPage();
		return;
	}

	auto serverName = m_serverAddressComboItemKeys[itemKey];
	auto serverIterator = m_serverDefs.GetServers().find(serverName);
	if(serverIterator == std::end(m_serverDefs.GetServers()))
	{
		LoadNullLoginPage();
		return;
	}

	const auto& serverDef = serverIterator->second;
	if(serverDef.loginUrl.empty())
	{
		LoadNullLoginPage();
		return;
	}

	m_webBrowser.Navigate(string_cast<std::tstring>(serverDef.loginUrl).c_str());
}

void CLauncherWindow::LoadNullLoginPage()
{
	auto document = m_webBrowser.GetDocument();
	if(!document.IsEmpty())
	{
		auto page = GetNullLoginPage();

		HRESULT result = S_OK;

		BSTR documentText = SysAllocString(page.c_str());

		{
			SAFEARRAYBOUND documentBounds = {};
			documentBounds.cElements = 1;
			documentBounds.lLbound = 0;
			auto documentArray = SafeArrayCreate(VT_VARIANT, 1, &documentBounds);
			{
				result = SafeArrayLock(documentArray);
				assert(SUCCEEDED(result));
				auto& elementVar = reinterpret_cast<VARIANT*>(documentArray->pvData)[0];
				elementVar.vt		= VT_BSTR;
				elementVar.bstrVal	= documentText;
				result = SafeArrayUnlock(documentArray);
				assert(SUCCEEDED(result));
			}
			result = document->write(documentArray);
			assert(SUCCEEDED(result));
			SafeArrayDestroy(documentArray);
		}

		SysFreeString(documentText);

		document->close();
	}
}

void CLauncherWindow::ResetLoginPage()
{
	if(m_pageLoaded)
	{
		m_webBrowser.Stop();
		m_webBrowser.Navigate(_T("about:blank"));
		m_pageLoaded = false;
	}
}

std::tstring CLauncherWindow::GetNullLoginPage()
{
	HRSRC pageResource = FindResource(NULL, MAKEINTRESOURCE(IDR_NULL_LOGIN_PAGE), RT_RCDATA);
	assert(pageResource != NULL);
	HGLOBAL pageResourceHandle = LoadResource(NULL, pageResource);
	DWORD pageResourceSize = SizeofResource(NULL, pageResource);
	assert(pageResourceHandle != NULL);
	const char* pageResourceData = static_cast<const char*>(LockResource(pageResourceHandle));
	std::string pageText = std::string(pageResourceData, pageResourceData + pageResourceSize);
	UnlockResource(pageResourceHandle);
	return string_cast<std::tstring>(pageText);
}

void CLauncherWindow::BeforeNavigate(Framework::Win32::CWebBrowser::BEFORENAVIGATE_INFO* beforeNavigateInfo)
{
	const auto& navigateUrl = beforeNavigateInfo->navigateUrl;
	
	const int partSize = 256;
	TCHAR scheme[partSize];
	TCHAR hostName[partSize];
	TCHAR urlPath[partSize];

	URL_COMPONENTS components;
	memset(&components, 0, sizeof(URL_COMPONENTS));
	components.dwStructSize = sizeof(URL_COMPONENTS);
	components.lpszScheme		= scheme;
	components.dwSchemeLength	= partSize;
	components.lpszHostName		= hostName;
	components.dwHostNameLength	= partSize;
	components.lpszUrlPath		= urlPath;
	components.dwUrlPathLength	= partSize;

	BOOL result = InternetCrackUrl(navigateUrl.c_str(), 0, ICU_ESCAPE, &components);
	if(result == FALSE) return;

	if(_tcscmp(components.lpszScheme, _T("ffxiv"))) return;
	if(_tcscmp(components.lpszHostName, _T("login_success"))) return;

	auto parameters = GetUrlParameters(components.lpszUrlPath);

	auto sessionIdIterator = parameters.find(_T("sessionId"));
	if(sessionIdIterator != std::end(parameters))
	{
		auto sessionId = string_cast<std::string>(sessionIdIterator->second);
		try
		{
			LaunchGame(sessionId.c_str());
		}
		catch(const std::exception& except)
		{
			auto message = _T("Failed to launch game: ") + string_cast<std::tstring>(except.what());
			MessageBox(m_hWnd, message.c_str(), APP_NAME, MB_ICONERROR);
			LoadLoginPage();
			beforeNavigateInfo->cancel = true;
			return;
		}
	}
}

CLauncherWindow::StringKeyValueMap CLauncherWindow::GetUrlParameters(const TCHAR* url)
{
	std::map<std::tstring, std::tstring> parameters;
	auto paramStart = _tcschr(url, _T('?'));
	if(paramStart != nullptr)
	{
		while(1)
		{
			paramStart++;
			auto nextParam = _tcschr(paramStart, _T('&'));
			auto paramSeparatorPos = _tcschr(paramStart, _T('='));
			if(paramSeparatorPos != nullptr)
			{
				auto paramEnd = nextParam ? nextParam : (paramStart + _tcslen(paramStart));
				std::tstring paramName(paramStart, paramSeparatorPos);
				std::tstring paramValue(paramSeparatorPos + 1, paramEnd);
				parameters.insert(std::make_pair(paramName, paramValue));
			}
			if(nextParam == nullptr) break;
			paramStart = nextParam;
		}
	}
	return parameters;
}
