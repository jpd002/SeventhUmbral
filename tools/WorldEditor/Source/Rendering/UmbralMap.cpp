#include "UmbralMap.h"
#include "../ResourceManager.h"

CUmbralMap::CUmbralMap(const MapLayoutPtr& mapLayout)
{
	const auto& layoutNodes = mapLayout->GetLayoutNodes();

	//Build unit tree object nodes
	for(const auto& nodePair : layoutNodes)
	{
		if(auto unitTreeObjectNode = std::dynamic_pointer_cast<CMapLayout::UNIT_TREE_OBJECT_NODE>(nodePair.second))
		{
			auto unitTreeObjectRenderable = CreateUnitTreeObject(mapLayout, unitTreeObjectNode);
			m_unitTreeObjectRenderables.insert(std::make_pair(nodePair.first, unitTreeObjectRenderable));
		}
	}

//	unsigned int nodeIdx = 0;
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
				auto unitTreeObjectRenderableIterator = m_unitTreeObjectRenderables.find(refNodeIterator->first);
				assert(unitTreeObjectRenderableIterator != std::end(m_unitTreeObjectRenderables));
				if(unitTreeObjectRenderableIterator == std::end(m_unitTreeObjectRenderables))
				{
					continue;
				}

				auto unitTreeObjectRenderable = unitTreeObjectRenderableIterator->second;
				unitTreeObjectRenderable->SetPosition(instancePosition);
				unitTreeObjectRenderable->SetRotation(instanceRotY);
				unitTreeObjectRenderable->UpdateTransformations();

				unitTreeObjectRenderable->TraverseNodes(
					[&] (const Athena::SceneNodePtr& node)
					{
						if(auto mesh = std::dynamic_pointer_cast<CUmbralMesh>(node))
						{
							auto instance = mesh->CreateInstance();
							m_instances.push_back(instance);
						}
						return true;
					}
				);

//				nodeIdx++;
			}
		}
	}
}

CUmbralMap::~CUmbralMap()
{

}

void CUmbralMap::GetMeshes(Athena::MeshArray& meshes, const Athena::CCamera* camera)
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

Athena::SceneNodePtr CUmbralMap::CreateUnitTreeObject(const MapLayoutPtr& mapLayout, const std::shared_ptr<CMapLayout::UNIT_TREE_OBJECT_NODE>& node)
{
	auto result = Athena::CSceneNode::Create();

	const auto& layoutNodes = mapLayout->GetLayoutNodes();

	int nodeIdx = 0;

	for(const auto& item : node->items)
	{
		auto refNodeIterator = layoutNodes.find(item.nodePtr);
		if(refNodeIterator == std::end(layoutNodes)) continue;

		auto refNode = refNodeIterator->second;
		if(auto bgPartsBaseObjectNode = std::dynamic_pointer_cast<CMapLayout::BGPARTS_BASE_OBJECT_NODE>(refNode))
		{
			//if(nodeIdx == 1)
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

			nodeIdx++;
		}
	}

	return result;
}
