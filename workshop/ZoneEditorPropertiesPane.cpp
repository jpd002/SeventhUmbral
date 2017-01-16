#include "ZoneeditorPropertiesPane.h"
#include "resource.h"
#include "string_format.h"
#include "boost/lexical_cast.hpp"

CZoneEditorPropertiesPane::CZoneEditorPropertiesPane(HWND parentWnd)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_ZONEEDITOR_PROPERTIESPANE), parentWnd)
{
	SetClassPtr();
	m_propertyGrid = std::make_unique<Framework::Win32::CPropertyGrid>(m_hWnd);
}

CZoneEditorPropertiesPane::~CZoneEditorPropertiesPane()
{

}

void CZoneEditorPropertiesPane::SetSelection(const Framework::Win32::PropertyBagPtr& propertyBag)
{
	m_propertyGrid->SetPropertyBag(propertyBag);
}

void CZoneEditorPropertiesPane::Refresh()
{
	RedrawWindow(m_propertyGrid->m_hWnd, nullptr, NULL, RDW_ALLCHILDREN | RDW_ERASENOW | RDW_UPDATENOW | RDW_INVALIDATE);
}

long CZoneEditorPropertiesPane::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto clientRect = GetClientRect();
	m_propertyGrid->SetSizePosition(clientRect);
	return TRUE;
}
