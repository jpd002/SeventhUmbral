#include "SheetViewer.h"
#include "resource.h"
#include "win32/HorizontalSplitter.h"
#include "win32/VerticalSplitter.h"
#include "win32/MenuItem.h"
#include "string_format.h"

const CSheetViewer::LanguageMenuValueMap CSheetViewer::m_languageMenuValues =
{
	std::make_pair(CSheetViewerToolbarPane::LANGUAGE_JAPANESE,	"jp"),
	std::make_pair(CSheetViewerToolbarPane::LANGUAGE_ENGLISH,	"en"),
	std::make_pair(CSheetViewerToolbarPane::LANGUAGE_FRENCH,	"fr"),
	std::make_pair(CSheetViewerToolbarPane::LANGUAGE_GERMAN,	"de")
};

CSheetViewer::CSheetViewer(HWND parentWnd, uint32 schemaId)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_SHEETVIEWER), parentWnd)
, m_schemaId(schemaId)
{
	SetClassPtr();

	m_splitter = std::make_unique<Framework::Win32::CVerticalSplitter>(m_hWnd, GetClientRect());

	m_subSplitter = std::make_unique<Framework::Win32::CHorizontalSplitter>(m_splitter->m_hWnd, GetClientRect());
	m_toolbarPane = std::make_unique<CSheetViewerToolbarPane>(m_splitter->m_hWnd);

	m_schemaPane = std::make_unique<CSheetViewerSchemaPane>(m_subSplitter->m_hWnd, schemaId);
	m_dataPane = std::make_unique<CSheetViewerDataPane>(m_subSplitter->m_hWnd);

	m_splitter->SetEdgePosition(0.05f);
	m_splitter->SetFixed(true);
	m_splitter->SetChild(0, m_toolbarPane->m_hWnd);
	m_splitter->SetChild(1, m_subSplitter->m_hWnd);

	m_subSplitter->SetEdgePosition(0.25f);
	m_subSplitter->SetChild(0, m_schemaPane->m_hWnd);
	m_subSplitter->SetChild(1, m_dataPane->m_hWnd);

	SetLanguage(m_toolbarPane->GetSelectedLanguage());
}

CSheetViewer::~CSheetViewer()
{

}

std::string CSheetViewer::GetName() const
{
	const char* schemaName = (m_schemaId == CSheetSchema::GAME_SCHEMA_FILE_ID) ? "Game Data" : "Equipment Variations";
	return string_format("Data Sheets - %s", schemaName);
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
	else if(CWindow::IsNotifySource(m_toolbarPane.get(), nmhdr))
	{
		switch(nmhdr->code)
		{
		case CSheetViewerToolbarPane::NOTIFY_LANGUAGE_CHANGE:
			{
				auto languageChangeInfo = reinterpret_cast<CSheetViewerToolbarPane::LANGUAGE_CHANGE_INFO*>(nmhdr);
				SetLanguage(languageChangeInfo->language);
			}
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

void CSheetViewer::SetLanguage(CSheetViewerToolbarPane::LANGUAGE language)
{
	auto menuItemValueIterator = m_languageMenuValues.find(language);
	if(menuItemValueIterator == std::end(m_languageMenuValues))
	{
		menuItemValueIterator = m_languageMenuValues.begin();
	}
	m_dataPane->SetLanguage(menuItemValueIterator->second);
}

void CSheetViewer::OnSchemaPaneSelChange(CSheetViewerSchemaPane::SELCHANGE_INFO* selChangeInfo)
{
	m_dataPane->SetSheetInfo(selChangeInfo->sheetName, selChangeInfo->sheetId);
}
