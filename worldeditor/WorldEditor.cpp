#include <boost/lexical_cast.hpp>
#include "WorldEditor.h"
#include "StdStream.h"
#include "StdStreamUtils.h"
#include "../dataobjects/FileManager.h"
#include "../renderobjects/ResourceManager.h"
#include "../renderobjects/GlobalResources.h"
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

CVector3 g_translateXPlaneNormals[2] =
{
	CVector3(0, 1, 0),
	CVector3(0, 0, 1),
};

CVector3 g_translateYPlaneNormals[2] =
{
	CVector3(1, 0, 0),
	CVector3(0, 0, 1)
};

CVector3 g_translateZPlaneNormals[2] =
{
	CVector3(1, 0, 0),
	CVector3(0, 1, 0)
};

CWorldEditor::CWorldEditor(bool isEmbedding)
: m_isEmbedding(isEmbedding)
{
	CGlobalResources::GetInstance().Initialize();

	CreateWorld();

	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_overlayViewport.get());

	m_translationGizmo = CTranslationGizmo::Create();

	if(!isEmbedding)
	{
		m_debugOverlay = std::make_shared<CDebugOverlay>();
		CFileManager::SetGamePath("C:\\Program Files (x86)\\SquareEnix\\FINAL FANTASY XIV");
//		CreateMap(0x29B00001);
		CreateActor(1, 10005);
		SetActorPosition(1, CVector3(0, -1, 0));
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
	}
}

