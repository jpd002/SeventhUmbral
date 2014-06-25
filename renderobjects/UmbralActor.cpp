#include "UmbralActor.h"
#include "string_format.h"
#include "StdStream.h"
#include "UmbralModel.h"
#include "UmbralMesh.h"
#include "../dataobjects/FileManager.h"
#include "WeaponVars.h"

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

void ReplaceMaterialParam(const Palleon::MaterialPtr& material, const char* paramName, const CVector4& newParamValue)
{
	auto param = material->GetEffectParameter(paramName);
	CVector4 paramValue(0, 0, 0, 0);
	if(param.IsVector3())
	{
		paramValue = CVector4(param.GetVector3(), 0);
	}
	else
	{
		paramValue = param.GetVector4();
	}
	paramValue.x = (newParamValue.x == 1000.f) ? paramValue.x : newParamValue.x;
	paramValue.y = (newParamValue.y == 1000.f) ? paramValue.y : newParamValue.y;
	paramValue.z = (newParamValue.z == 1000.f) ? paramValue.z : newParamValue.z;
	paramValue.w = (newParamValue.w == 1000.f) ? paramValue.w : newParamValue.w;
	if(param.IsVector3())
	{
		param.SetVector3(paramValue.xyz());
	}
	else
	{
		param.SetVector4(paramValue);
	}
	material->SetEffectParameter(paramName, param);
}

void ReplaceMaterialParam(const Palleon::MaterialPtr& material, const char* paramName, float newParamValue)
{
	auto param = material->GetEffectParameter(paramName);
	float paramValue = param.GetScalar();
	paramValue = (newParamValue == 1000.f) ? paramValue : newParamValue;
	param.SetScalar(paramValue);
	material->SetEffectParameter(paramName, param);
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
	uint32 variation = m_topModelId & 0x3FF;

	auto gamePath = CFileManager::GetGamePath();
	auto modelPath = string_format("%s/client/chara/%s/%s%0.3d/equ/e%0.3d/top_mdl/0001",
		gamePath.string().c_str(), charaFolder, charaPrefix, modelFolder, subModelId);

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

	uint32 textureId = 0;
	if(modelClass == 4)
	{
		uint32 varWepId = 1000000000 + (modelFolder * 1000000) + (subModelId * 1000) + variation;
		auto var = CWeaponVars::GetInstance().GetVarForId(varWepId);
		textureId = var.textureId;

		for(const auto& meshNode : model->GetChildren())
		{
			if(auto mesh = std::dynamic_pointer_cast<CUmbralMesh>(meshNode))
			{
				auto meshName = mesh->GetName();
				int materialId = 0;
				if(meshName.find("_a") != std::string::npos)
				{
					materialId = 0;
				}
				if(meshName.find("_b") != std::string::npos)
				{
					materialId = 1;
				}
				const auto& varWepMaterial = var.materials[materialId];
				auto material = mesh->GetMaterial();
				ReplaceMaterialParam(material, "ps_diffuseColor", varWepMaterial.diffuseColor);
				ReplaceMaterialParam(material, "ps_multiDiffuseColor", varWepMaterial.multiDiffuseColor);
				ReplaceMaterialParam(material, "ps_specularColor", varWepMaterial.specularColor);
				ReplaceMaterialParam(material, "ps_multiSpecularColor", varWepMaterial.multiSpecularColor);
				ReplaceMaterialParam(material, "ps_reflectivity", varWepMaterial.specularColor);
				ReplaceMaterialParam(material, "ps_multiReflectivity", varWepMaterial.multiSpecularColor);
				ReplaceMaterialParam(material, "ps_shininess", varWepMaterial.shininess);
				ReplaceMaterialParam(material, "ps_multiShininess", varWepMaterial.multiShininess);
			}
		}
	}

	{
		auto texturePath = string_format("%s/client/chara/%s/%s%0.3d/equ/e%0.3d/top_tex2/%0.4d",
			gamePath.string().c_str(), charaFolder, charaPrefix, modelFolder, subModelId, textureId);

		Framework::CStdStream inputStream(texturePath.c_str(), "rb");
		auto textureResource = CSectionLoader::ReadSection(ResourceNodePtr(), inputStream);
		model->SetLocalTexture(textureResource);
	}

	m_renderableDirty = false;
}
