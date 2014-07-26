#include "SettingsPage.h"
#include "resource.h"
#include "Utf8.h"
#include "AppConfig.h"
#include "AppPreferences.h"
#include "string_cast.h"

CSettingsPage::CSettingsPage(HWND parentWnd)
: CDialog(MAKEINTRESOURCE(IDD_SETTINGSPAGE), parentWnd)
{
	SetClassPtr();
	m_gameLocationEdit = Framework::Win32::CEdit(GetItem(IDC_SETTINGSPAGE_GAMELOCATION_EDIT));

	UpdateGameLocation();
}

CSettingsPage::~CSettingsPage()
{

}

std::string CSettingsPage::GetName() const
{
	return "General Settings";
}

long CSettingsPage::OnCommand(unsigned short cmd, unsigned short, HWND)
{
	switch(cmd)
	{
	case IDC_SETTINGSPAGE_BROWSE_GAMELOCATION:
		BrowseGameLocation();
		break;
	}
	return TRUE;
}

void CSettingsPage::UpdateGameLocation()
{
	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));
	m_gameLocationEdit.SetText(string_cast<std::tstring>(gameLocation).c_str());
}

void CSettingsPage::BrowseGameLocation()
{
	auto gameLocation = BrowseForFolder(_T("Specify FFXIV folder"));
	if(!gameLocation.empty())
	{
		CAppConfig::GetInstance().SetPreferenceString(PREF_WORKSHOP_GAME_LOCATION, 
			Framework::Utf8::ConvertTo(string_cast<std::wstring>(gameLocation)).c_str());
		UpdateGameLocation();
	}
}

std::tstring CSettingsPage::BrowseForFolder(const TCHAR* title)
{
	std::tstring folder;
	BROWSEINFO browseInfo = {};
	browseInfo.hwndOwner = m_hWnd;
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

