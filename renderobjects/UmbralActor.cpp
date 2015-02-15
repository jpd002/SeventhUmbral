#include "UmbralActor.h"
#include "string_format.h"
#include "StdStream.h"
#include "UmbralModel.h"
#include "UmbralMesh.h"
#include "../dataobjects/FileManager.h"
#include "../dataobjects/itemvar/WeaponVars.h"
#include "../dataobjects/itemvar/EquipVars.h"
#include "../dataobjects/itemvar/TexPathVars.h"

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

void CUmbralActor::SetBaseModelId(uint32 modelId)
{
	m_baseModelId = modelId;
	m_renderableDirty = true;
}

void CUmbralActor::SetHelmModelId(uint32 modelId)
{
	m_helmModelId = modelId;
	m_renderableDirty = true;
}

void CUmbralActor::SetTopModelId(uint32 modelId)
{
	m_topModelId = modelId;
	m_renderableDirty = true;
}

void CUmbralActor::SetBottomModelId(uint32 modelId)
{
	m_bottomModelId = modelId;
	m_renderableDirty = true;
}

void CUmbralActor::Update(float dt)
{
	CSceneNode::Update(dt);
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
	RemoveAllChildren();

	m_boundingSphere = CSphere();

	LoadSubModel(SUBMODEL_HELM, m_helmModelId);
	LoadSubModel(SUBMODEL_TOP, m_topModelId);
	LoadSubModel(SUBMODEL_BOTTOM, m_bottomModelId);
	LoadSubModel(SUBMODEL_GLOVE, m_gloveModelId);
	LoadSubModel(SUBMODEL_SHOE, m_shoeModelId);

	m_renderableDirty = false;
}

void CUmbralActor::LoadSubModel(SUBMODEL_TYPE submodelType, uint32 submodelId)
{
	std::vector<Palleon::SceneNodePtr> modelsToAdd;

	try
	{
		uint32 modelFolder = m_baseModelId % 10000;
		auto actorType = static_cast<ACTOR_TYPE>(m_baseModelId / 10000);
		const char* charaFolder = "";
		const char* charaPrefix = "";
		switch(actorType)
		{
		case ACTOR_PC:
			charaFolder = "pc";
			charaPrefix = "c";
			break;
		case ACTOR_MONSTER:
			charaFolder = "mon";
			charaPrefix = "m";
			break;
		case ACTOR_BGOBJ:
			charaFolder = "bgobj";
			charaPrefix = "b";
			break;
		case ACTOR_WEAPON:
			charaFolder = "wep";
			charaPrefix = "w";
			break;
		default:
			assert(0);
			break;
		}

		const char* partFolder = "";
		switch(submodelType)
		{
		case SUBMODEL_HELM:
			partFolder = "met";
			break;
		case SUBMODEL_TOP:
			partFolder = "top";
			break;
		case SUBMODEL_BOTTOM:
			partFolder = "dwn";
			break;
		case SUBMODEL_GLOVE:
			partFolder = "glv";
			break;
		case SUBMODEL_SHOE:
			partFolder = "sho";
			break;
		default:
			assert(0);
			break;
		}

		uint32 subModelId = submodelId >> 10;
		uint32 textureId = (submodelId >> 5) & 0x1F;
		uint32 variation = submodelId & 0x1F;
		uint32 textureFolder = modelFolder;
		uint32 textureFileNumber = textureId;

		auto gamePath = CFileManager::GetGamePath();

		if(actorType == ACTOR_WEAPON)
		{
			//Variation is submodelId & 0x3FF?
			uint32 varWepId = ComputeVarWepId(modelFolder, subModelId, textureId, variation);
			auto var = CWeaponVars::GetInstance().GetVarForId(varWepId);
			textureFileNumber = var.textureId;
		}
		else if(actorType == ACTOR_PC)
		{
			uint32 varEquipId = ComputeVarEquipId(actorType, submodelType, subModelId, textureId, variation);
			auto var = CEquipVars::GetInstance().GetVarForId(varEquipId);
			textureFolder = CTexPathVars::GetInstance().GetTexturePathId(var.textureId, modelFolder);
		}

		auto textureResource = ResourceNodePtr();

		//Load texture
		{
			auto texturePath = string_format("%s/client/chara/%s/%s%0.3d/equ/e%0.3d/%s_tex2/%0.4d",
				gamePath.string().c_str(), charaFolder, charaPrefix, textureFolder, subModelId, partFolder, textureFileNumber);

			Framework::CStdStream inputStream(texturePath.c_str(), "rb");
			textureResource = CSectionLoader::ReadSection(ResourceNodePtr(), inputStream);
		}

		//Load model
		auto modelPath = string_format("%s/client/chara/%s/%s%0.3d/equ/e%0.3d/%s_mdl/0001",
			gamePath.string().c_str(), charaFolder, charaPrefix, modelFolder, subModelId, partFolder);

		Framework::CStdStream inputStream(modelPath.c_str(), "rb");

		auto modelResource = CSectionLoader::ReadSection(ResourceNodePtr(), inputStream);

		auto modelChunks = modelResource->SelectNodes<CModelChunk>();
		assert(!modelChunks.empty());
		for(const auto& modelChunk : modelChunks)
		{
			auto model = CreateModel(modelChunk);
			modelsToAdd.push_back(model);

			if(actorType == ACTOR_PC)
			{
				uint32 varEquipId = ComputeVarEquipId(actorType, submodelType, subModelId, textureId, variation);
				auto var = CEquipVars::GetInstance().GetVarForId(varEquipId);

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
						if(meshName.find("_c") != std::string::npos)
						{
							materialId = 2;
						}
						if(meshName.find("_d") != std::string::npos)
						{
							assert(0);
						}
						const auto& varMaterial = var.materials[materialId];
						auto material = mesh->GetMaterial();
						ReplaceMaterialParam(material, "ps_diffuseColor", varMaterial.diffuseColor);
						ReplaceMaterialParam(material, "ps_multiDiffuseColor", varMaterial.multiDiffuseColor);
						ReplaceMaterialParam(material, "ps_specularColor", varMaterial.specularColor);
						ReplaceMaterialParam(material, "ps_multiSpecularColor", varMaterial.multiSpecularColor);
						ReplaceMaterialParam(material, "ps_reflectivity", varMaterial.specularColor);
						ReplaceMaterialParam(material, "ps_multiReflectivity", varMaterial.multiSpecularColor);
						ReplaceMaterialParam(material, "ps_shininess", varMaterial.shininess);
						ReplaceMaterialParam(material, "ps_multiShininess", varMaterial.multiShininess);
						mesh->SetActivePolyGroups(var.polyGroupState);
					}
				}
			}
			else if(actorType == ACTOR_WEAPON)
			{
				uint32 varWepId = ComputeVarWepId(modelFolder, subModelId, textureId, variation);
				auto var = CWeaponVars::GetInstance().GetVarForId(varWepId);

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
						if(meshName.find("_c") != std::string::npos)
						{
							materialId = 2;
						}
						if(meshName.find("_d") != std::string::npos)
						{
							assert(0);
						}
						const auto& varMaterial = var.materials[materialId];
						auto material = mesh->GetMaterial();
						ReplaceMaterialParam(material, "ps_diffuseColor", varMaterial.diffuseColor);
						ReplaceMaterialParam(material, "ps_multiDiffuseColor", varMaterial.multiDiffuseColor);
						ReplaceMaterialParam(material, "ps_specularColor", varMaterial.specularColor);
						ReplaceMaterialParam(material, "ps_multiSpecularColor", varMaterial.multiSpecularColor);
						ReplaceMaterialParam(material, "ps_reflectivity", varMaterial.specularColor);
						ReplaceMaterialParam(material, "ps_multiReflectivity", varMaterial.multiSpecularColor);
						ReplaceMaterialParam(material, "ps_shininess", varMaterial.shininess);
						ReplaceMaterialParam(material, "ps_multiShininess", varMaterial.multiShininess);
						mesh->SetActivePolyGroups(var.polyGroupState);
					}
				}
			}

			model->SetLocalTexture(textureResource);
		}
	}
	catch(...)
	{
		modelsToAdd.clear();
	}

	for(const auto& model : modelsToAdd)
	{
		AppendChild(model);
	}
}

