#include "AppConfig.h"
#include "AppPreferences.h"
#include "WorldEditorControl.h"
#include "resource.h"
#include "string_format.h"
#include "palleon/EmbedRemoteCall.h"
#include "math/MathStringUtils.h"
#include "../worldeditor/RpcDefs.h"

static bool HasRpcSucceeded(const std::string& rpcString)
{
	auto rpc = Palleon::CEmbedRemoteCall(rpcString);
	return rpc.GetParam(WORLDEDITOR_PARAM_RESULT) == WORLDEDITOR_RESULT_SUCCEEDED;
}

CWorldEditorControl::CWorldEditorControl(HWND parentWnd)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_ZONEEDITOR_WORLDPANE), parentWnd)
{
	SetClassPtr();
	CreateViewer();
}

CWorldEditorControl::~CWorldEditorControl()
{

}

void CWorldEditorControl::SetMap(uint32 mapId)
{
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetMethod(WORLDEDITOR_METHOD_SETMAP);
	rpc.SetParam(WORLDEDITOR_PARAM_MAPID, std::to_string(mapId));
	auto result = m_embedControl->ExecuteCommand(rpc.ToString());
	assert(HasRpcSucceeded(result));
}

void CWorldEditorControl::SetControlScheme(CONTROL_SCHEME scheme)
{
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetMethod(WORLDEDITOR_METHOD_SETCONTROLSCHEME);
	uint32 controlSchemeId = WORLDEDITOR_CONTROLSCHEME_EDITOR;
	switch(scheme)
	{
	case CONTROL_SCHEME::EDITOR:
		controlSchemeId = WORLDEDITOR_CONTROLSCHEME_EDITOR;
		break;
	case CONTROL_SCHEME::VIEWER:
		controlSchemeId = WORLDEDITOR_CONTROLSCHEME_VIEWER;
		break;
	default:
		assert(false);
		break;
	}
	rpc.SetParam(WORLDEDITOR_PARAM_CONTROLSCHEME, std::to_string(controlSchemeId));
	auto result = m_embedControl->ExecuteCommand(rpc.ToString());
	assert(HasRpcSucceeded(result));
}

CVector3 CWorldEditorControl::GetCameraPosition()
{
	Palleon::CEmbedRemoteCall outRpc;
	outRpc.SetMethod(WORLDEDITOR_METHOD_GETCAMERAPOSITION);
	auto inRpcString = m_embedControl->ExecuteCommand(outRpc.ToString());
	assert(HasRpcSucceeded(inRpcString));
	auto inRpc = Palleon::CEmbedRemoteCall(inRpcString);
	auto position = MathStringUtils::ParseVector3(inRpc.GetParam(WORLDEDITOR_PARAM_POSITION));
	return position;
}

void CWorldEditorControl::SetCameraPosition(const CVector3& position)
{
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetMethod(WORLDEDITOR_METHOD_SETCAMERAPOSITION);
	rpc.SetParam(WORLDEDITOR_PARAM_POSITION, MathStringUtils::ToString(position));
	auto result = m_embedControl->ExecuteCommand(rpc.ToString());
	assert(HasRpcSucceeded(result));
}

void CWorldEditorControl::CreateActor(uint32 actorId)
{
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetMethod(WORLDEDITOR_METHOD_CREATEACTOR);
	rpc.SetParam(WORLDEDITOR_PARAM_ID, std::to_string(actorId));
	auto result = m_embedControl->ExecuteCommand(rpc.ToString());
	assert(HasRpcSucceeded(result));
}

void CWorldEditorControl::SetActorBaseModelId(uint32 actorId, uint32 baseModelId)
{
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetMethod(WORLDEDITOR_METHOD_SETACTORBASEMODELID);
	rpc.SetParam(WORLDEDITOR_PARAM_ID, std::to_string(actorId));
	rpc.SetParam(WORLDEDITOR_PARAM_BASEMODELID, std::to_string(baseModelId));
	auto result = m_embedControl->ExecuteCommand(rpc.ToString());
	assert(HasRpcSucceeded(result));
}

void CWorldEditorControl::SetActorTopModelId(uint32 actorId, uint32 topModelId)
{
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetMethod(WORLDEDITOR_METHOD_SETACTORTOPMODELID);
	rpc.SetParam(WORLDEDITOR_PARAM_ID, std::to_string(actorId));
	rpc.SetParam(WORLDEDITOR_PARAM_TOPMODELID, std::to_string(topModelId));
	auto result = m_embedControl->ExecuteCommand(rpc.ToString());
	assert(HasRpcSucceeded(result));
}

void CWorldEditorControl::SetActorPosition(uint32 actorId, const CVector3& position)
{
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetMethod(WORLDEDITOR_METHOD_SETACTORPOSITION);
	rpc.SetParam(WORLDEDITOR_PARAM_ID, std::to_string(actorId));
	rpc.SetParam(WORLDEDITOR_PARAM_POSITION, MathStringUtils::ToString(position));
	auto result = m_embedControl->ExecuteCommand(rpc.ToString());
	assert(HasRpcSucceeded(result));
}

void CWorldEditorControl::SetActive(bool active)
{
	m_embedControl->SetRunning(active);
}

long CWorldEditorControl::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto rect = GetClientRect();
	if(m_embedControl)
	{
		m_embedControl->SetSizePosition(rect);
	}
	return FALSE;
}

void CWorldEditorControl::CreateViewer()
{
	assert(!m_embedControl);

	TCHAR moduleFileName[_MAX_PATH];
	GetModuleFileName(NULL, moduleFileName, _MAX_PATH);
	boost::filesystem::path dataPath(moduleFileName);
	dataPath.remove_leaf();
	dataPath /= "worldeditor";

	m_embedControl = std::make_shared<Palleon::CWin32EmbedControl>(m_hWnd, GetClientRect(),
		_T("WorldEditor.exe"), dataPath.native().c_str());

	m_embedControl->NotificationRaised.connect(NotificationRaised);

	{
		Palleon::CEmbedRemoteCall rpc;
		rpc.SetMethod(WORLDEDITOR_METHOD_SETGAMEPATH);
		rpc.SetParam(WORLDEDITOR_PARAM_PATH, CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));
		auto result = m_embedControl->ExecuteCommand(rpc.ToString());
		assert(HasRpcSucceeded(result));
	}
}
