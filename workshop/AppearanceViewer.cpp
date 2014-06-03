#include "AppearanceViewer.h"
#include "resource.h"
#include "win32/Static.h"

CAppearanceViewer::CAppearanceViewer()
: CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER))
{
	SetClassPtr();

	auto placeholder = Framework::Win32::CStatic(GetItem(IDC_APPEARANCE_RENDER_PLACEHOLDER));
	auto placeholderRect = placeholder.GetWindowRect();
	placeholderRect.ScreenToClient(m_hWnd);

	m_embedControl = std::make_shared<Palleon::CWin32EmbedControl>(m_hWnd, placeholderRect);
}

CAppearanceViewer::~CAppearanceViewer()
{

}

long CAppearanceViewer::OnCommand(unsigned short cmd, unsigned short, HWND)
{
	switch(cmd)
	{
	case IDCANCEL:
		Destroy();
		break;
	}
	return FALSE;
}
