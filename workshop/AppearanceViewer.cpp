#include "AppearanceViewer.h"
#include "win32/HorizontalSplitter.h"
#include "resource.h"

CAppearanceViewer::CAppearanceViewer(HWND parentWnd)
: CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER), parentWnd)
{
	SetClassPtr();
	m_splitter = std::make_unique<Framework::Win32::CHorizontalSplitter>(m_hWnd, GetClientRect());
	m_actorListPane = std::make_unique<CAppearanceViewerActorListPane>(m_splitter->m_hWnd);
	m_actorViewPane = std::make_unique<CAppearanceViewerActorViewPane>(m_splitter->m_hWnd);
	m_splitter->SetEdgePosition(0.25);

	m_splitter->SetChild(0, m_actorListPane->m_hWnd);
	m_splitter->SetChild(1, m_actorViewPane->m_hWnd);

	m_actorViewPane->SetActor(40031, 0x400);

//	m_modelIdEdit = Framework::Win32::CEdit(GetItem(IDC_APPEARANCEVIEWER_MODELIDEDIT));
//	SetActor(40000 + actorInfo.baseModelId, actorInfo.topModelId * 0x400);
//	m_modelIdEdit.SetText(string_format(_T("%d"), (actorInfo.baseModelId << 20) | (actorInfo.topModelId << 10)).c_str());
}

CAppearanceViewer::~CAppearanceViewer()
{

}

long CAppearanceViewer::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto rect = GetClientRect();
	m_splitter->SetSizePosition(rect);
	return FALSE;
}
