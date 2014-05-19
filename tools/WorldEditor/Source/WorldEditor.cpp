#include "WorldEditor.h"
#include "StdStream.h"
#include "StdStreamUtils.h"
#include "../../../renderobjects/FileManager.h"
#include "../../../renderobjects/ResourceManager.h"
#include "../../../renderobjects/GlobalResources.h"
#include "../../../renderobjects/UmbralActor.h"
#include "../../../renderobjects/UmbralMap.h"
#include "../../../renderobjects/UmbralModel.h"
#include "ActorDatabase.h"
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
{
	CGlobalResources::GetInstance().Initialize();
	m_package = Palleon::CPackage::Create("global");

	CreateUi();
	CreateWorld();

	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_overlayViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_uiViewport.get());
}

CWorldEditor::~CWorldEditor()
{
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_uiViewport.get());
	CGlobalResources::GetInstance().Release();
}

void CWorldEditor::CreateUi()
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

void CWorldEditor::CreateWorld()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	{
		auto camera = CTouchFreeCamera::Create();
		camera->SetPerspectiveProjection(M_PI / 4.f, screenSize.x / screenSize.y, 1.f, 500.f, Palleon::HANDEDNESS_RIGHTHANDED);
		m_mainCamera = camera;
	}

	{
		auto viewport = Palleon::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_mainViewport = viewport;
	}

	{
		auto viewport = Palleon::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_overlayViewport = viewport;
	}

	CreateMap();
	CreateActors();
	CreateBaseAxis();
}

void CWorldEditor::CreateMap()
{
	auto sceneRoot = m_mainViewport->GetSceneRoot();

	//Create skybox
	{
		auto skyTexture = Palleon::CTextureLoader::CreateCubeTextureFromFile("./data/global/skybox.dds");
		auto skyBox = Palleon::CCubeMesh::Create();
		skyBox->SetIsPeggedToOrigin(true);
		skyBox->SetScale(CVector3(50, 50, 50));
		skyBox->GetMaterial()->SetCullingMode(Palleon::CULLING_CCW);
		skyBox->GetMaterial()->SetTexture(0, skyTexture);
		skyBox->GetMaterial()->SetTextureCoordSource(0, Palleon::TEXTURE_COORD_CUBE_POS);
		sceneRoot->AppendChild(skyBox);
	}

	{
		auto mapLayoutPath = CFileManager::GetResourcePath(0x29B00001);
		auto mapLayout = std::make_shared<CMapLayout>();
		auto mapStream = Framework::CreateInputStdStream(mapLayoutPath.native());
		mapLayout->Read(mapStream);

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

void CWorldEditor::CreateActors()
{
	//WIP
#if 0
	auto sceneRoot = m_mainViewport->GetSceneRoot();

	Framework::CStdStream stream("D:\\Projects\\SeventhUmbral\\data\\ffxivd_actors.xml", "rb");
	CActorDatabase actorDatabase = CActorDatabase::CreateFromXml(stream);

	for(const auto& actorDef : actorDatabase.GetActors())
	{
		auto actor = std::make_shared<CUmbralActor>();
		actor->SetBaseModelId(actorDef.baseModelId);
		actor->SetPosition(actorDef.position);
		sceneRoot->AppendChild(actor);
	}
#endif
}

void CWorldEditor::CreateBaseAxis()
{
	auto sceneRoot = m_overlayViewport->GetSceneRoot();

	static const CVector3 g_arrowScale(0.075f, 0.25f, 0.075f);
	
	{
		auto baseAxisNode = Palleon::CSceneNode::Create();
		baseAxisNode->SetPosition(CVector3(289.2f, 5.00f, -563.f));
		sceneRoot->AppendChild(baseAxisNode);

		{
			auto axisMesh = Palleon::CAxisMesh::Create();
			axisMesh->SetScale(CVector3(1, 1, 1));
			baseAxisNode->AppendChild(axisMesh);
		}

		//X arrow
		{
			auto coneMesh = Palleon::CConeMesh::Create();
			coneMesh->SetPosition(CVector3(1, 0, 0));
			coneMesh->SetRotation(CQuaternion(CVector3(0, 0, 1), M_PI / 2.f));
			coneMesh->SetScale(g_arrowScale);
			coneMesh->GetMaterial()->SetColor(CColor(1, 0, 0, 1));
			baseAxisNode->AppendChild(coneMesh);
		}

		//Y arrow
		{
			auto coneMesh = Palleon::CConeMesh::Create();
			coneMesh->SetPosition(CVector3(0, 1, 0));
			coneMesh->SetScale(g_arrowScale);
			coneMesh->GetMaterial()->SetColor(CColor(0, 1, 0, 1));
			baseAxisNode->AppendChild(coneMesh);
		}

		//Z arrow
		{
			auto coneMesh = Palleon::CConeMesh::Create();
			coneMesh->SetPosition(CVector3(0, 0, 1));
			coneMesh->SetRotation(CQuaternion(CVector3(1, 0, 0), -M_PI / 2.f));
			coneMesh->SetScale(g_arrowScale);
			coneMesh->GetMaterial()->SetColor(CColor(0, 0, 1, 1));
			baseAxisNode->AppendChild(coneMesh);
		}
	}
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
			Palleon::CGraphicDevice::GetInstance().GetDrawCallCount(),
			static_cast<int>(Palleon::CGraphicDevice::GetInstance().GetFrameRate()));
		m_metricsLabel->SetText(metricsText);
	}

	m_mainCamera->Update(dt);
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_overlayViewport->GetSceneRoot()->Update(dt);
	m_overlayViewport->GetSceneRoot()->UpdateTransformations();
	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();
}

void CWorldEditor::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(static_cast<float>(x), static_cast<float>(y));
	m_mainCamera->UpdateDrag(m_mousePosition);
}

void CWorldEditor::NotifyMouseDown()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_PRESSED);
	m_mainCamera->BeginDrag(m_mousePosition);
}

void CWorldEditor::NotifyMouseUp()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_RELEASED);
	m_mainCamera->EndDrag();
}

void CWorldEditor::NotifyKeyDown(Palleon::KEY_CODE keyCode)
{
	switch(keyCode)
	{
	case Palleon::KEY_CODE_W:
		m_mainCamera->BeginMoveForward();
		break;
	case Palleon::KEY_CODE_S:
		m_mainCamera->BeginMoveBackward();
		break;
	case Palleon::KEY_CODE_A:
		m_mainCamera->BeginStrafeLeft();
		break;
	case Palleon::KEY_CODE_D:
		m_mainCamera->BeginStrafeRight();
		break;
	}
	
}

void CWorldEditor::NotifyKeyUp(Palleon::KEY_CODE keyCode)
{
	switch(keyCode)
	{
	case Palleon::KEY_CODE_W:
		m_mainCamera->EndMoveForward();
		break;
	case Palleon::KEY_CODE_S:
		m_mainCamera->EndMoveBackward();
		break;
	case Palleon::KEY_CODE_A:
		m_mainCamera->EndStrafeLeft();
		break;
	case Palleon::KEY_CODE_D:
		m_mainCamera->EndStrafeRight();
		break;
	}
}

void CWorldEditor::NotifyInputCancelled()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_RELEASED);
	m_mainCamera->CancelInputTracking();
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

Palleon::CApplication* CreateApplication()
{
#ifdef _SCAN_LAYOUTS
	AnalyseDirectory(CFileManager::GetGamePath());
#endif
	return new CWorldEditor();
}
