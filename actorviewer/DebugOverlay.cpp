#include "DebugOverlay.h"
#include "string_format.h"

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

void CDebugOverlay::Update(float dt)
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

void CDebugOverlay::NotifySizeChanged()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();
	m_uiViewport->GetCamera()->SetupOrthoCamera(screenSize.x, screenSize.y);
}

void CDebugOverlay::CreateUi()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	m_uiViewport = Palleon::CViewport::Create();

	{
		auto camera = Palleon::CCamera::Create();
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
