#include "AppearanceViewerActorInfoPane.h"
#include "resource.h"
#include "string_format.h"

CAppearanceViewerActorInfoPane::CAppearanceViewerActorInfoPane(HWND parentWnd)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER_ACTORINFOPANE), parentWnd)
{
	SetClassPtr();
	m_modelIdEdit = Framework::Win32::CEdit(GetItem(IDC_APPEARANCEVIEWER_MODELIDEDIT));
}

CAppearanceViewerActorInfoPane::~CAppearanceViewerActorInfoPane()
{

}

void CAppearanceViewerActorInfoPane::SetModelId(uint32 modelId)
{
	m_modelIdEdit.SetText(string_format(_T("%u"), modelId).c_str());
}
