#include "ActorViewer.h"
#include "string_format.h"
#include "../renderobjects/UmbralActor.h"
#include "../renderobjects/GlobalResources.h"

static const CVector3 g_actorPosition(0, 0, -3);

CActorViewer::CActorViewer()
: m_mousePosition(0, 0)
, m_forwardButtonBoundingBox(0, 0, 0, 0)
, m_backwardButtonBoundingBox(0, 0, 0, 0)
{
	CGlobalResources::GetInstance().Initialize();
	m_package = Palleon::CPackage::Create("global");

	CreateActor();
	CreateUi();

	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_uiViewport.get());
}

CActorViewer::~CActorViewer()
{
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_uiViewport.get());
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

			{
				auto sprite = scene->FindNode<Palleon::CSprite>("BackwardSprite");
				m_backwardButtonBoundingBox.position = sprite->GetPosition().xy();
				m_backwardButtonBoundingBox.size = sprite->GetSize();
			}

			{
				auto sprite = scene->FindNode<Palleon::CSprite>("ForwardSprite");
				m_forwardButtonBoundingBox.position = sprite->GetPosition().xy();
				m_forwardButtonBoundingBox.size = sprite->GetSize();
			}

			sceneRoot->AppendChild(scene);
		}
	}
}

void CActorViewer::CreateActor()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	{
		auto camera = CTouchFreeCamera::Create();
		camera->SetPerspectiveProjection(M_PI / 4.f, screenSize.x / screenSize.y, 0.01f, 100.f, Palleon::HANDEDNESS_RIGHTHANDED);
		camera->SetPosition(CVector3(0, 0.5f, 0));
		m_mainCamera = camera;
	}

	{
		auto viewport = Palleon::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_mainViewport = viewport;
	}

	auto sceneRoot = m_mainViewport->GetSceneRoot();

	{
		auto actor = std::make_shared<CUmbralActor>();
		actor->SetBaseModelId(10006);
		actor->SetPosition(g_actorPosition);
		sceneRoot->AppendChild(actor);
	}

	UpdateLights();
}

void CActorViewer::Update(float dt)
{
	UpdateLights();

	{
		auto cameraPosition = m_mainCamera->GetPosition();
		auto positionText = string_format("Pos = (X: %0.2f, Y: %0.2f, Z: %0.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
		m_positionLabel->SetText(positionText);
	}

	{
		auto metricsText = string_format("Draw Calls = %d - FPS = %d", 
			Palleon::CGraphicDevice::GetInstance().GetDrawCallCount(),
			static_cast<int>(Palleon::CGraphicDevice::GetInstance().GetFrameRate()));
		m_metricsLabel->SetText(metricsText);
	}

	m_elapsed += dt;

	m_mainCamera->Update(dt);
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();
}

void CActorViewer::UpdateLights()
{
	m_mainViewport->SetEffectParameter("ps_ambientLightColor", Palleon::CEffectParameter(CVector4(0, 0, 0, 0)));
	m_mainViewport->SetEffectParameter("ps_dirLightDirection0", Palleon::CEffectParameter(CVector3(cos(m_elapsed), -1, sin(m_elapsed)).Normalize()));
	m_mainViewport->SetEffectParameter("ps_dirLightDirection1", Palleon::CEffectParameter(CVector3(-sin(m_elapsed), -1, cos(m_elapsed)).Normalize()));
	m_mainViewport->SetEffectParameter("ps_dirLightColor0", Palleon::CEffectParameter(CVector4(1.0f, 1.0f, 1.0f, 0)));
	m_mainViewport->SetEffectParameter("ps_dirLightColor1", Palleon::CEffectParameter(CVector4(1.0f, 1.0f, 1.0f, 0)));
}

void CActorViewer::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(x, y);
	m_mainCamera->NotifyMouseMove(x, y);
}

void CActorViewer::NotifyMouseDown()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_PRESSED);
	if(m_forwardButtonBoundingBox.Intersects(CBox2(m_mousePosition.x, m_mousePosition.y, 4, 4)))
	{
		m_mainCamera->NotifyMouseDown_MoveForward();
	}
	else if(m_backwardButtonBoundingBox.Intersects(CBox2(m_mousePosition.x, m_mousePosition.y, 4, 4)))
	{
		m_mainCamera->NotifyMouseDown_MoveBackward();
	}
	else
	{
		m_mainCamera->NotifyMouseDown_Center();
	}
}

void CActorViewer::NotifyMouseUp()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_RELEASED);
	m_mainCamera->NotifyMouseUp();
}

Palleon::CApplication* CreateApplication()
{
	return new CActorViewer();
}
