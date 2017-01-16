#include "WorldViewer.h"
#include "resource.h"

CWorldViewer::CWorldViewer(HWND parentWnd, uint32 mapId)
: CDialog(MAKEINTRESOURCE(IDD_WORLDVIEWER), parentWnd)
, m_mapId(mapId)
{
	SetClassPtr();
	m_viewer = std::make_unique<CWorldEditorControl>(m_hWnd);
	m_viewer->SetControlScheme(CWorldEditorControl::CONTROL_SCHEME::EDITOR);
	m_viewer->SetMap(m_mapId);
}

CWorldViewer::~CWorldViewer()
{

}

std::string CWorldViewer::GetName() const
{
	switch(m_mapId)
	{
	case 0xA09B0000:
		return "World - Gridania Inn Room";
		break;
	case 0xA09B0001:
		return "World - Limsa Lominsa Inn Room";
		break;
	case 0xA09B0002:
		return "World - Ul'dah Inn Room";
		break;
	default:
		return "World";
	}
}

void CWorldViewer::SetActive(bool active)
{
	m_viewer->SetActive(active);
}

long CWorldViewer::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto rect = GetClientRect();
	if(m_viewer)
	{
		m_viewer->SetSizePosition(rect);
	}
	return FALSE;
}
