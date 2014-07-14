#include "UmbralMap.h"
#include "ResourceManager.h"

CUmbralMap::CUmbralMap(const MapLayoutPtr& mapLayout)
{
	for(const auto& resourceItem : mapLayout->GetResourceItems())
	{
		if(resourceItem.type == '\0trb' || resourceItem.type == '\0txb')
		{
			CResourceManager::GetInstance().LoadResource(resourceItem.resourceId, resourceItem.name);
		}
	}

	const auto& layoutNodes = mapLayout->GetLayoutNodes();

	//Build bg parts
	for(const auto& nodePair : layoutNodes)
	{
		if(auto bgPartObjectNode = std::dynamic_pointer_cast<CMapLayout::BGPARTS_BASE_OBJECT_NODE>(nodePair.second))
		{
			auto bgPartObject = CreateBgPartObject(mapLayout, bgPartObjectNode);
			assert(bgPartObject);
			m_bgPartObjects.insert(std::make_pair(nodePair.first, bgPartObject));
		}
	}

	//Build unit tree object nodes
	for(const auto& nodePair : layoutNodes)
	{
		if(auto unitTreeObjectNode = std::dynamic_pointer_cast<CMapLayout::UNIT_TREE_OBJECT_NODE>(nodePair.second))
		{
			auto unitTreeObject = CreateUnitTreeObject(mapLayout, unitTreeObjectNode);
			m_unitTreeObjects.insert(std::make_pair(nodePair.first, unitTreeObject));
		}
	}

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
				auto unitTreeObjectIterator = m_unitTreeObjects.find(refNodeIterator->first);
				assert(unitTreeObjectIterator != std::end(m_unitTreeObjects));
				if(unitTreeObjectIterator == std::end(m_unitTreeObjects)) continue;

				auto unitTreeObject = unitTreeObjectIterator->second;
				unitTreeObject->SetPosition(instancePosition);
				unitTreeObject->SetRotation(instanceRotY);
				unitTreeObject->UpdateTransformations();

				unitTreeObject->TraverseNodes(
					[&] (const Palleon::SceneNodePtr& node)
					{
						if(auto mesh = std::dynamic_pointer_cast<CUmbralMesh>(node))
						{
							auto instance = mesh->CreateInstance();
							m_instances.push_back(instance);
						}
						return true;
					}
				);
			}
			else if(auto bgPartObjectNode = std::dynamic_pointer_cast<CMapLayout::BGPARTS_BASE_OBJECT_NODE>(refNode))
			{
				auto bgPartObjectIterator = m_bgPartObjects.find(refNodeIterator->first);
				assert(bgPartObjectIterator != std::end(m_bgPartObjects));
				if(bgPartObjectIterator == std::end(m_bgPartObjects)) continue;

				auto bgPartObject = bgPartObjectIterator->second;
				auto prevParent = bgPartObject->GetParent();
				if(prevParent)
				{
					prevParent->RemoveChild(bgPartObject);
				}

				auto tempNode = Palleon::CSceneNode::Create();
				tempNode->AppendChild(bgPartObject);
				tempNode->SetPosition(instancePosition);
				tempNode->SetRotation(instanceRotY);
				tempNode->UpdateTransformations();

				bgPartObject->TraverseNodes(
					[&] (const Palleon::SceneNodePtr& node)
					{
						if(auto mesh = std::dynamic_pointer_cast<CUmbralMesh>(node))
						{
							auto instance = mesh->CreateInstance();
							m_instances.push_back(instance);
						}
						return true;
					}
				);

				tempNode->RemoveAllChildren();
				if(prevParent)
				{
					prevParent->AppendChild(bgPartObject);
				}
			}
		}
	}
}

CUmbralMap::~CUmbralMap()
{

}

void CUmbralMap::GetMeshes(Palleon::MeshArray& meshes, const Palleon::CCamera* camera)
{
	auto cameraFrustum = camera->GetFrustum();
	for(const auto& instance : m_instances)
	{
		auto boundingSphere = instance->GetWorldBoundingSphere();
		if(cameraFrustum.Intersects(boundingSphere))
		{
			meshes.push_back(instance.get());
		}
	}
}

UmbralModelPtr CUmbralMap::CreateBgPartObject(const MapLayoutPtr& mapLayout, const std::shared_ptr<CMapLayout::BGPARTS_BASE_OBJECT_NODE>& node)
{
	UmbralModelPtr result;

	auto modelResource = CResourceManager::GetInstance().GetResource(node->resourceName);
	assert(modelResource);
	if(!modelResource) return result;

	auto modelChunk = modelResource->SelectNode<CModelChunk>();
	assert(modelChunk);
	if(!modelChunk) return result;

	CVector3 minPos(node->minX, node->minY, node->minZ);
	CVector3 maxPos(node->maxX, node->maxY, node->maxZ);

	CVector3 bgPartSize = (maxPos - minPos) / 2;
	CVector3 bgPartPos = (maxPos + minPos) / 2;

	result = std::make_shared<CUmbralModel>(modelChunk);
	result->SetPosition(bgPartPos);
	result->SetScale(bgPartSize);
				
	result->TraverseNodes(
		[&] (const Palleon::SceneNodePtr& node)
		{
			if(auto mesh = std::dynamic_pointer_cast<CUmbralMesh>(node))
			{
				mesh->RebuildIndices();
			}
			return true;
		}
	);

	return result;
}

Palleon::SceneNodePtr CUmbralMap::CreateUnitTreeObject(const MapLayoutPtr& mapLayout, const std::shared_ptr<CMapLayout::UNIT_TREE_OBJECT_NODE>& node)
{
	auto result = Palleon::CSceneNode::Create();

	const auto& layoutNodes = mapLayout->GetLayoutNodes();

	for(const auto& item : node->items)
	{
		auto refNodeIterator = layoutNodes.find(item.nodePtr);
		if(refNodeIterator == std::end(layoutNodes)) continue;

		auto refNode = refNodeIterator->second;
		if(auto bgPartsBaseObjectNode = std::dynamic_pointer_cast<CMapLayout::BGPARTS_BASE_OBJECT_NODE>(refNode))
		{
			auto bgPartObjectIterator = m_bgPartObjects.find(item.nodePtr);
			if(bgPartObjectIterator == std::end(m_bgPartObjects)) continue;
			result->AppendChild(bgPartObjectIterator->second);
		}
	}

	return result;
}
