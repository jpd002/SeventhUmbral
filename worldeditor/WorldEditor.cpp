#include <boost/lexical_cast.hpp>
#include "WorldEditor.h"
#include "StdStream.h"
#include "StdStreamUtils.h"
#include "../dataobjects/FileManager.h"
#include "../renderobjects/ResourceManager.h"
#include "../renderobjects/GlobalResources.h"
#include "../renderobjects/UmbralActor.h"
#include "../renderobjects/UmbralMap.h"
#include "../renderobjects/UmbralModel.h"

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
#define MAIN_CAMERA_NEAR_Z	(1.0f)
#define MAIN_CAMERA_FAR_Z	(500.f)

CWorldEditor::CWorldEditor(bool isEmbedding)
: m_elapsed(0)
, m_mousePosition(0, 0)
{
	CGlobalResources::GetInstance().Initialize();

	CreateWorld();

	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_overlayViewport.get());

	if(!isEmbedding)
	{
		m_debugOverlay = std::make_shared<CDebugOverlay>();
		//CFileManager::SetGamePath("F:\\FFXIV_10_Copy");
		CreateMap(0xA09B0002);
	}
}

CWorldEditor::~CWorldEditor()
{
	m_debugOverlay.reset();
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_mainViewport.get());
	CGlobalResources::GetInstance().Release();
}

void CWorldEditor::CreateWorld()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	{
		auto camera = CTouchFreeCamera::Create();
		camera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z, Palleon::HANDEDNESS_RIGHTHANDED);
		camera->SetPosition(CVector3(0, 2, 0));
		m_mainCamera = camera;
	}

	{
		auto viewport = Palleon::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_mainViewport = viewport;
	}

	{
		auto lightDir0 = CVector3(1, -1, 0).Normalize();
		auto lightDir1 = CVector3(-1, -1, 0).Normalize();

		m_mainViewport->SetEffectParameter("ps_ambientLightColor", Palleon::CEffectParameter(CVector4(0, 0, 0, 0)));
		m_mainViewport->SetEffectParameter("ps_dirLightDirection0", Palleon::CEffectParameter(lightDir0));
		m_mainViewport->SetEffectParameter("ps_dirLightDirection1", Palleon::CEffectParameter(lightDir1));
		m_mainViewport->SetEffectParameter("ps_dirLightColor0", Palleon::CEffectParameter(CVector4(1.0f, 1.0f, 1.0f, 0)));
		m_mainViewport->SetEffectParameter("ps_dirLightColor1", Palleon::CEffectParameter(CVector4(1.0f, 1.0f, 1.0f, 0)));
	}

	{
		auto viewport = Palleon::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_overlayViewport = viewport;
	}
}

void CWorldEditor::CreateMap(uint32 mapId)
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
		auto mapLayoutPath = CFileManager::GetResourcePath(mapId);
		auto mapLayout = std::make_shared<CMapLayout>();
		auto mapStream = Framework::CreateInputStdStream(mapLayoutPath.native());
		mapLayout->Read(mapStream);

		auto map = std::make_shared<CUmbralMap>(mapLayout);
		sceneRoot->AppendChild(map);
	}
#if 0
	{
		auto mapLayoutPath = CFileManager::GetResourcePath(0x29D90002);
		auto mapLayout = std::make_shared<CMapLayout>();
		mapLayout->Read(Framework::CreateInputStdStream(mapLayoutPath.native()));

		auto map = std::make_shared<CUmbralMap>(mapLayout);
		sceneRoot->AppendChild(map);
	}
#endif
}

void CWorldEditor::CreateActors()
{
	//WIP
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
	m_mainCamera->Update(dt);
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_overlayViewport->GetSceneRoot()->Update(dt);
	m_overlayViewport->GetSceneRoot()->UpdateTransformations();

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
	m_mousePosition = CVector2(static_cast<float>(x), static_cast<float>(y));
	m_mainCamera->UpdateDrag(m_mousePosition);
}

void CWorldEditor::NotifyMouseDown()
{
	m_mainCamera->BeginDrag(m_mousePosition);
}

void CWorldEditor::NotifyMouseUp()
{
	m_mainCamera->EndDrag();
}

void CWorldEditor::NotifyKeyDown(Palleon::KEY_CODE keyCode)
{
	switch(keyCode)
	{
	case Palleon::KEY_CODE_ARROW_UP:
		m_mainCamera->BeginMoveForward();
		break;
	case Palleon::KEY_CODE_ARROW_DOWN:
		m_mainCamera->BeginMoveBackward();
		break;
	case Palleon::KEY_CODE_ARROW_LEFT:
		m_mainCamera->BeginStrafeLeft();
		break;
	case Palleon::KEY_CODE_ARROW_RIGHT:
		m_mainCamera->BeginStrafeRight();
		break;
	}
	
}

void CWorldEditor::NotifyKeyUp(Palleon::KEY_CODE keyCode)
{
	switch(keyCode)
	{
	case Palleon::KEY_CODE_ARROW_UP:
		m_mainCamera->EndMoveForward();
		break;
	case Palleon::KEY_CODE_ARROW_DOWN:
		m_mainCamera->EndMoveBackward();
		break;
	case Palleon::KEY_CODE_ARROW_LEFT:
		m_mainCamera->EndStrafeLeft();
		break;
	case Palleon::KEY_CODE_ARROW_RIGHT:
		m_mainCamera->EndStrafeRight();
		break;
	}
}

void CWorldEditor::NotifyInputCancelled()
{
	m_mainCamera->CancelInputTracking();
}

std::string CWorldEditor::NotifyExternalCommand(const std::string& command)
{
	Palleon::CEmbedRemoteCall rpc(command);
	auto method = rpc.GetMethod();
	if(method == "SetGamePath")
	{
		auto gamePath = rpc.GetParam("Path");
		CFileManager::SetGamePath(gamePath);
		return std::string("success");
	}
	if(method == "SetMap")
	{
		try
		{
			auto mapId = boost::lexical_cast<uint32>(rpc.GetParam("MapId"));
			CreateMap(mapId);
//			CreateActors();
//			CreateBaseAxis();
			return "success";
		}
		catch(...)
		{
			return "failed";
		}
		return "success";
	}
	return std::string("failed");
}

Palleon::CApplication* CreateApplication(bool isEmbedding)
{
	return new CWorldEditor(isEmbedding);
}
