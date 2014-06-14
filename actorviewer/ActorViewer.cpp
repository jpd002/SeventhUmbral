#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "ActorViewer.h"
#include "string_format.h"
#include "../renderobjects/GlobalResources.h"
#include "../dataobjects/FileManager.h"

#include "Utf8.h"

#define MAIN_CAMERA_FOV		(M_PI / 4.f)
#define MAIN_CAMERA_NEAR_Z	(0.01f)
#define MAIN_CAMERA_FAR_Z	(100.f)

CActorViewer::CActorViewer()
{
	CGlobalResources::GetInstance().Initialize();

	CreateScene();
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	
	if(!m_isEmbedding)
	{
		m_package = Palleon::CPackage::Create("global");
		CreateUi();
		Palleon::CGraphicDevice::GetInstance().AddViewport(m_uiViewport.get());
	}
}

CActorViewer::~CActorViewer()
{
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_mainViewport.get());
	if(!m_isEmbedding)
	{
		Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_uiViewport.get());
	}
	CGlobalResources::GetInstance().Release();
}

void CActorViewer::CreateUi()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	m_uiViewport = Palleon::CViewport::Create();

	{
		Palleon::CameraPtr camera = Palleon::CCamera::Create();
		camera->SetupOrthoCamera(screenSize.x, screenSize.y);
		m_uiViewport->SetCamera(camera);
	}

	{
		auto sceneRoot = m_uiViewport->GetSceneRoot();

		{
			auto scene = Palleon::CScene::Create(Palleon::CResourceManager::GetInstance().GetResource<Palleon::CSceneDescriptor>("main_scene.xml"));

			m_positionLabel = scene->FindNode<Palleon::CLabel>("PositionLabel");
			m_metricsLabel = scene->FindNode<Palleon::CLabel>("MetricsLabel");

			sceneRoot->AppendChild(scene);
		}
	}
}

void CActorViewer::CreateScene()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	{
		auto camera = Palleon::CCamera::Create();
		camera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, 
			MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z, Palleon::HANDEDNESS_RIGHTHANDED);
		m_mainCamera = camera;
	}

	{
		auto viewport = Palleon::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_mainViewport = viewport;
	}

	UpdateLights();
}

void CActorViewer::SetActor(uint32 baseModelId, uint32 topModelId)
{
	auto sceneRoot = m_mainViewport->GetSceneRoot();

	if(m_actor)
	{
		m_actorRotationNode->RemoveChild(m_actor);
		m_actor.reset();
	}

	if(m_actorRotationNode)
	{
		sceneRoot->RemoveChild(m_actorRotationNode);
		m_actorRotationNode.reset();
	}

	m_cameraHAngle = 0;
	m_cameraVAngle = 0;
	m_cameraZoomDelta = 0;

	{
		auto node = Palleon::CSceneNode::Create();
		sceneRoot->AppendChild(node);
		m_actorRotationNode = node;
	}

	{
		auto actor = std::make_shared<CUmbralActor>();
		actor->SetBaseModelId(baseModelId);
		actor->SetTopModelId(topModelId);
		actor->RebuildActorRenderables();
		m_actorRotationNode->AppendChild(actor);
		m_actor = actor;
	}

	{
		auto boundingSphere = m_actor->GetBoundingSphere();
		m_actor->SetPosition(boundingSphere.position * -1.0f);
	}

	UpdateCameraLookAt();
}

void CActorViewer::Update(float dt)
{
	{
		if(m_commandMode == COMMAND_MODE_DRAG_CAMERA)
		{
			float deltaX = m_dragPosition.x - m_mousePosition.x;
			float deltaY = m_dragPosition.y - m_mousePosition.y;
			m_cameraHAngle = m_dragHAngle + deltaX * 0.015f;
			m_cameraVAngle = m_dragVAngle + deltaY * 0.015f;
			m_cameraVAngle = std::min<float>(m_cameraVAngle, M_PI / 2);
			m_cameraVAngle = std::max<float>(m_cameraVAngle, -M_PI / 2);
		}

		CMatrix4 yawMatrix(CMatrix4::MakeAxisYRotation(m_cameraHAngle));
		CMatrix4 pitchMatrix(CMatrix4::MakeAxisXRotation(m_cameraVAngle));

		CMatrix4 rotationMatrix = yawMatrix * pitchMatrix;
		if(m_actorRotationNode)
		{
			m_actorRotationNode->SetRotation(rotationMatrix);
		}
	}
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();

	if(!m_isEmbedding)
	{
		{
			auto metricsText = string_format("Draw Calls = %d - FPS = %d", 
				Palleon::CGraphicDevice::GetInstance().GetDrawCallCount(),
				static_cast<int>(Palleon::CGraphicDevice::GetInstance().GetFrameRate()));
			m_metricsLabel->SetText(metricsText);
		}
		m_uiViewport->GetSceneRoot()->Update(dt);
		m_uiViewport->GetSceneRoot()->UpdateTransformations();
	}

	m_elapsed += dt;

}

