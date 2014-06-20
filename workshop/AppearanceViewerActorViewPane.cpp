#include "AppConfig.h"
#include "AppPreferences.h"
#include "AppearanceViewerActorViewPane.h"
#include "resource.h"
#include "palleon\EmbedRemoteCall.h"

CAppearanceViewerActorViewPane::CAppearanceViewerActorViewPane(HWND parentWnd)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER_ACTORVIEWPANE), parentWnd)
{
	SetClassPtr();
	CreateActorViewer();
	m_failLabel = Framework::Win32::CStatic(GetItem(IDC_APPEARANCEVIEWER_FAILLABEL));
}

CAppearanceViewerActorViewPane::~CAppearanceViewerActorViewPane()
{

}

long CAppearanceViewerActorViewPane::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto rect = GetClientRect();
	m_embedControl->SetSizePosition(rect);
	return FALSE;
}

void CAppearanceViewerActorViewPane::SetActor(uint32 baseModelId, uint32 topModelId)
{
	m_failLabel.Show(SW_HIDE);
	try
	{
		Palleon::CEmbedRemoteCall rpc;
		rpc.SetMethod("SetActor");
		rpc.SetParam("BaseModelId", std::to_string(baseModelId));
		rpc.SetParam("TopModelId", std::to_string(topModelId));
		m_embedControl->ExecuteCommand(rpc.ToString());
	}
	catch(...)
	{
		auto failLabelRect = m_failLabel.GetWindowRect();
		auto embedControlRect = m_embedControl->GetWindowRect();
		failLabelRect.ScreenToClient(m_hWnd);
		embedControlRect.ScreenToClient(m_hWnd);
		int failLabelWidth = failLabelRect.Width();
		int failLabelHeight = failLabelRect.Height();
		int failLabelOffsetX = (embedControlRect.Width() - failLabelWidth) / 2;
		int failLabelOffsetY = (embedControlRect.Height() - failLabelHeight) / 2;
		failLabelRect.SetLeft(embedControlRect.Left() + failLabelOffsetX);
		failLabelRect.SetTop(embedControlRect.Top() + failLabelOffsetY);
		failLabelRect.SetRight(failLabelRect.Left() + failLabelWidth);
		failLabelRect.SetBottom(failLabelRect.Top() + failLabelHeight);
		m_failLabel.SetSizePosition(failLabelRect);
		m_failLabel.Show(SW_SHOW);
	}
}

void CAppearanceViewerActorViewPane::CreateActorViewer()
{
	auto placeholder = Framework::Win32::CStatic(GetItem(IDC_APPEARANCEVIEWER_RENDER_PLACEHOLDER));
	auto placeholderRect = placeholder.GetWindowRect();
	placeholderRect.ScreenToClient(m_hWnd);

	TCHAR moduleFileName[_MAX_PATH];
	GetModuleFileName(NULL, moduleFileName, _MAX_PATH);
	boost::filesystem::path actorViewerDataPath(moduleFileName);
	actorViewerDataPath.remove_leaf();
	actorViewerDataPath /= "actorviewer";

	m_embedControl = std::make_shared<Palleon::CWin32EmbedControl>(m_hWnd, placeholderRect,
		_T("ActorViewer.exe"), actorViewerDataPath.native().c_str());
	
	{
		Palleon::CEmbedRemoteCall rpc;
		rpc.SetMethod("SetGamePath");
		rpc.SetParam("Path", CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));
		m_embedControl->ExecuteCommand(rpc.ToString());
	}
}
