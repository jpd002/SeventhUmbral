#pragma once

#include "win32/Dialog.h"
#include "win32/ComboBox.h"
#include "win32/Static.h"
#include "win32/WebBrowser.h"
#include <boost/filesystem.hpp>
#include "ServerDefs.h"

class CLauncherWindow : public Framework::Win32::CDialog
{
public:
									CLauncherWindow();
	virtual							~CLauncherWindow();

protected:
	long							OnCommand(unsigned short, unsigned short, HWND) override;
	long							OnNotify(WPARAM, NMHDR*) override;
	long							OnTimer(WPARAM) override;
	long							OnCtlColorStatic(HDC, HWND) override;

private:
	typedef std::map<unsigned int, std::string> ServerAddressComboItemKeyMap;
	typedef std::map<std::tstring, std::tstring> StringKeyValueMap;

	static std::tstring				GetGameLocationFromInstallInfo();
	static bool						IsValidGameLocationPath(const boost::filesystem::path&);
	static StringKeyValueMap		GetUrlParameters(const TCHAR*);

	void							LoadLoginPage();
	void							ResetLoginPage();
	void							LoadNullLoginPage();
	static std::tstring				GetNullLoginPage();

	void							BeforeNavigate(Framework::Win32::CWebBrowser::BEFORENAVIGATE_INFO*);
	void							FillServerAddressComboBox();
	void							LoadServerAddressComboBoxSetting();
	void							SetVersionInfo();

	void							ShowGameSettings();
	void							LaunchGame(const char*);

	bool							m_pageLoaded;
	UINT_PTR						m_pageLoadTimerId;
	Framework::Win32::CComboBox		m_serverAddressComboBox;
	Framework::Win32::CStatic		m_versionInfoLabel;
	Framework::Win32::CWebBrowser	m_webBrowser;
	CServerDefs						m_serverDefs;
	ServerAddressComboItemKeyMap	m_serverAddressComboItemKeys;
};
