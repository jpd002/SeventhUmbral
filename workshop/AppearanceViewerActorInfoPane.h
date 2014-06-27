#pragma once

#include "win32/Dialog.h"
#include "win32/Edit.h"

class CAppearanceViewerActorInfoPane : public Framework::Win32::CDialog
{
public:
								CAppearanceViewerActorInfoPane(HWND);
	virtual						~CAppearanceViewerActorInfoPane();

	void						SetModelId(uint32);

private:
	Framework::Win32::CEdit		m_modelIdEdit;
};
