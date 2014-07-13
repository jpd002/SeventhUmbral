#include "AboutWindow.h"
#include "AppDef.h"
#include <shobjidl.h>

CAboutWindow::CAboutWindow()
{

}

CAboutWindow::~CAboutWindow()
{

}

void CAboutWindow::DoModal(HWND parentWnd)
{
	const TCHAR* message = _T("Version ") APP_VERSIONSTR _T("\r\n\r\n")
		_T("<a href=\"siteurl\">http://seventhumbral.org</a>");
	TASKDIALOGCONFIG tdc = {};
	tdc.cbSize = sizeof(TASKDIALOGCONFIG);
	tdc.pszWindowTitle		= _T("About ");
	tdc.dwCommonButtons		= TDCBF_OK_BUTTON;
	tdc.nDefaultButton		= IDOK;
	tdc.pszMainInstruction	= APP_NAME;
	tdc.dwFlags				= TDF_ENABLE_HYPERLINKS;
	tdc.pszContent			= message;
	tdc.pszMainIcon			= TD_INFORMATION_ICON;
	tdc.pfCallback			= &TaskDialogCallback;
	tdc.hwndParent			= parentWnd;
	TaskDialogIndirect(&tdc, NULL, NULL, NULL);
}

HRESULT CALLBACK CAboutWindow::TaskDialogCallback(HWND, UINT uNotification, WPARAM, LPARAM, LONG_PTR)
{
	if(uNotification == TDN_HYPERLINK_CLICKED)
	{
		ShellExecute(NULL, _T("open"), _T("http://seventhumbral.org"), NULL, NULL, SW_SHOW);
	}
	return S_OK;
}
