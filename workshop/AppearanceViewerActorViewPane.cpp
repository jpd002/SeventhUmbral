#include "AppConfig.h"
#include "AppPreferences.h"
#include "AppearanceViewerActorViewPane.h"
#include "resource.h"
#include "palleon\EmbedRemoteCall.h"

CAppearanceViewerActorViewPane::CAppearanceViewerActorViewPane(HWND parentWnd)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER_ACTORVIEWPANE), parentWnd)
{
	SetClassPtr();
	m_failLabel = Framework::Win32::CStatic(GetItem(IDC_APPEARANCEVIEWER_FAILLABEL));
	CreateActorViewer();
}

CAppearanceViewerActorViewPane::~CAppearanceViewerActorViewPane()
{

}

long CAppearanceViewerActorViewPane::OnSize(unsigned int, unsigned int, unsigned int)
{
	if(m_embedControl)
	{
		auto rect = GetClientRect();
		m_embedControl->SetSizePosition(rect);
	}
	{
		auto failLabelRect = m_failLabel.GetWindowRect();
		auto embedControlRect = GetWindowRect();
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
	}
	return FALSE;
}

void CAppearanceViewerActorViewPane::SetActive(bool active)
{
	m_embedControl->SetRunning(active);
}

void CAppearanceViewerActorViewPane::SetActor(uint32 baseModelId, uint32 topModelId)
{
	m_failLabel.Show(SW_HIDE);
	if(!m_embedControl->IsClientActive())
	{
		m_embedControl->Destroy();
		m_embedControl.reset();
		CreateActorViewer();
	}
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetMethod("SetActor");
	rpc.SetParam("BaseModelId", std::to_string(baseModelId));
	rpc.SetParam("TopModelId", std::to_string(topModelId));
	auto result = m_embedControl->ExecuteCommand(rpc.ToString());
	if(result != "success")
	{
		m_failLabel.SetText(_T("Failed to load item."));
		m_failLabel.Show(SW_SHOW);
	}
}

void CAppearanceViewerActorViewPane::CreateActorViewer()
{
	assert(!m_embedControl);

	auto rect = GetClientRect();

	TCHAR moduleFileName[_MAX_PATH];
	GetModuleFileName(NULL, moduleFileName, _MAX_PATH);
	boost::filesystem::path actorViewerDataPath(moduleFileName);
	actorViewerDataPath.remove_leaf();
	actorViewerDataPath /= "actorviewer";

	m_embedControl = std::make_shared<Palleon::CWin32EmbedControl>(m_hWnd, rect,
		_T("ActorViewer.exe"), actorViewerDataPath.native().c_str());
	m_embedControl->ErrorRaised.connect([&] (Palleon::CWin32EmbedControl* sender) { EmbedControl_OnErrorRaised(sender); });
	{
		Palleon::CEmbedRemoteCall rpc;
		rpc.SetMethod("SetGamePath");
		rpc.SetParam("Path", CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));
		m_embedControl->ExecuteCommand(rpc.ToString());
	}
}

void CAppearanceViewerActorViewPane::EmbedControl_OnErrorRaised(Palleon::CWin32EmbedControl*)
{
	m_failLabel.SetText(_T("Guest crashed, please select another item."));
	m_failLabel.Show(SW_SHOW);
}
