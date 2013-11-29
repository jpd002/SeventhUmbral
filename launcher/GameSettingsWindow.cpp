#include "GameSettingsWindow.h"
#include "resource.h"
#include "AppConfig.h"
#include "AppPreferences.h"
#include "string_cast.h"
#include "Utf8.h"
#include "Utils.h"

CGameSettingsWindow::CGameSettingsWindow(HWND parentWnd)
: CDialog(MAKEINTRESOURCE(IDD_GAMESETTINGSWINDOW), parentWnd)
{
	SetClassPtr();

	m_gameLocationEdit = Framework::Win32::CEdit(GetDlgItem(m_hWnd, IDC_GAMELOCATION_EDIT));
	auto gameLocation = Framework::Utf8::ConvertFrom(CAppConfig::GetInstance().GetPreferenceString(PREF_LAUNCHER_GAME_LOCATION));
	m_gameLocationEdit.SetText(gameLocation.c_str());
}

CGameSettingsWindow::~CGameSettingsWindow()
{

}

long CGameSettingsWindow::OnCommand(unsigned short cmdId, unsigned short cmdType, HWND hwndFrom)
{
	switch(cmdId)
	{
	case IDOK:
		Save();
		Destroy();
		break;
	case IDCANCEL:
		Destroy();
		break;
	case IDC_BROWSE_GAMELOCATION:
		{
			auto newLocation = Utils::BrowseForGameLocation(m_hWnd);
			if(!newLocation.empty())
			{
				m_gameLocationEdit.SetText(newLocation.c_str());
			}
		}
		break;
	}
	return FALSE;
}

void CGameSettingsWindow::Save()
{
	auto gameLocation = m_gameLocationEdit.GetText();
	CAppConfig::GetInstance().SetPreferenceString(PREF_LAUNCHER_GAME_LOCATION, Framework::Utf8::ConvertTo(gameLocation).c_str());
}
