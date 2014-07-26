#pragma once

#include "win32/Dialog.h"
#include "win32/Edit.h"
#include "Document.h"

class CSettingsPage : public Framework::Win32::CDialog, public IDocument
{
public:
								CSettingsPage(HWND);
	virtual						~CSettingsPage();

	virtual std::string			GetName() const override;

protected:
	long						OnCommand(unsigned short, unsigned short, HWND) override;

private:
	void						BrowseGameLocation();
	std::tstring				BrowseForFolder(const TCHAR*);

	void						UpdateGameLocation();

	Framework::Win32::CEdit		m_gameLocationEdit;
};
