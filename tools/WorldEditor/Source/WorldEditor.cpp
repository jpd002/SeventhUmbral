#include "WorldEditor.h"
#include "StdStream.h"
#include "StdStreamUtils.h"
#include "SectionLoader.h"
#include "Rendering/UmbralModel.h"
#include "FileManager.h"
#include "ResourceDefs.h"
#include "ResourceManager.h"
#include "Rendering/GlobalResources.h"

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

	auto mapLayoutPath = CFileManager::GetResourcePath(0x92050003);

	m_mapLayout = std::make_shared<CMapLayout>();
	m_mapLayout->Read(Framework::CreateInputStdStream(mapLayoutPath.native()));

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
		camera->SetPerspectiveProjection(M_PI / 4.f, screenSize.x / screenSize.y, 1.f, 10000.f, Athena::HANDEDNESS_RIGHTHANDED);
		m_mainCamera = camera;
	}

	{
		auto viewport = Athena::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		m_mainViewport = viewport;
	}

	auto sceneRoot = m_mainViewport->GetSceneRoot();

	unsigned int nodeIdx = 0;

	const auto& layoutNodes = m_mapLayout->GetLayoutNodes();
	for(const auto& nodePair : layoutNodes)
	{
		if(auto instanceNode = std::dynamic_pointer_cast<CMapLayout::INSTANCE_OBJECT_NODE>(nodePair.second))
		{
			auto refNodeIterator = layoutNodes.find(instanceNode->refNodePtr);
			if(refNodeIterator == std::end(layoutNodes)) continue;

			CVector3 instancePosition(instanceNode->posX, instanceNode->posY, instanceNode->posZ);
			CQuaternion instanceRotY(CVector3(0, -1, 0), instanceNode->rotY);

			auto refNode = refNodeIterator->second;
			if(auto unitTreeObjectNode = std::dynamic_pointer_cast<CMapLayout::UNIT_TREE_OBJECT_NODE>(refNode))
			{
				auto unitTreeObject = CreateUnitTreeObject(unitTreeObjectNode);
				unitTreeObject->SetPosition(instancePosition);
				unitTreeObject->SetRotation(instanceRotY);
				sceneRoot->AppendChild(unitTreeObject);
			}
		}
	}
}

Athena::SceneNodePtr CWorldEditor::CreateUnitTreeObject(const std::shared_ptr<CMapLayout::UNIT_TREE_OBJECT_NODE>& node)
{
	auto result = Athena::CSceneNode::Create();

	const auto& layoutNodes = m_mapLayout->GetLayoutNodes();

	for(const auto& item : node->items)
	{
		auto refNodeIterator = layoutNodes.find(item.nodePtr);
		if(refNodeIterator == std::end(layoutNodes)) continue;

		auto refNode = refNodeIterator->second;
		if(auto bgPartsBaseObjectNode = std::dynamic_pointer_cast<CMapLayout::BGPARTS_BASE_OBJECT_NODE>(refNode))
		{
			auto modelResource = CResourceManager::GetInstance().GetResource(bgPartsBaseObjectNode->resourceName);
			assert(modelResource);
			if(!modelResource) continue;

			auto modelChunk = modelResource->SelectNode<CModelChunk>();
			assert(modelChunk);
			if(!modelChunk) continue;

			CVector3 minPos(bgPartsBaseObjectNode->minX, bgPartsBaseObjectNode->minY, bgPartsBaseObjectNode->minZ);
			CVector3 maxPos(bgPartsBaseObjectNode->maxX, bgPartsBaseObjectNode->maxY, bgPartsBaseObjectNode->maxZ);

			CVector3 bgPartSize = (maxPos - minPos) / 2;
			CVector3 bgPartPos = (maxPos + minPos) / 2;

			auto model = std::make_shared<CUmbralModel>(modelChunk);
			model->SetPosition(bgPartPos);
			model->SetScale(bgPartSize);
			result->AppendChild(model);
		}
	}

	return result;
}

void CWorldEditor::Update(float dt)
{
	m_mainCamera->Update(dt);
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();
}

void CWorldEditor::NotifyMouseMove(unsigned int x, unsigned int y)
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
