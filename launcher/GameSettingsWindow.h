#pragma once

#include "win32/Dialog.h"
#include "win32/Edit.h"

class CGameSettingsWindow : public Framework::Win32::CDialog
{
public:
									CGameSettingsWindow(HWND);
	virtual							~CGameSettingsWindow();

protected:
	long							OnCommand(unsigned short, unsigned short, HWND) override;

private:
	void							BrowseGameLocation();
	void							Save();

	Framework::Win32::CEdit			m_gameLocationEdit;

};
