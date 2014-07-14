#include "string_format.h"
#include "DebugOverlay.h"

CDebugOverlay::CDebugOverlay()
{
	m_package = Palleon::CPackage::Create("global");

	CreateUi();
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_uiViewport.get());
}

CDebugOverlay::~CDebugOverlay()
{
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_uiViewport.get());
}

void CDebugOverlay::SetCameraPosition(const CVector3& cameraPosition)
{
	m_cameraPosition = cameraPosition;
}

void CDebugOverlay::Update(float dt)
{
	{
		auto positionText = string_format("Pos = (X: %0.2f, Y: %0.2f, Z: %0.2f)", m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z);
		m_positionLabel->SetText(positionText);
	}

	{
		auto metricsText = string_format("Draw Calls = %d - FPS = %d", 
			Palleon::CGraphicDevice::GetInstance().GetDrawCallCount(),
			static_cast<int>(Palleon::CGraphicDevice::GetInstance().GetFrameRate()));
		m_metricsLabel->SetText(metricsText);
	}

	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();
}

void CDebugOverlay::CreateUi()
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
