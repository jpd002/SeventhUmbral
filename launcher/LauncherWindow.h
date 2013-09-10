#pragma once

#include "win32/Dialog.h"
#include "win32/Edit.h"
#include "win32/ComboBox.h"
#include <boost/filesystem.hpp>
#include "ServerDefs.h"

class CLauncherWindow : public Framework::Win32::CDialog
{
public:
								CLauncherWindow();
	virtual						~CLauncherWindow();

protected:
	long						OnCommand(unsigned short, unsigned short, HWND) override;
	
private:
	typedef std::map<unsigned int, std::string> ServerAddressComboItemKeyMap;

	static std::tstring				GetGameLocationFromInstallInfo();
	static bool						IsValidGameLocationPath(const boost::filesystem::path&);
	
	void							FillServerAddressComboBox();
	void							LoadServerAddressComboBoxSetting();

	void							BrowseGameLocation();
	void							LaunchGame();

	Framework::Win32::CEdit			m_gameLocationEdit;
	Framework::Win32::CComboBox		m_serverAddressComboBox;
	CServerDefs						m_serverDefs;
	ServerAddressComboItemKeyMap	m_serverAddressComboItemKeys;
};
