#include "AppearanceViewer.h"
#include "win32/HorizontalSplitter.h"
#include "win32/VerticalSplitter.h"
#include "resource.h"

CAppearanceViewer::CAppearanceViewer(HWND parentWnd)
: CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER), parentWnd)
{
	SetClassPtr();
	m_mainSplitter = std::make_unique<Framework::Win32::CHorizontalSplitter>(m_hWnd, GetClientRect());
	m_subSplitter = std::make_unique<Framework::Win32::CVerticalSplitter>(m_mainSplitter->m_hWnd, GetClientRect());
	m_actorListPane = std::make_unique<CAppearanceViewerActorListPane>(m_mainSplitter->m_hWnd);
	m_actorViewPane = std::make_unique<CAppearanceViewerActorViewPane>(m_subSplitter->m_hWnd);
	m_actorInfoPane = std::make_unique<CAppearanceViewerActorInfoPane>(m_subSplitter->m_hWnd);
	m_mainSplitter->SetEdgePosition(0.25);
	m_subSplitter->SetEdgePosition(0.90);

	m_mainSplitter->SetChild(0, m_actorListPane->m_hWnd);
	m_mainSplitter->SetChild(1, m_subSplitter->m_hWnd);

	m_subSplitter->SetChild(0, m_actorViewPane->m_hWnd);
	m_subSplitter->SetChild(1, m_actorInfoPane->m_hWnd);
}

CAppearanceViewer::~CAppearanceViewer()
{

}

long CAppearanceViewer::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto rect = GetClientRect();
	m_mainSplitter->SetSizePosition(rect);
	return FALSE;
}

long CAppearanceViewer::OnNotify(WPARAM, LPNMHDR nmhdr)
{
	if(CWindow::IsNotifySource(m_actorListPane.get(), nmhdr))
	{
		switch(nmhdr->code)
		{
		case CAppearanceViewerActorListPane::NOTIFY_SELCHANGE:
			OnActorListPaneSelChange(reinterpret_cast<CAppearanceViewerActorListPane::SELCHANGE_INFO*>(nmhdr));
			break;
		}
	}
	return FALSE;
}

void CAppearanceViewer::OnActorListPaneSelChange(CAppearanceViewerActorListPane::SELCHANGE_INFO* selChangeInfo)
{
	m_actorViewPane->SetActor(selChangeInfo->baseModelId, selChangeInfo->topModelId);
}
