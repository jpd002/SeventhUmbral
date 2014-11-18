#pragma once

#include "win32/Dialog.h"
#include "win32/Edit.h"

class CZoneEditorPropertiesPane : public Framework::Win32::CDialog
{
public:
								CZoneEditorPropertiesPane(HWND);
	virtual						~CZoneEditorPropertiesPane();

	void						SetSelection(uint32, uint32);

private:
	Framework::Win32::CEdit		m_idEdit;
	Framework::Win32::CEdit		m_baseModelIdEdit;
};
