#include "WorldEditor.h"
#include "StdStream.h"
#include "StdStreamUtils.h"
#include "SectionLoader.h"
#include "UmbralModel.h"
#include "FileManager.h"
#include "ResourceDefs.h"
#include "ResourceManager.h"

CWorldEditor::CWorldEditor()
: m_elapsed(0)
, m_mousePosition(0, 0)
, m_forwardButtonBoundingBox(0, 0, 0, 0)
, m_backwardButtonBoundingBox(0, 0, 0, 0)
{
	m_package = Athena::CPackage::Create("global");

	auto mapLayoutPath = CFileManager::GetResourcePath(0xA09B0000);		//+1 for Limsa Lominsa room, +2 for Ul'dah room
//	auto mapLayoutPath = CFileManager::GetResourcePath(0x72AD0000);
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
				auto button = scene->FindNode<Athena::CButtonBase>("PrevModelButton");
				button->Press.connect([&] (Athena::CSceneNode*) { OnPrevModelButtonPress(); });
			}

			{
				auto button = scene->FindNode<Athena::CButtonBase>("NextModelButton");
				button->Press.connect([&] (Athena::CSceneNode*) { OnNextModelButtonPress(); });
			}

			{
				auto button = scene->FindNode<Athena::CButtonBase>("PrevTextureButton");
				button->Press.connect([&] (Athena::CSceneNode*) { OnPrevTextureButtonPress(); });
			}

			{
				auto button = scene->FindNode<Athena::CButtonBase>("NextTextureButton");
				button->Press.connect([&] (Athena::CSceneNode*) { OnNextTextureButtonPress(); });
			}

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
		camera->SetPerspectiveProjection(M_PI / 4.f, screenSize.x / screenSize.y, 1.f, 1000.f);
		camera->SetPosition(CVector3(0, 0, -100.f));
//		camera->LookAt(CVector3(0.f, 0, 100.f), CVector3(0, 0, 0), CVector3(0, 1, 0));
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

			auto refNode = refNodeIterator->second;
			if(auto unitTreeObjectNode = std::dynamic_pointer_cast<CMapLayout::UNIT_TREE_OBJECT_NODE>(refNode))
			{
				auto unitTreeObject = CreateUnitTreeObject(unitTreeObjectNode);
				unitTreeObject->SetPosition(instancePosition);
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

void CWorldEditor::OnPrevModelButtonPress()
{
//	if(m_currentModelIdx == 0) return;
//	SetCurrentModel(m_currentModelIdx - 1);
}

void CWorldEditor::OnNextModelButtonPress()
{
//	if(m_currentModelIdx == (m_models.size() - 1)) return;
//	SetCurrentModel(m_currentModelIdx + 1);
}

void CWorldEditor::OnPrevTextureButtonPress()
{
//	if(m_currentTextureIdx == 0) return;
//	SetCurrentTexture(m_currentTextureIdx - 1);
}

void CWorldEditor::OnNextTextureButtonPress()
{
//	if(m_currentTextureIdx == (m_textures.size() - 1)) return;
//	SetCurrentTexture(m_currentTextureIdx + 1);
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