UmbralModelPtr CUmbralActor::CreateModel(const ModelChunkPtr& modelChunk)
{
	auto boundingBox = modelChunk->SelectNode<CCompChunk>();

	CVector3 boxMin(boundingBox->GetMinX(), boundingBox->GetMinY(), boundingBox->GetMinZ());
	CVector3 boxMax(boundingBox->GetMaxX(), boundingBox->GetMaxY(), boundingBox->GetMaxZ());

	CVector3 modelSize = (boxMax - boxMin) / 2;
	CVector3 modelPos = (boxMax + boxMin) / 2;

	auto model = std::make_shared<CUmbralModel>(modelChunk);
	model->SetPosition(modelPos);
	model->SetScale(modelSize);

	auto modelBoundingSphere = model->GetBoundingSphere();
	modelBoundingSphere.radius *= std::max(std::max(modelSize.x, modelSize.y), modelSize.z);
	modelBoundingSphere.position += modelPos;
	m_boundingSphere = m_boundingSphere.Accumulate(modelBoundingSphere);

	return model;
}

uint32 CUmbralActor::ComputeVarEquipId(ACTOR_TYPE actorType, SUBMODEL_TYPE submodelType, uint32 subModelId, uint32 textureId, uint32 variation)
{
	uint32 result = 0;
	switch(actorType)
	{
	case ACTOR_PC:
		{
			result = 100000000;
			result += subModelId * 100000;
			result += textureId * 100;
			result += variation;
			switch(submodelType)
			{
			case SUBMODEL_TOP:
				result += 10000;
				break;
			case SUBMODEL_BOTTOM:
				result += 20000;
				break;
			case SUBMODEL_GLOVE:
				result += 30000;
				break;
			case SUBMODEL_SHOE:
				result += 40000;
				break;
			case SUBMODEL_HELM:
				result += 60000;
				break;
			default:
				assert(0);
				break;
			}
		}
		break;
	default:
		assert(0);
		break;
	}
	return result;
}

uint32 CUmbralActor::ComputeVarWepId(uint32 modelFolder, uint32 subModelId, uint32 textureId, uint32 variation)
{
	uint32 result = 1000000000 + (modelFolder * 1000000) + (subModelId * 1000) + (textureId * 32) + variation;
	return result;
}