void CActorViewer::UpdateLights()
{
//	auto lightDir0 = CVector3(cos(m_elapsed), -1, sin(m_elapsed)).Normalize();
//	auto lightDir1 = CVector3(-sin(m_elapsed), -1, cos(m_elapsed)).Normalize();
	auto lightDir0 = CVector3(1, -1, 0).Normalize();
	auto lightDir1 = CVector3(0, 0, 0);

	m_mainViewport->SetEffectParameter("ps_ambientLightColor", Palleon::CEffectParameter(CVector4(0, 0, 0, 0)));
	m_mainViewport->SetEffectParameter("ps_dirLightDirection0", Palleon::CEffectParameter(lightDir0));
	m_mainViewport->SetEffectParameter("ps_dirLightDirection1", Palleon::CEffectParameter(lightDir1));
	m_mainViewport->SetEffectParameter("ps_dirLightColor0", Palleon::CEffectParameter(CVector4(1.0f, 1.0f, 1.0f, 0)));
	m_mainViewport->SetEffectParameter("ps_dirLightColor1", Palleon::CEffectParameter(CVector4(1.0f, 1.0f, 1.0f, 0)));
}

void CActorViewer::UpdateCameraLookAt()
{
	auto boundingSphere = m_actor->GetBoundingSphere();
	float zoomLevel = boundingSphere.radius * 2;
	float zoomLevelFactor = (m_cameraZoomDelta / 1024.f) + 1.10f;
	m_mainCamera->LookAt(CVector3(0, 0, zoomLevel * zoomLevelFactor), CVector3(0, 0, 0),
		CVector3(0, 1, 0), Palleon::HANDEDNESS_RIGHTHANDED);
}

void CActorViewer::NotifySizeChanged()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();
	m_mainCamera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, 
		MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z, Palleon::HANDEDNESS_RIGHTHANDED);
	m_uiViewport->GetCamera()->SetupOrthoCamera(screenSize.x, screenSize.y);
}

void CActorViewer::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(x, y);
}

void CActorViewer::NotifyMouseWheel(int delta)
{
	m_cameraZoomDelta -= static_cast<float>(delta);
	m_cameraZoomDelta = std::min(m_cameraZoomDelta,  1024.f);
	m_cameraZoomDelta = std::max(m_cameraZoomDelta, -1024.f);
	UpdateCameraLookAt();
}

void CActorViewer::NotifyMouseDown()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_PRESSED);

	m_commandMode = COMMAND_MODE_DRAG_CAMERA;
	m_dragHAngle = m_cameraHAngle;
	m_dragVAngle = m_cameraVAngle;
	m_dragPosition = m_mousePosition;
}

void CActorViewer::NotifyMouseUp()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_RELEASED);

	m_commandMode = COMMAND_MODE_IDLE;
}

void CActorViewer::NotifyIsEmbedding()
{
	m_isEmbedding = true;
}

void CActorViewer::NotifyExternalCommand(const std::string& command)
{
	Palleon::CEmbedRemoteCall rpc(command);
	auto method = rpc.GetMethod();
	if(method == "SetGamePath")
	{
		auto gamePath = rpc.GetParam("Path");
		CFileManager::SetGamePath(gamePath);
	}
	if(method == "SetActor")
	{
		auto baseModelId = boost::lexical_cast<uint32>(rpc.GetParam("BaseModelId"));
		auto topModelId = boost::lexical_cast<uint32>(rpc.GetParam("TopModelId"));
		SetActor(baseModelId, topModelId);
	}
}

Palleon::CApplication* CreateApplication()
{
	return new CActorViewer();
}
