#include "ZoneEditor.h"
#include "resource.h"
#include "win32/HorizontalSplitter.h"
#include "palleon/EmbedRemoteCall.h"

CZoneEditor::CZoneEditor(HWND parentWnd, uint32 mapId)
: CDialog(MAKEINTRESOURCE(IDD_ZONEEDITOR), parentWnd)
{
	SetClassPtr();

	m_mainSplitter = std::make_unique<Framework::Win32::CHorizontalSplitter>(m_hWnd, GetClientRect());
	m_propertiesPane = std::make_unique<CZoneEditorPropertiesPane>(m_mainSplitter->m_hWnd);
	m_worldPane = std::make_unique<CZoneEditorWorldPane>(m_mainSplitter->m_hWnd, mapId);

	m_worldPane->NotificationRaised.connect([&] (const std::string& command) { WorldNotificationRaised(command); } );

	m_mainSplitter->SetChild(0, m_worldPane->m_hWnd);
	m_mainSplitter->SetChild(1, m_propertiesPane->m_hWnd);
	m_mainSplitter->SetMasterChild(1);
	m_mainSplitter->SetEdgePosition(0.50);

	//TODO: Load from def file
	{
		ACTOR actor;
		actor.baseModelId = 10005;
		m_actors.insert(std::make_pair(109100, actor));
	}

	{
		ACTOR actor;
		actor.baseModelId = 10029;
		m_actors.insert(std::make_pair(109101, actor));
	}

	for(const auto& actorPair : m_actors)
	{
		const auto& actor(actorPair.second);
		m_worldPane->CreateActor(actorPair.first, actor.baseModelId);
	}
}

CZoneEditor::~CZoneEditor()
{

}

std::string CZoneEditor::GetName() const
{
	return "Zone Editor";
}

void CZoneEditor::SetActive(bool active)
{
	m_worldPane->SetActive(active);
}

long CZoneEditor::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto rect = GetClientRect();
	m_mainSplitter->SetSizePosition(rect);
	return FALSE;
}

void CZoneEditor::WorldNotificationRaised(const std::string& command)
{
	Palleon::CEmbedRemoteCall rpc(command);
	auto method = rpc.GetMethod();
	if(method == "selected")
	{
		uint32 actorId = atoi(rpc.GetParam("actorId").c_str());
		auto actorIterator = m_actors.find(actorId);
		assert(actorIterator != m_actors.end());
		if(actorIterator != m_actors.end())
		{
			const auto& actor = actorIterator->second;
			m_propertiesPane->SetSelection(actorId, actor.baseModelId);
		}
	}
}
