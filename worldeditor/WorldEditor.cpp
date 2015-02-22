#include "WorldEditor.h"
#include "StdStream.h"
#include "StdStreamUtils.h"
#include "../dataobjects/FileManager.h"
#include "../renderobjects/ResourceManager.h"
#include "../renderobjects/GlobalResources.h"
#include "../renderobjects/UmbralModel.h"
#include "EditorControlScheme.h"
#include "ViewerControlScheme.h"

//0x03E70001 -> Mor'dhona
//0x25B10001 -> Some boat
//0x25B10002 -> Boat insides
//0x28D90001 -> Coerthas
//0x29D90001 -> Limsa Lominsa
//0x29B00001 -> Gridania
//0x615A0001 -> Ul'dah
//0x615A0002 -> Some Thanalan cavern?
//0x615A0003 -> Ul'dah indoors
//0x615A0004 -> Thanalan (part 1?)
//0x615A0005 -> Thanalan (part 2?)
//0xA09B0000 -> Gridania Inn Room
//0xA09B0001 -> Limsa Lominsa Inn Room
//0xA09B0002 -> Ul'dah Inn Room
//0xABF40000 -> Rivenroad
//0x8B380000 -> La Noscea (boats and towers)
//0x8B380001 -> Airship
//0x92050003 -> Some island
//0x92050004 -> Some beach

#define MAIN_CAMERA_FOV		(M_PI / 4.f)
#define MAIN_CAMERA_NEAR_Z	(0.01f)
#define MAIN_CAMERA_FAR_Z	(100.f)

CWorldEditor::CWorldEditor(bool isEmbedding)
: m_isEmbedding(isEmbedding)
{
	CGlobalResources::GetInstance().Initialize();

	CreateWorld();

	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_overlayViewport.get());

	//m_controlScheme = std::make_shared<CViewerControlScheme>(*this);
	m_controlScheme = std::make_shared<CEditorControlScheme>(*this);

	if(!isEmbedding)
	{
		m_debugOverlay = std::make_shared<CDebugOverlay>();
		CFileManager::SetGamePath("C:\\Program Files (x86)\\SquareEnix\\FINAL FANTASY XIV");
//		CreateMap(0xA09B0000);
//		CreateMap(0x29B00003);
		CreateActor(1);
		SetActorBaseModelId(1, 1);
//		SetActorTopModelId(1, 0x828);
//		SetActorPosition(1, CVector3(0, 0, -2));
//		m_mainCamera->SetPosition(CVector3(297.f, 15.f, -551.f));
	}
}

CWorldEditor::~CWorldEditor()
{
	if(m_map)
	{
		m_map->CancelLoading();
	}
	m_debugOverlay.reset();
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_overlayViewport.get());
	CGlobalResources::GetInstance().Release();
}

bool CWorldEditor::GetIsEmbedding() const
{
	return m_isEmbedding;
}

TouchFreeCameraPtr CWorldEditor::GetMainCamera() const
{
	return m_mainCamera;
}

TranslationGizmoPtr CWorldEditor::GetTranslationGizmo() const
{
	return m_translationGizmo;
}

Palleon::SceneNodePtr CWorldEditor::GetActorRotationNode() const
{
	return m_actorRotationNode;
}

UmbralActorPtr CWorldEditor::GetActor(uint32 actorId) const
{
	auto actorIterator = m_actors.find(actorId);
	assert(actorIterator != std::end(m_actors));
	auto actor = actorIterator->second;
	return actor;
}

const CWorldEditor::ActorMap& CWorldEditor::GetActors() const
{
	return m_actors;
}

void CWorldEditor::CreateWorld()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	{
		auto camera = CTouchFreeCamera::Create();
		camera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z, Palleon::HANDEDNESS_RIGHTHANDED);
		m_mainCamera = camera;
	}

	{
		auto viewport = Palleon::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_mainViewport = viewport;
	}

	{
		auto lightDir0 = CVector3(0, -1, 0).Normalize();
		auto lightDir1 = CVector3(1, -1, 0).Normalize();

		m_mainViewport->SetEffectParameter("ps_ambientLightColor", Palleon::CEffectParameter(CVector4(0, 0, 0, 0)));
		m_mainViewport->SetEffectParameter("ps_dirLightDirection0", Palleon::CEffectParameter(lightDir0));
		m_mainViewport->SetEffectParameter("ps_dirLightDirection1", Palleon::CEffectParameter(lightDir1));
		m_mainViewport->SetEffectParameter("ps_dirLightColor0", Palleon::CEffectParameter(CVector4(1.00f, 1.00f, 1.00f, 0)));
		m_mainViewport->SetEffectParameter("ps_dirLightColor1", Palleon::CEffectParameter(CVector4(0.20f, 0.20f, 0.20f, 0)));
	}

	//Create skybox
	{
		auto skyTexture = Palleon::CTextureLoader::CreateCubeTextureFromFile("./data/global/skybox.dds");
		auto skyBox = Palleon::CCubeMesh::Create();
		skyBox->SetIsPeggedToOrigin(true);
		skyBox->SetScale(CVector3(50, 50, 50));
		skyBox->GetMaterial()->SetCullingMode(Palleon::CULLING_CCW);
		skyBox->GetMaterial()->SetTexture(0, skyTexture);
		skyBox->GetMaterial()->SetTextureCoordSource(0, Palleon::TEXTURE_COORD_CUBE_POS);
		m_mainViewport->GetSceneRoot()->AppendChild(skyBox);
	}

	{
		auto viewport = Palleon::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_overlayViewport = viewport;
	}

	{
		auto gizmo = CTranslationGizmo::Create();
		gizmo->SetVisible(false);
		m_overlayViewport->GetSceneRoot()->AppendChild(gizmo);
		m_translationGizmo = gizmo;
	}

	{
		auto node = Palleon::CSceneNode::Create();
		m_mainViewport->GetSceneRoot()->AppendChild(node);
		m_actorRotationNode = node;
	}
}

