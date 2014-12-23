#include "ZoneEditor.h"
#include "resource.h"
#include "win32/HorizontalSplitter.h"
#include "math/MathStringUtils.h"
#include "StdStreamUtils.h"
#include "palleon/EmbedRemoteCall.h"
#include "../dataobjects/server/ZoneDefinition.h"
#include "ActorPropertyBag.h"

#define TEST_FILE_PATH ("C:\\Projects\\SeventhUmbral\\test.xml")

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

	LoadZoneDefinition();
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

bool CZoneEditor::GetDirty() const
{
	return m_documentDirty;
}

void CZoneEditor::SetDirty(bool dirty)
{
	m_documentDirty = dirty;
	StateChanged();
}

void CZoneEditor::Save()
{
	if(m_documentDirty)
	{
		SaveZoneDefinition();
	}
}

long CZoneEditor::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto rect = GetClientRect();
	m_mainSplitter->SetSizePosition(rect);
	return FALSE;
}

void CZoneEditor::LoadZoneDefinition()
{
	auto inputStream = Framework::CreateInputStdStream(std::string(TEST_FILE_PATH));
	CZoneDefinition zone(inputStream);

	for(const auto& actor : zone.GetActors())
	{
		ACTOR editorActor;
		editorActor.id = actor.id;
		editorActor.baseModelId = actor.baseModelId;
		editorActor.position = actor.position;
		editorActor.nameStringId = actor.nameStringId;
		m_actors.insert(std::make_pair(actor.id, editorActor));
	}

	for(const auto& actorPair : m_actors)
	{
		const auto& actor(actorPair.second);
		uint32 actorId = actorPair.first;
		m_worldPane->CreateActor(actorId);
		m_worldPane->SetActorBaseModelId(actorId, actor.baseModelId);
		m_worldPane->SetActorPosition(actorId, actor.position);
	}
}

void CZoneEditor::SaveZoneDefinition()
{
	CZoneDefinition zone;

	for(const auto& actorPair : m_actors)
	{
		const auto& editorActor(actorPair.second);
		uint32 actorId = actorPair.first;
		ACTOR_DEFINITION actor;
		actor.id = actorId;
		actor.baseModelId = editorActor.baseModelId;
		actor.position = editorActor.position;
		zone.AddActor(actor);
	}

	auto outputStream = Framework::CreateOutputStdStream(std::string(TEST_FILE_PATH));
	zone.Save(outputStream);

	m_documentDirty = false;
	StateChanged();
}

void CZoneEditor::PropertyChanged(unsigned int propertyIndex)
{
	if(m_selectedActorId == 0)
	{
		return;
	}
	assert(m_actors.find(m_selectedActorId) != m_actors.end());
	auto& actor = m_actors[m_selectedActorId];
	switch(propertyIndex)
	{
	case CActorPropertyBag::ACTOR_PROPERTY_BASEMODELID:
		m_worldPane->SetActorBaseModelId(m_selectedActorId, actor.baseModelId);
		SetDirty(true);
		break;
	}
}

void CZoneEditor::WorldNotificationRaised(const std::string& command)
{
	Palleon::CEmbedRemoteCall rpc(command);
	auto method = rpc.GetMethod();
	if(method == "selected")
	{
		uint32 actorId = atoi(rpc.GetParam("actorId").c_str());
		m_selectedActorId = actorId;
		if(actorId != 0)
		{
			auto actorIterator = m_actors.find(actorId);
			assert(actorIterator != m_actors.end());
			if(actorIterator != m_actors.end())
			{
				const auto& actor = actorIterator->second;
				auto propertyBag = std::make_shared<CActorPropertyBag>(actorIterator->second);
				propertyBag->PropertyChanged.connect(
					[&] (unsigned int index)
					{
						PropertyChanged(index);
					}
				);
				m_propertiesPane->SetSelection(propertyBag);
			}
		}
		else
		{
			m_propertiesPane->SetSelection(Framework::Win32::PropertyBagPtr());
		}
	}
	else if(method == "actorMoved")
	{
		uint32 actorId = atoi(rpc.GetParam("actorId").c_str());
		auto position = MathStringUtils::ParseVector3(rpc.GetParam("position"));
		auto actorIterator = m_actors.find(actorId);
		assert(actorIterator != m_actors.end());
		if(actorIterator != m_actors.end())
		{
			actorIterator->second.position = position;
			m_propertiesPane->Refresh();
			SetDirty(true);
		}
	}
}
