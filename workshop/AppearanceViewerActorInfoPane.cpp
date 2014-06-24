#include "AppearanceViewerActorInfoPane.h"
#include "resource.h"

CAppearanceViewerActorInfoPane::CAppearanceViewerActorInfoPane(HWND parentWnd)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER_ACTORINFOPANE), parentWnd)
{
	SetClassPtr();
	m_modelIdEdit = Framework::Win32::CEdit(GetItem(IDC_APPEARANCEVIEWER_MODELIDEDIT));
//	SetActor(40000 + actorInfo.baseModelId, actorInfo.topModelId * 0x400);
//	m_modelIdEdit.SetText(string_format(_T("%d"), (actorInfo.baseModelId << 20) | (actorInfo.topModelId << 10)).c_str());
}

CAppearanceViewerActorInfoPane::~CAppearanceViewerActorInfoPane()
{

}