void CWorldEditor::CreateActor(uint32 id, uint32 baseModelId)
{
	assert(m_actors.find(id) == m_actors.end());

	auto sceneRoot = m_mainViewport->GetSceneRoot();
	
	{
		auto actor = std::make_shared<CUmbralActor>();
		actor->SetBaseModelId(baseModelId);
		actor->SetPosition(CVector3(0, 2, 0));
		sceneRoot->AppendChild(actor);
		m_actors.insert(std::make_pair(id, actor));
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

	if(m_translationGizmo->GetParent())
	{
		auto mouseRay = GetMouseRay();
		m_translationGizmo->UpdateHoverState(mouseRay);
	}

	if(m_state == STATE_TRANSLATE)
	{
		m_lastIntersectPosition = m_intersectPosition;
		auto mouseRay = GetMouseRay();
		auto selectedObject = m_translationGizmo->GetParent();
		auto objectTransform = selectedObject->GetWorldTransformation();
		CVector3 objectPosition(objectTransform(3, 0), objectTransform(3, 1), objectTransform(3, 2));
		if(m_translationMode == CTranslationGizmo::HITTEST_X)
		{
			auto translationPlane = CPlane();
			if(fabs(m_dot[0]) > fabs(m_dot[1]))
			{
				translationPlane = CPlane(g_translateXPlaneNormals[0], objectPosition.y);
			}
			else
			{
				translationPlane = CPlane(g_translateXPlaneNormals[1], objectPosition.z);
			}
			auto intersectResult = Intersects(translationPlane, mouseRay);
			if(intersectResult.first)
			{
				m_intersectPosition = intersectResult.second;
				if(!m_lastIntersectPosition.IsNull())
				{
					m_delta = m_intersectPosition - m_lastIntersectPosition;
				}
				selectedObject->SetPosition(selectedObject->GetPosition() + CVector3(m_delta.x, 0, 0));
			}
		}
		else if(m_translationMode == CTranslationGizmo::HITTEST_Y)
		{
			auto translationPlane = CPlane();
			if(fabs(m_dot[0]) > fabs(m_dot[1]))
			{
				translationPlane = CPlane(g_translateYPlaneNormals[0], objectPosition.x);
			}
			else
			{
				translationPlane = CPlane(g_translateYPlaneNormals[1], objectPosition.z);
			}
			auto intersectResult = Intersects(translationPlane, mouseRay);
			if(intersectResult.first)
			{
				m_intersectPosition = intersectResult.second;
				if(!m_lastIntersectPosition.IsNull())
				{
					m_delta = m_intersectPosition - m_lastIntersectPosition;
				}
				selectedObject->SetPosition(selectedObject->GetPosition() + CVector3(0, m_delta.y, 0));
			}
		}
		else if(m_translationMode == CTranslationGizmo::HITTEST_Z)
		{
			auto translationPlane = CPlane();
			if(fabs(m_dot[0]) > fabs(m_dot[1]))
			{
				translationPlane = CPlane(g_translateZPlaneNormals[0], objectPosition.x);
			}
			else
			{
				translationPlane = CPlane(g_translateZPlaneNormals[1], objectPosition.y);
			}
			auto intersectResult = Intersects(translationPlane, mouseRay);
			if(intersectResult.first)
			{
				m_intersectPosition = intersectResult.second;
				if(!m_lastIntersectPosition.IsNull())
				{
					m_delta = m_intersectPosition - m_lastIntersectPosition;
				}
				selectedObject->SetPosition(selectedObject->GetPosition() + CVector3(0, 0, m_delta.z));
			}
		}
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
//	m_mainCamera->BeginDrag(m_mousePosition);

	assert(m_state == STATE_IDLE);
	if(m_translationGizmo->GetParent())
	{
		auto mouseRay = GetMouseRay();
		auto testResult = m_translationGizmo->HitTest(mouseRay);
		if(testResult != CTranslationGizmo::HITTEST_NONE)
		{
			m_translationGizmo->SetStickyMode(testResult);
			m_state = STATE_TRANSLATE;
			m_translationMode = testResult;
			m_intersectPosition = CVector3(0, 0, 0);
			m_lastIntersectPosition = CVector3(0, 0, 0);
			m_delta = CVector3(0, 0, 0);

			//Either xy plane or xz plane
			switch(testResult)
			{
			case CTranslationGizmo::HITTEST_X:
				for(unsigned int i = 0; i < 2; i++)
				{
					m_dot[i] = mouseRay.direction.Dot(g_translateXPlaneNormals[i]);
				}
				break;
			case CTranslationGizmo::HITTEST_Y:
				for(unsigned int i = 0; i < 2; i++)
				{
					m_dot[i] = mouseRay.direction.Dot(g_translateYPlaneNormals[i]);
				}
				break;
			case CTranslationGizmo::HITTEST_Z:
				for(unsigned int i = 0; i < 2; i++)
				{
					m_dot[i] = mouseRay.direction.Dot(g_translateZPlaneNormals[i]);
				}
				break;
			}
		}
	}
}

void CWorldEditor::NotifyMouseUp()
{
//	m_mainCamera->EndDrag();

	if(m_state == STATE_IDLE)
	{
		auto mouseRay = GetMouseRay();

		if(m_translationGizmo->GetParent())
		{
			m_translationGizmo->GetParent()->RemoveChild(m_translationGizmo);
		}

		for(const auto& actorPair : m_actors)
		{
			const auto& actor = actorPair.second;
			auto boundingSphere = Transform(actor->GetBoundingSphere(), actor->GetWorldTransformation());
			auto intersectResult = Intersects(boundingSphere, mouseRay);
			if(intersectResult.first)
			{
				if(m_isEmbedding)
				{
					Palleon::CEmbedRemoteCall rpc;
					rpc.SetMethod("selected");
					rpc.SetParam("actorId", std::to_string(actorPair.first));
					Palleon::CEmbedManager::GetInstance().NotifyClient(rpc);
				}
				actor->AppendChild(m_translationGizmo);
				break;
			}
		}
	}
	else
	{
		m_state = STATE_IDLE;
		m_translationGizmo->SetStickyMode(CTranslationGizmo::HITTEST_NONE);
		m_translationMode = CTranslationGizmo::HITTEST_NONE;
	}
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
	else if(method == "SetMap")
	{
		try
		{
			auto mapId = boost::lexical_cast<uint32>(rpc.GetParam("MapId"));
			CreateMap(mapId);
			return "success";
		}
		catch(...)
		{
			return "failed";
		}
		return "success";
	}
	else if(method == "CreateActor")
	{
		try
		{
			auto id = boost::lexical_cast<uint32>(rpc.GetParam("Id"));
			auto baseModelId = boost::lexical_cast<uint32>(rpc.GetParam("BaseModelId"));
			CreateActor(id, baseModelId);
		}
		catch(...)
		{
			return "failed";
		}
	}
	return std::string("failed");
}

CRay CWorldEditor::GetMouseRay() const
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();
	auto screenPos = (m_mousePosition / screenSize) * 2 - CVector2(1, 1);
	auto ray = m_mainCamera->Unproject(screenPos);
	return ray;
}

Palleon::CApplication* CreateApplication(bool isEmbedding)
{
	return new CWorldEditor(isEmbedding);
}
