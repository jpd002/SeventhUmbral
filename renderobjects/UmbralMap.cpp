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
				for(const auto& item : unitTreeObjectNode->items)
				{
					auto refNodeIterator = layoutNodes.find(item.nodePtr);
					assert(refNodeIterator != std::end(layoutNodes));
					if(refNodeIterator == std::end(layoutNodes)) continue;

					auto refNode = refNodeIterator->second;
					if(auto bgPartsBaseObjectNode = std::dynamic_pointer_cast<CMapLayout::BGPARTS_BASE_OBJECT_NODE>(refNode))
					{
						auto bgPartObjectIterator = m_bgPartObjects.find(item.nodePtr);
						assert(bgPartObjectIterator != std::end(m_bgPartObjects));
						if(bgPartObjectIterator == std::end(m_bgPartObjects)) continue;

						auto tempNode = Palleon::CSceneNode::Create();
						tempNode->AppendChild(bgPartObjectIterator->second);
						tempNode->SetPosition(instancePosition);
						tempNode->SetRotation(instanceRotY);
						tempNode->UpdateTransformations();
						tempNode->TraverseNodes(
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
				}
			}
			else if(auto bgPartObjectNode = std::dynamic_pointer_cast<CMapLayout::BGPARTS_BASE_OBJECT_NODE>(refNode))
			{
				auto bgPartObjectIterator = m_bgPartObjects.find(refNodeIterator->first);
				assert(bgPartObjectIterator != std::end(m_bgPartObjects));
				if(bgPartObjectIterator == std::end(m_bgPartObjects)) continue;

				auto tempNode = Palleon::CSceneNode::Create();
				tempNode->AppendChild(bgPartObjectIterator->second);
				tempNode->SetPosition(instancePosition);
				tempNode->SetRotation(instanceRotY);
				tempNode->UpdateTransformations();

				tempNode->TraverseNodes(
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
