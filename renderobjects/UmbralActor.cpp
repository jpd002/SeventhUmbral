#include "UmbralActor.h"
#include "string_format.h"
#include "StdStream.h"
#include "UmbralModel.h"
#include "../dataobjects/FileManager.h"

CUmbralActor::CUmbralActor()
{

}

CUmbralActor::~CUmbralActor()
{

}

const CSphere& CUmbralActor::GetBoundingSphere() const
{
	return m_boundingSphere;
}

void CUmbralActor::SetBaseModelId(uint32 baseModelId)
{
	m_baseModelId = baseModelId;
	m_renderableDirty = true;
}

void CUmbralActor::SetTopModelId(uint32 topModelId)
{
	m_topModelId = topModelId;
	m_renderableDirty = true;
}

void CUmbralActor::Update(float)
{
	if(m_renderableDirty)
	{
		RebuildActorRenderables();
		assert(m_renderableDirty == false);
	}
}

void CUmbralActor::RebuildActorRenderables()
{
	uint32 modelFolder = m_baseModelId % 10000;
	uint32 modelClass = m_baseModelId / 10000;
	const char* charaFolder = "";
	const char* charaPrefix = "";
	switch(modelClass)
	{
	case 1:
		charaFolder = "mon";
		charaPrefix = "m";
		break;
	case 2:
		charaFolder = "bgobj";
		charaPrefix = "b";
		break;
	case 4:
		charaFolder = "wep";
		charaPrefix = "w";
		break;
	default:
		assert(0);
		break;
	}

	uint32 subModelId = m_topModelId >> 10;
	uint32 textureId = (m_topModelId >> 5) & 0x1F;

	auto gamePath = CFileManager::GetGamePath();
	auto modelPath = string_format("%s/client/chara/%s/%s%0.3d/equ/e%0.3d/top_mdl/0001",
		gamePath.string().c_str(), charaFolder, charaPrefix, modelFolder, subModelId);
	auto texturePath = string_format("%s/client/chara/%s/%s%0.3d/equ/e%0.3d/top_tex2/%0.4d",
		gamePath.string().c_str(), charaFolder, charaPrefix, modelFolder, subModelId, textureId);

	Framework::CStdStream inputStream(modelPath.c_str(), "rb");

	auto modelResource = CSectionLoader::ReadSection(ResourceNodePtr(), inputStream);

	auto modelChunk = modelResource->SelectNode<CModelChunk>();
	assert(modelChunk);
	if(!modelChunk) return;

	auto boundingBox = modelChunk->SelectNode<CCompChunk>();

	CVector3 boxMin(boundingBox->GetMinX(), boundingBox->GetMinY(), boundingBox->GetMinZ());
	CVector3 boxMax(boundingBox->GetMaxX(), boundingBox->GetMaxY(), boundingBox->GetMaxZ());

	CVector3 modelSize = (boxMax - boxMin) / 2;
	CVector3 modelPos = (boxMax + boxMin) / 2;

	auto model = std::make_shared<CUmbralModel>(modelChunk);
	model->SetPosition(modelPos);
	model->SetScale(modelSize);
	AppendChild(model);

	auto modelBoundingSphere = model->GetBoundingSphere();
	modelBoundingSphere.radius *= std::max(std::max(modelSize.x, modelSize.y), modelSize.z);
	modelBoundingSphere.position += modelPos;
	m_boundingSphere = modelBoundingSphere;

	{
		Framework::CStdStream inputStream(texturePath.c_str(), "rb");
		auto textureResource = CSectionLoader::ReadSection(ResourceNodePtr(), inputStream);
		model->SetLocalTexture(textureResource);
	}

	m_renderableDirty = false;
}
