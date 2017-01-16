#include "UmbralMap.h"
#include "ResourceManager.h"

CUmbralMap::CUmbralMap(const MapLayoutPtr& mapLayout)
{
	m_initThread = std::thread([&, mapLayout] () { InitializeMap(mapLayout); } );
}

CUmbralMap::~CUmbralMap()
{
	CancelLoading();
}

void CUmbralMap::GetMeshes(Palleon::MeshArray& meshes, const Palleon::CCamera* camera)
{
	std::lock_guard<std::mutex> instancesLock(m_instancesMutex);
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

void CUmbralMap::CancelLoading()
{
	if(m_initThread.joinable())
	{
		m_loadingCanceled = true;
		m_initThread.join();
	}
}

void CUmbralMap::InitializeMap(const MapLayoutPtr& mapLayout)
{
	for(const auto& resourceItem : mapLayout->GetResourceItems())
	{
		if(resourceItem.type == '\0trb' || resourceItem.type == '\0txb')
		{
			CResourceManager::GetInstance().LoadResource(resourceItem.resourceId, resourceItem.name);
		}
	}

	auto instancesToBuild = GetInstancesToBuild(mapLayout);
	CreateInstances(mapLayout, instancesToBuild);
}

CUmbralMap::InstanceInfoMap CUmbralMap::GetInstancesToBuild(const MapLayoutPtr& mapLayout)
{
	InstanceInfoMap instancesToBuild;

	const auto& layoutNodes = mapLayout->GetLayoutNodes();

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
						INSTANCE_INFO instanceInfo;
						instanceInfo.position	= instancePosition;
						instanceInfo.rotation	= instanceRotY;
						instancesToBuild.insert(std::make_pair(item.nodePtr, instanceInfo));
					}
				}
			}
			else if(auto bgPartObjectNode = std::dynamic_pointer_cast<CMapLayout::BGPARTS_BASE_OBJECT_NODE>(refNode))
			{
				INSTANCE_INFO instanceInfo;
				instanceInfo.position	= instancePosition;
				instanceInfo.rotation	= instanceRotY;
				instancesToBuild.insert(std::make_pair(refNodeIterator->first, instanceInfo));
			}
		}
	}

	return std::move(instancesToBuild);
}

void CUmbralMap::CreateInstances(const MapLayoutPtr& mapLayout, const InstanceInfoMap& instancesToBuild)
{
	auto sharedContext = Palleon::CGraphicDevice::GetInstance().CreateSharedContext();

	const auto& layoutNodes = mapLayout->GetLayoutNodes();

	//Build bg parts
	for(const auto& nodePair : layoutNodes)
	{
		if(auto bgPartObjectNode = std::dynamic_pointer_cast<CMapLayout::BGPARTS_BASE_OBJECT_NODE>(nodePair.second))
		{
			if(m_loadingCanceled) return;

			auto bgPartObject = CreateBgPartObject(mapLayout, bgPartObjectNode);
			assert(bgPartObject);
			m_bgPartObjects.insert(std::make_pair(nodePair.first, bgPartObject));

			auto instancesBegin = instancesToBuild.lower_bound(nodePair.first);
			auto instancesEnd = instancesToBuild.upper_bound(nodePair.first);

			for(auto instanceIterator = instancesBegin;
				instanceIterator != instancesEnd; instanceIterator++)
			{
				const auto& instanceInfo(instanceIterator->second);

				auto tempNode = Palleon::CSceneNode::Create();
				tempNode->AppendChild(bgPartObject);
				tempNode->SetPosition(instanceInfo.position);
				tempNode->SetRotation(instanceInfo.rotation);
				tempNode->UpdateTransformations();
				tempNode->TraverseNodes(
					[&] (const Palleon::SceneNodePtr& node)
					{
						if(auto mesh = std::dynamic_pointer_cast<CUmbralMesh>(node))
						{
							std::lock_guard<std::mutex> instancesLock(m_instancesMutex);
							auto instance = mesh->CreateInstance();
							m_instances.push_back(instance);
						}
						return true;
					}
				);
			}

			sharedContext->Flush();
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
