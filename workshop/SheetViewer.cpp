#include "SheetViewer.h"
#include "resource.h"
#include "win32/HorizontalSplitter.h"
#include "win32/MenuItem.h"
#include "string_format.h"

const CSheetViewer::LanguageToolBarItemMap CSheetViewer::m_languageToolBarItems =
{
	std::make_pair(LANGUAGE_JAPANESE,	LANGUAGE_TOOLBAR_ITEM(_T("JP"), _T("日本語"), "jp")),
	std::make_pair(LANGUAGE_ENGLISH,	LANGUAGE_TOOLBAR_ITEM(_T("EN"), _T("English"), "en")),
	std::make_pair(LANGUAGE_FRENCH,		LANGUAGE_TOOLBAR_ITEM(_T("FR"), _T("Français"), "fr")),
	std::make_pair(LANGUAGE_GERMAN,		LANGUAGE_TOOLBAR_ITEM(_T("DE"), _T("Deutsch"), "de"))
};

CSheetViewer::CSheetViewer(HWND parentWnd, uint32 schemaId)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_SHEETVIEWER), parentWnd)
, m_schemaId(schemaId)
{
	SetClassPtr();

	m_toolbar = Framework::Win32::CToolBar(m_hWnd);
	m_splitter = std::make_unique<Framework::Win32::CHorizontalSplitter>(m_hWnd, GetClientRect());
	m_schemaPane = std::make_unique<CSheetViewerSchemaPane>(m_splitter->m_hWnd, schemaId);
	m_dataPane = std::make_unique<CSheetViewerDataPane>(m_splitter->m_hWnd);

	for(const auto& toolbarItemPair : m_languageToolBarItems)
	{
		const auto& toolbarItem(toolbarItemPair.second);
		m_toolbar.InsertTextButton(toolbarItem.buttonText, toolbarItemPair.first);
		m_toolbar.SetButtonToolTipText(toolbarItemPair.first, toolbarItem.toolTipText);
	}

	m_toolbar.SetButtonChecked(m_selectedLanguage, true);
	m_toolbar.Resize();

	m_splitter->SetEdgePosition(0.25f);
	m_splitter->SetChild(0, m_schemaPane->m_hWnd);
	m_splitter->SetChild(1, m_dataPane->m_hWnd);

	SetLanguage(m_selectedLanguage);
}

CSheetViewer::~CSheetViewer()
{

}

std::string CSheetViewer::GetName() const
{
	const char* schemaName = (m_schemaId == CSheetSchema::GAME_SCHEMA_FILE_ID) ? "Game Data" : "Equipment Variations";
	return string_format("Data Sheets - %s", schemaName);
}

long CSheetViewer::OnNotify(WPARAM wparam, LPNMHDR nmhdr)
{
	if(CWindow::IsNotifySource(m_schemaPane.get(), nmhdr))
	{
		switch(nmhdr->code)
		{
		case CSheetViewerSchemaPane::NOTIFY_SELCHANGE:
			OnSchemaPaneSelChange(reinterpret_cast<CSheetViewerSchemaPane::SELCHANGE_INFO*>(nmhdr));
			break;
		}
	}
	m_toolbar.ProcessNotify(wparam, nmhdr);
	return FALSE;
}

long CSheetViewer::OnCommand(unsigned short id, unsigned short, HWND)
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
			SetLanguage(m_selectedLanguage);
		}
		break;
	}
	return TRUE;
}

long CSheetViewer::OnSize(unsigned int, unsigned int, unsigned int)
{
	m_toolbar.Resize();
	auto clientRect = GetClientRect();
	auto toolbarClientRect = m_toolbar.GetWindowRect();
	clientRect.SetTop(toolbarClientRect.Height());
	m_splitter->SetSizePosition(clientRect);
	return FALSE;
}

void CSheetViewer::SetLanguage(LANGUAGE language)
{
	auto languageToolBarItemIterator = m_languageToolBarItems.find(language);
	if(languageToolBarItemIterator == std::end(m_languageToolBarItems))
	{
		languageToolBarItemIterator = m_languageToolBarItems.begin();
	}
	const auto& languageToolBarItem = languageToolBarItemIterator->second;
	m_dataPane->SetLanguage(languageToolBarItem.languageCode);
}

void CSheetViewer::OnSchemaPaneSelChange(CSheetViewerSchemaPane::SELCHANGE_INFO* selChangeInfo)
{
	m_dataPane->SetSheetInfo(selChangeInfo->sheetName, selChangeInfo->sheetId);
}
