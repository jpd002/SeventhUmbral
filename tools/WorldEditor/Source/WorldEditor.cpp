#include "WorldEditor.h"
#include "StdStream.h"
#include "StdStreamUtils.h"
#include "SectionLoader.h"
#include "Rendering/UmbralModel.h"
#include "FileManager.h"
#include "ResourceDefs.h"
#include "ResourceManager.h"
#include "Rendering/GlobalResources.h"
#include "string_format.h"

//0x03E70001 -> Mor'dhona
//0x25B10001 -> Some boat
//0x25B10002 -> Boat insides
//0x28D90001 -> Coerthas
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

CWorldEditor::CWorldEditor()
: m_elapsed(0)
, m_mousePosition(0, 0)
, m_forwardButtonBoundingBox(0, 0, 0, 0)
, m_backwardButtonBoundingBox(0, 0, 0, 0)
{
	CGlobalResources::GetInstance().Initialize();
	m_package = Athena::CPackage::Create("global");

	CreateUi();
	CreateWorld();

	Athena::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	Athena::CGraphicDevice::GetInstance().AddViewport(m_uiViewport.get());
}

CWorldEditor::~CWorldEditor()
{
	Athena::CGraphicDevice::GetInstance().RemoveViewport(m_mainViewport.get());
	Athena::CGraphicDevice::GetInstance().RemoveViewport(m_uiViewport.get());
	CGlobalResources::GetInstance().Release();
}

void CWorldEditor::CreateUi()
{
	auto screenSize = Athena::CGraphicDevice::GetInstance().GetScreenSize();

	m_uiViewport = Athena::CViewport::Create();

	{
		Athena::CameraPtr camera = Athena::CCamera::Create();
		camera->SetupOrthoCamera(screenSize.x, screenSize.y);
		m_uiViewport->SetCamera(camera);
	}

	{
		auto sceneRoot = m_uiViewport->GetSceneRoot();

		{
			auto scene = Athena::CScene::Create(Athena::CResourceManager::GetInstance().GetResource<Athena::CSceneDescriptor>("main_scene.xml"));

			m_positionLabel = scene->FindNode<Athena::CLabel>("PositionLabel");
			m_metricsLabel = scene->FindNode<Athena::CLabel>("MetricsLabel");

			{
				auto sprite = scene->FindNode<Athena::CSprite>("BackwardSprite");
				m_backwardButtonBoundingBox.position = sprite->GetPosition().xy();
				m_backwardButtonBoundingBox.size = sprite->GetSize();
			}

			{
				auto sprite = scene->FindNode<Athena::CSprite>("ForwardSprite");
				m_forwardButtonBoundingBox.position = sprite->GetPosition().xy();
				m_forwardButtonBoundingBox.size = sprite->GetSize();
			}

			sceneRoot->AppendChild(scene);
		}
	}
}

void CWorldEditor::CreateWorld()
{
	auto screenSize = Athena::CGraphicDevice::GetInstance().GetScreenSize();

	{
		auto camera = CTouchFreeCamera::Create();
		camera->SetPerspectiveProjection(M_PI / 4.f, screenSize.x / screenSize.y, 1.f, 500.f, Athena::HANDEDNESS_RIGHTHANDED);
		m_mainCamera = camera;
	}

	{
		auto viewport = Athena::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_mainViewport = viewport;
	}

	auto sceneRoot = m_mainViewport->GetSceneRoot();

	{
		auto mapLayoutPath = CFileManager::GetResourcePath(0xA09B0000);
		auto mapLayout = std::make_shared<CMapLayout>();
		mapLayout->Read(Framework::CreateInputStdStream(mapLayoutPath.native()));

		auto map = std::make_shared<CUmbralMap>(mapLayout);
		sceneRoot->AppendChild(map);
	}

//	{
//		auto mapLayoutPath = CFileManager::GetResourcePath(0x29B00002);
//		auto mapLayout = std::make_shared<CMapLayout>();
//		mapLayout->Read(Framework::CreateInputStdStream(mapLayoutPath.native()));
//
//		auto map = std::make_shared<CUmbralMap>(mapLayout);
//		sceneRoot->AppendChild(map);
//	}
}

void CWorldEditor::Update(float dt)
{
	{
		auto cameraPosition = m_mainCamera->GetPosition();
		auto positionText = string_format("Pos = (X: %0.2f, Y: %0.2f, Z: %0.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
		m_positionLabel->SetText(positionText);
	}

	{
		auto metricsText = string_format("Draw Calls = %d - FPS = %d", 
			Athena::CGraphicDevice::GetInstance().GetDrawCallCount(),
			static_cast<int>(Athena::CGraphicDevice::GetInstance().GetFrameRate()));
		m_metricsLabel->SetText(metricsText);
	}

	m_mainCamera->Update(dt);
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();
}

void CWorldEditor::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(x, y);
	m_mainCamera->NotifyMouseMove(x, y);
}

void CWorldEditor::NotifyMouseDown()
{
	Athena::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Athena::INPUT_EVENT_PRESSED);
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

void CWorldEditor::NotifyMouseUp()
{
	Athena::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Athena::INPUT_EVENT_RELEASED);
	m_mainCamera->NotifyMouseUp();
}

//#define _SCAN_LAYOUTS

#ifdef _SCAN_LAYOUTS

#include <Windows.h>
#include "string_format.h"

bool IsFileMapLayout(const boost::filesystem::path& filePath)
{
	auto inputStream = Framework::CreateInputStdStream(filePath.string());

	uint8 header[0x10];
	inputStream.Read(header, 0x10);
	if(header[0] == 0x4D && header[1] == 0x61 && header[2] == 0x70 && header[3] == 0x4C)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AnalyseDirectory(const boost::filesystem::path& directoryPath)
{
	for(boost::filesystem::directory_iterator pathIterator(directoryPath); 
		pathIterator != boost::filesystem::directory_iterator(); pathIterator++)
	{
		const auto& path = *pathIterator;
		if(boost::filesystem::is_directory(path))
		{
			AnalyseDirectory(path);
			continue;
		}
		bool result = IsFileMapLayout(path);
		if(result)
		{
			OutputDebugStringA(string_format("Found MapLayout File: '%s'...\r\n", path.path().string().c_str()).c_str());
		}
	}
}

#endif

Athena::CApplication* CreateApplication()
{
#ifdef _SCAN_LAYOUTS
	AnalyseDirectory(CFileManager::GetGamePath());
#endif
	return new CWorldEditor();
}
