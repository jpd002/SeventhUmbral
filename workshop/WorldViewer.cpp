#include "AppConfig.h"
#include "AppPreferences.h"
#include "WorldViewer.h"
#include "resource.h"
#include "palleon\EmbedRemoteCall.h"


CWorldViewer::CWorldViewer(HWND parentWnd)
: CDialog(MAKEINTRESOURCE(IDD_WORLDVIEWER), parentWnd)
{
	SetClassPtr();
	CreateViewer();
}

CWorldViewer::~CWorldViewer()
{

}

std::string CWorldViewer::GetName() const
{
	return "World";
}

void CWorldViewer::SetActive(bool active)
{
	m_embedControl->SetRunning(active);
}

long CWorldViewer::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto rect = GetClientRect();
	if(m_embedControl)
	{
		m_embedControl->SetSizePosition(rect);
	}
	return FALSE;
}

void CWorldViewer::CreateViewer()
{
	assert(!m_embedControl);

	TCHAR moduleFileName[_MAX_PATH];
	GetModuleFileName(NULL, moduleFileName, _MAX_PATH);
	boost::filesystem::path dataPath(moduleFileName);
	dataPath.remove_leaf();
	dataPath /= "worldeditor";

	m_embedControl = std::make_shared<Palleon::CWin32EmbedControl>(m_hWnd, GetClientRect(),
		_T("WorldEditor.exe"), dataPath.native().c_str());
	{
		Palleon::CEmbedRemoteCall rpc;
		rpc.SetMethod("SetGamePath");
		rpc.SetParam("Path", CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));
		m_embedControl->ExecuteCommand(rpc.ToString());
	}
	{
		Palleon::CEmbedRemoteCall rpc;
		rpc.SetMethod("SetActor");
		m_embedControl->ExecuteCommand(rpc.ToString());
	}
}
