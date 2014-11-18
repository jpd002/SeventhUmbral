#include "ZoneeditorPropertiesPane.h"
#include "resource.h"
#include "string_format.h"

CZoneEditorPropertiesPane::CZoneEditorPropertiesPane(HWND parentWnd)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_ZONEEDITOR_PROPERTIESPANE), parentWnd)
{
	SetClassPtr();

	m_idEdit = Framework::Win32::CEdit(GetItem(IDC_ZONEEDITOR_PROPERTIESPANE_IDEDIT));
	m_baseModelIdEdit = Framework::Win32::CEdit(GetItem(IDC_ZONEEDITOR_PROPERTIESPANE_BASEMODELIDEDIT));
}

CZoneEditorPropertiesPane::~CZoneEditorPropertiesPane()
{

}

void CZoneEditorPropertiesPane::SetSelection(uint32 id, uint32 baseModelId)
{
	m_idEdit.SetText(string_format(_T("%d"), id).c_str());
	m_baseModelIdEdit.SetText(string_format(_T("%d"), baseModelId).c_str());
}
