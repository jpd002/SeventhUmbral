#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "ActorViewer.h"
#include "../renderobjects/GlobalResources.h"
#include "../dataobjects/FileManager.h"

#define MAIN_CAMERA_FOV		(M_PI / 4.f)
#define MAIN_CAMERA_NEAR_Z	(0.01f)
#define MAIN_CAMERA_FAR_Z	(100.f)

CActorViewer::CActorViewer(bool isEmbedding)
{
	CGlobalResources::GetInstance().Initialize();

	CreateScene();
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());

	if(!isEmbedding)
	{
		m_debugOverlay = std::make_shared<CDebugOverlay>();
	}
}

CActorViewer::~CActorViewer()
{
	m_debugOverlay.reset();
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_mainViewport.get());
	CGlobalResources::GetInstance().Release();
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

	if(m_debugOverlay)
	{
		m_debugOverlay->Update(dt);
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
	if(m_actor)
	{
		auto boundingSphere = m_actor->GetBoundingSphere();
		float zoomLevel = boundingSphere.radius * 2;
		float zoomLevelFactor = (m_cameraZoomDelta / 1024.f) + 1.10f;
		m_mainCamera->LookAt(CVector3(0, 0, zoomLevel * zoomLevelFactor), CVector3(0, 0, 0),
			CVector3(0, 1, 0), Palleon::HANDEDNESS_RIGHTHANDED);
	}
}

void CActorViewer::NotifySizeChanged()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();
	m_mainCamera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, 
		MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z, Palleon::HANDEDNESS_RIGHTHANDED);
	if(m_debugOverlay)
	{
		m_debugOverlay->NotifySizeChanged();
	}
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
	m_commandMode = COMMAND_MODE_DRAG_CAMERA;
	m_dragHAngle = m_cameraHAngle;
	m_dragVAngle = m_cameraVAngle;
	m_dragPosition = m_mousePosition;
}

void CActorViewer::NotifyMouseUp()
{
	m_commandMode = COMMAND_MODE_IDLE;
}

std::string CActorViewer::NotifyExternalCommand(const std::string& command)
{
	Palleon::CEmbedRemoteCall rpc(command);
	auto method = rpc.GetMethod();
	if(method == "SetGamePath")
	{
		auto gamePath = rpc.GetParam("Path");
		CFileManager::SetGamePath(gamePath);
		return std::string("success");
	}
	if(method == "SetActor")
	{
		try
		{
			auto baseModelId = boost::lexical_cast<uint32>(rpc.GetParam("BaseModelId"));
			auto topModelId = boost::lexical_cast<uint32>(rpc.GetParam("TopModelId"));
			SetActor(baseModelId, topModelId);
			return std::string("success");
		}
		catch(...)
		{
			return std::string("failed");
		}
	}
	return std::string("failed");
}

Palleon::CApplication* CreateApplication(bool isEmbedding)
{
	return new CActorViewer(isEmbedding);
}
