#include "SheetViewerToolbarPane.h"
#include "resource.h"

const CSheetViewerToolbarPane::LanguageToolBarItemMap CSheetViewerToolbarPane::m_languageToolBarItems =
{
	std::make_pair(LANGUAGE_JAPANESE,	LANGUAGE_TOOLBAR_ITEM(_T("JP"), _T("日本語"))),
	std::make_pair(LANGUAGE_ENGLISH,	LANGUAGE_TOOLBAR_ITEM(_T("EN"), _T("English"))),
	std::make_pair(LANGUAGE_FRENCH,		LANGUAGE_TOOLBAR_ITEM(_T("FR"), _T("Français"))),
	std::make_pair(LANGUAGE_GERMAN,		LANGUAGE_TOOLBAR_ITEM(_T("DE"), _T("Deutsch")))
};

CSheetViewerToolbarPane::CSheetViewerToolbarPane(HWND parentWnd)
: CDialog(MAKEINTRESOURCE(IDD_SHEETVIEWER_TOOLBARPANE), parentWnd)
{
	SetClassPtr();

	m_toolbar = Framework::Win32::CToolBar(m_hWnd);

	for(const auto& toolbarItemPair : m_languageToolBarItems)
	{
		const auto& toolbarItem(toolbarItemPair.second);
		m_toolbar.InsertTextButton(toolbarItem.buttonText, toolbarItemPair.first);
		m_toolbar.SetButtonToolTipText(toolbarItemPair.first, toolbarItem.toolTipText);
	}

	m_toolbar.SetButtonChecked(m_selectedLanguage, true);
}

CSheetViewerToolbarPane::~CSheetViewerToolbarPane()
{

}

CSheetViewerToolbarPane::LANGUAGE CSheetViewerToolbarPane::GetSelectedLanguage() const
{
	return m_selectedLanguage;
}

long CSheetViewerToolbarPane::OnSize(unsigned int, unsigned int, unsigned int)
{
	m_toolbar.Resize();
	return TRUE;
}

long CSheetViewerToolbarPane::OnNotify(WPARAM wparam, NMHDR* hdr)
{
	m_toolbar.ProcessNotify(wparam, hdr);
	return FALSE;
}

long CSheetViewerToolbarPane::OnCommand(unsigned short id, unsigned short, HWND)
{
	switch(id)
	{
	case LANGUAGE_JAPANESE:
	case LANGUAGE_ENGLISH:
	case LANGUAGE_FRENCH:
	case LANGUAGE_GERMAN:
		{
			m_toolbar.SetButtonChecked(m_selectedLanguage, false);
			m_selectedLanguage = static_cast<LANGUAGE>(id);
			m_toolbar.SetButtonChecked(m_selectedLanguage, true);

			LANGUAGE_CHANGE_INFO languageChangeInfo = {};
			languageChangeInfo.code			= NOTIFY_LANGUAGE_CHANGE;
			languageChangeInfo.hwndFrom		= m_hWnd;
			languageChangeInfo.idFrom		= 0;
			languageChangeInfo.language		= m_selectedLanguage;
			SendMessage(GetParent(), WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&languageChangeInfo));
		}
		break;
	}
	return TRUE;
}
