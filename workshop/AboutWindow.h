#pragma once

#include "win32/Window.h"

class CAboutWindow
{
public:
								CAboutWindow();
	virtual						~CAboutWindow();

	void						DoModal(HWND);

private:
	static HRESULT CALLBACK		TaskDialogCallback(HWND, UINT, WPARAM, LPARAM, LONG_PTR);
};
