#include "AppConfig.h"
#include "AppPreferences.h"
#include "WorldViewer.h"
#include "resource.h"
#include "palleon\EmbedRemoteCall.h"


CWorldViewer::CWorldViewer(HWND parentWnd, uint32 mapId)
: CDialog(MAKEINTRESOURCE(IDD_WORLDVIEWER), parentWnd)
, m_mapId(mapId)
{
	SetClassPtr();
	CreateViewer();
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
		rpc.SetMethod("SetMap");
		rpc.SetParam("MapId", std::to_string(m_mapId));
		auto result = m_embedControl->ExecuteCommand(rpc.ToString());
	}
}
