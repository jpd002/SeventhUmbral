#include "SheetViewerSchemaPane.h"
#include "resource.h"
#include "../dataobjects/FileManager.h"
#include "string_cast.h"
#include "StdStreamUtils.h"

CSheetViewerSchemaPane::CSheetViewerSchemaPane(HWND parentWnd, uint32 schemaId)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_SHEETVIEWER_SCHEMAPANE), parentWnd)
{
	SetClassPtr();

	m_schemaListView = GetItem(IDC_SHEETSCHEMA_LIST);
	m_schemaListView.SetExtendedListViewStyle(m_schemaListView.GetExtendedListViewStyle() | LVS_EX_FULLROWSELECT);

	//Schema View Columns
	{
		LVCOLUMN col = {};
		col.pszText = _T("Name");
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.cx = m_schemaListView.GetClientRect().Right();
		m_schemaListView.InsertColumn(0, col);
	}

	{
		auto schemaFilePath = CFileManager::GetResourcePath(schemaId);
		auto schemaStream = Framework::CreateInputStdStream(schemaFilePath.native());
		m_schema = CSheetSchema(schemaStream);
	}

	PopulateSchemaView();
}

CSheetViewerSchemaPane::~CSheetViewerSchemaPane()
{

}

long CSheetViewerSchemaPane::OnNotify(WPARAM, LPNMHDR nmhdr)
{
	if(nmhdr->idFrom == IDC_SHEETSCHEMA_LIST)
	{
		switch(nmhdr->code)
		{
		case LVN_ITEMCHANGED:
			OnSchemaViewItemChanged(reinterpret_cast<NMLISTVIEW*>(nmhdr));
			break;
		}
	}
	return FALSE;
}

long CSheetViewerSchemaPane::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto clientRect = GetClientRect();
	m_schemaListView.SetSizePosition(clientRect);
	return FALSE;
}

void CSheetViewerSchemaPane::OnSchemaViewItemChanged(NMLISTVIEW* listView)
{
	if(listView->uNewState & LVIS_SELECTED)
	{
		auto sheetName = m_schemaListView.GetItemText(listView->iItem, 0);
		uint32 sheetId = m_schemaListView.GetItemData(listView->iItem);
		SELCHANGE_INFO selChangeInfo = {};
		selChangeInfo.code		= NOTIFY_SELCHANGE;
		selChangeInfo.hwndFrom	= m_hWnd;
		selChangeInfo.idFrom	= 0;
		selChangeInfo.sheetName	= string_cast<std::string>(sheetName);
		selChangeInfo.sheetId	= sheetId;
		SendMessage(GetParent(), WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selChangeInfo));
	}
}

void CSheetViewerSchemaPane::PopulateSchemaView()
{
	m_schemaListView.DeleteAllItems();
	for(const auto& sheetInfo : m_schema.GetSheets())
	{
		auto sheetName = string_cast<std::tstring>(sheetInfo.name);
		LVITEM item = {};
		item.pszText = const_cast<TCHAR*>(sheetName.c_str());
		item.lParam = sheetInfo.fileId;
		item.iItem = INT_MAX;
		item.mask = LVIF_TEXT | LVIF_PARAM;
		m_schemaListView.InsertItem(item);
	}
}