void CWorldEditor::CreateMap(uint32 mapId)
{
	auto sceneRoot = m_mainViewport->GetSceneRoot();

	{
		auto mapLayoutPath = CFileManager::GetResourcePath(mapId);
		auto mapLayout = std::make_shared<CMapLayout>();
		auto mapStream = Framework::CreateInputStdStream(mapLayoutPath.native());
		mapLayout->Read(mapStream);

		auto map = std::make_shared<CUmbralMap>(mapLayout);
		sceneRoot->AppendChild(map);
		m_map = map;
	}
}

void CWorldEditor::CreateActor(uint32 id)
{
	assert(m_actors.find(id) == m_actors.end());

	auto sceneRoot = m_mainViewport->GetSceneRoot();
	
	{
		auto actor = std::make_shared<CUmbralActor>();
		sceneRoot->AppendChild(actor);
		m_actors.insert(std::make_pair(id, actor));
	}

	if(m_controlScheme)
	{
		m_controlScheme->NotifyActorChanged(id);
	}
}

void CWorldEditor::SetActorPosition(uint32 id, const CVector3& position)
{
	auto actorIterator = m_actors.find(id);
	assert(actorIterator != std::end(m_actors));
	if(actorIterator == std::end(m_actors))
	{
		return;
	}
	actorIterator->second->SetPosition(position);
}

void CWorldEditor::SetActorBaseModelId(uint32 id, uint32 baseModelId)
{
	auto actorIterator = m_actors.find(id);
	assert(actorIterator != std::end(m_actors));
	if(actorIterator == std::end(m_actors))
	{
		throw std::runtime_error("Actor doesn't exist");
	}
	actorIterator->second->SetBaseModelId(baseModelId);

	if(m_controlScheme)
	{
		m_controlScheme->NotifyActorChanged(id);
	}
}

void CWorldEditor::SetActorTopModelId(uint32 id, uint32 topModelId)
{
	auto actorIterator = m_actors.find(id);
	assert(actorIterator != std::end(m_actors));
	if(actorIterator == std::end(m_actors))
	{
		throw std::runtime_error("Actor doesn't exist");
	}
	actorIterator->second->SetTopModelId(topModelId);

	if(m_controlScheme)
	{
		m_controlScheme->NotifyActorChanged(id);
	}
}

void CWorldEditor::Update(float dt)
{
	if(m_controlScheme)
	{
		m_controlScheme->PreUpdate(dt);
	}

	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_overlayViewport->GetSceneRoot()->Update(dt);
	m_overlayViewport->GetSceneRoot()->UpdateTransformations();

	if(m_controlScheme)
	{
		m_controlScheme->PostUpdate(dt);
	}

	if(m_debugOverlay)
	{
		m_debugOverlay->SetCameraPosition(m_mainCamera->GetPosition());
		m_debugOverlay->Update(dt);
	}
}

void CWorldEditor::NotifySizeChanged()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();
	m_mainCamera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, 
		MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z, Palleon::HANDEDNESS_RIGHTHANDED);
}

void CWorldEditor::NotifyMouseMove(int x, int y)
{
	if(!m_controlScheme) return;
	m_controlScheme->NotifyMouseMove(x, y);
}

void CWorldEditor::NotifyMouseDown()
{
	if(!m_controlScheme) return;
	m_controlScheme->NotifyMouseDown();
}

void CWorldEditor::NotifyMouseUp()
{
	if(!m_controlScheme) return;
	m_controlScheme->NotifyMouseUp();
}

void CWorldEditor::NotifyMouseWheel(int delta)
{
	if(!m_controlScheme) return;
	m_controlScheme->NotifyMouseWheel(delta);
}

void CWorldEditor::NotifyKeyDown(Palleon::KEY_CODE keyCode)
{
	if(!m_controlScheme) return;
	m_controlScheme->NotifyKeyDown(keyCode);
}

void CWorldEditor::NotifyKeyUp(Palleon::KEY_CODE keyCode)
{
	if(!m_controlScheme) return;
	m_controlScheme->NotifyKeyUp(keyCode);
}

void CWorldEditor::NotifyInputCancelled()
{
	if(!m_controlScheme) return;
	m_mainCamera->CancelInputTracking();
}

Palleon::CApplication* CreateApplication(bool isEmbedding)
{
	return new CWorldEditor(isEmbedding);
}
