#include "SheetViewer.h"
#include "resource.h"
#include "win32/HorizontalSplitter.h"
#include "win32/MenuItem.h"

const CSheetViewer::LanguageMenuValueMap CSheetViewer::m_languageMenuValues =
{
	std::make_pair(ID_SHEETVIEWER_LANGUAGE_JAPANESE,	"jp"),
	std::make_pair(ID_SHEETVIEWER_LANGUAGE_ENGLISH,		"en"),
	std::make_pair(ID_SHEETVIEWER_LANGUAGE_FRENCH,		"fr"),
	std::make_pair(ID_SHEETVIEWER_LANGUAGE_GERMAN,		"de")
};

CSheetViewer::CSheetViewer()
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_SHEETVIEWER))
{
	SetClassPtr();

	m_splitter = std::make_unique<Framework::Win32::CHorizontalSplitter>(m_hWnd, GetClientRect());
	m_schemaPane = std::make_unique<CSheetViewerSchemaPane>(m_splitter->m_hWnd);
	m_dataPane = std::make_unique<CSheetViewerDataPane>(m_splitter->m_hWnd);

	m_splitter->SetEdgePosition(0.25);
	m_splitter->SetChild(0, m_schemaPane->m_hWnd);
	m_splitter->SetChild(1, m_dataPane->m_hWnd);

	SetLanguage(ID_SHEETVIEWER_LANGUAGE_JAPANESE);
}

CSheetViewer::~CSheetViewer()
{

}

long CSheetViewer::OnCommand(unsigned short cmd, unsigned short, HWND)
{
	switch(cmd)
	{
	case ID_SHEETVIEWER_LANGUAGE_JAPANESE:
	case ID_SHEETVIEWER_LANGUAGE_ENGLISH:
	case ID_SHEETVIEWER_LANGUAGE_FRENCH:
	case ID_SHEETVIEWER_LANGUAGE_GERMAN:
		SetLanguage(cmd);
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return FALSE;
}

long CSheetViewer::OnNotify(WPARAM, LPNMHDR nmhdr)
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
	return FALSE;
}

long CSheetViewer::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto clientRect = GetClientRect();
	m_splitter->SetSizePosition(clientRect);
	return FALSE;
}

void CSheetViewer::SetLanguage(uint32 menuItemId)
{
	auto menuItemValueIterator = m_languageMenuValues.find(menuItemId);
	if(menuItemValueIterator == std::end(m_languageMenuValues))
	{
		menuItemValueIterator = m_languageMenuValues.begin();
	}
	m_dataPane->SetLanguage(menuItemValueIterator->second);

	Framework::Win32::CMenuItem mainMenu(GetMenu(m_hWnd));
	for(const auto& menuItemValue : m_languageMenuValues)
	{
		auto subMenuItem = mainMenu.FindById(menuItemValue.first);
		if(!subMenuItem.IsEmpty())
		{
			subMenuItem.Check(strcmp(menuItemValue.second, menuItemValueIterator->second) == 0);
		}
	}
}

void CSheetViewer::OnSchemaPaneSelChange(CSheetViewerSchemaPane::SELCHANGE_INFO* selChangeInfo)
{
	m_dataPane->SetSheetInfo(selChangeInfo->sheetName, selChangeInfo->sheetId);
}
