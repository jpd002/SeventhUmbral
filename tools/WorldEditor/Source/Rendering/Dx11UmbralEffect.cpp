#include "Dx11UmbralEffect.h"
#include "Dx11UmbralEffectGenerator.h"

CDx11UmbralEffect::CDx11UmbralEffect(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
	const CD3DShader& vertexShader, const CD3DShader& pixelShader)
: CDx11Effect(device, deviceContext)
, m_vertexShader(vertexShader)
, m_pixelShader(pixelShader)
{
	auto vertexShaderText = CDx11UmbralEffectGenerator::GenerateVertexShader(m_vertexShader);
	auto pixelShaderText = CDx11UmbralEffectGenerator::GeneratePixelShader(m_vertexShader, m_pixelShader);

	CompileVertexShader(vertexShaderText);
	CompilePixelShader(pixelShaderText);

	{
		auto vertexShaderConstantTable = vertexShader.GetConstantTable();
		OffsetKeeper constantOffset;
		ParseVertexShaderConstantTable(constantOffset, vertexShaderConstantTable, CD3DShaderConstantTable::REGISTER_SET_FLOAT4);
		ParseVertexShaderConstantTable(constantOffset, vertexShaderConstantTable, CD3DShaderConstantTable::REGISTER_SET_BOOL);
		CreateVertexConstantBuffer(constantOffset.currentOffset);
	}

	{
		auto pixelShaderConstantTable = pixelShader.GetConstantTable();
		OffsetKeeper constantOffset;
		ParsePixelShaderConstantTable(constantOffset, pixelShaderConstantTable, CD3DShaderConstantTable::REGISTER_SET_FLOAT4);
		CreatePixelConstantBuffer(constantOffset.currentOffset);
	}
}

CDx11UmbralEffect::~CDx11UmbralEffect()
{

}

template <typename ParamType> 
void SetParamValue(uint8* constantBufferPtr, uint32 paramOffset, const ParamType& value)
{
	if(paramOffset != -1)
	{
		*reinterpret_cast<ParamType*>(constantBufferPtr + paramOffset) = value;
	}
}

template <typename ParamType>
ParamType GetMaterialEffectParamOrDefault(const Athena::MaterialPtr& material, const std::string& paramName, uint32 paramOffset, const ParamType& defaultValue);

template <>
float GetMaterialEffectParamOrDefault<float>(const Athena::MaterialPtr& material, const std::string& paramName, uint32 paramOffset, const float& defaultValue)
{
	auto effectParam = material->GetEffectParameter(paramName);
	assert((paramOffset == -1) || !effectParam.IsNull());
	if(effectParam.IsNull()) return defaultValue;
	return effectParam.GetScalar();
}

template <>
CVector3 GetMaterialEffectParamOrDefault<CVector3>(const Athena::MaterialPtr& material, const std::string& paramName, uint32 paramOffset, const CVector3& defaultValue)
{
	auto effectParam = material->GetEffectParameter(paramName);
	assert((paramOffset == -1) || !effectParam.IsNull());
	if(effectParam.IsNull()) return defaultValue;
	return effectParam.GetVector3();
}

template <>
CVector4 GetMaterialEffectParamOrDefault<CVector4>(const Athena::MaterialPtr& material, const std::string& paramName, uint32 paramOffset, const CVector4& defaultValue)
{
	auto effectParam = material->GetEffectParameter(paramName);
	assert((paramOffset == -1) || !effectParam.IsNull());
	if(effectParam.IsNull()) return defaultValue;
	if(effectParam.IsVector3())
	{
		return CVector4(effectParam.GetVector3(), 0);
	}
	else
	{
		return effectParam.GetVector4();
	}
}

void CDx11UmbralEffect::UpdateConstants(const Athena::MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewMatrix, const CMatrix4& projMatrix, 
	const CMatrix4& shadowViewProjMatrix)
{
	//Update vertex shader params
	{
		auto worldITMatrix = worldMatrix.Inverse().Transpose();
		auto viewITMatrix = viewMatrix.Inverse().Transpose();
		auto worldViewMatrix = worldMatrix * viewMatrix;
		auto worldViewProjMatrix = worldViewMatrix * projMatrix;
		CVector3 modelBBoxOffset(0, 0, 0);
		CVector3 modelBBoxScale(1, 1, 1);

		auto vertexOcclusionScale = GetMaterialEffectParamOrDefault(material, "vs_vertexOcclusionScale", m_vertexOcclusionScaleOffset, 0.0f);
		auto vertexColorBias = GetMaterialEffectParamOrDefault(material, "vs_vertexColorBias", m_vertexColorBiasOffset, CVector4(0, 0, 0, 0));

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(result));
		auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);

		memset(constantBufferPtr, 0, mappedResource.RowPitch);

		SetParamValue(constantBufferPtr, m_modelBBoxOffsetOffset, modelBBoxOffset);
		SetParamValue(constantBufferPtr, m_modelBBoxScaleOffset, modelBBoxScale);
		//Those two parameters don't give a good result for some reason
//		SetParamValue(constantBufferPtr, m_vertexOcclusionScaleOffset, vertexOcclusionScale);
//		SetParamValue(constantBufferPtr, m_vertexColorBiasOffset, vertexColorBias);
		SetParamValue<uint32>(constantBufferPtr, m_isUseInstancingOffset, 0);
		if(m_viewITMatrixOffset != -1)			*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewITMatrixOffset) = viewITMatrix.Transpose();
		if(m_worldITMatrixOffset != -1)			*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldITMatrixOffset) = worldITMatrix.Transpose();
		if(m_worldMatrixOffset != -1)			*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix.Transpose();
		if(m_worldViewMatrixOffset != -1)		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldViewMatrixOffset) = worldViewMatrix.Transpose();
		if(m_worldViewProjMatrixOffset != -1)	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldViewProjMatrixOffset) = worldViewProjMatrix.Transpose();

		m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
	}

	//Update pixel shader params
	{
		auto modulateColor = GetMaterialEffectParamOrDefault(material, "ps_modulateColor", m_modulateColorOffset, CVector4(1, 1, 1, 1));
		auto ambientColor = GetMaterialEffectParamOrDefault(material, "ps_ambientColor", m_ambientColorOffset, CVector4(0, 0, 0, 0));
		auto diffuseColor = GetMaterialEffectParamOrDefault(material, "ps_diffuseColor", m_diffuseColorOffset, CVector4(0, 0, 0, 0));
		auto specularColor = GetMaterialEffectParamOrDefault(material, "ps_specularColor", m_specularColorOffset, CVector4(0, 0, 0, 0));
		auto shininess = GetMaterialEffectParamOrDefault(material, "ps_shininess", m_shininessOffset, 128.f);
		auto reflectivity = GetMaterialEffectParamOrDefault(material, "ps_reflectivity", m_reflectivityOffset, CVector3(0, 0, 0));
		auto normalPower = GetMaterialEffectParamOrDefault(material, "ps_normalPower", m_normalPowerOffset, 1.f);

		auto multiDiffuseColor = GetMaterialEffectParamOrDefault(material, "ps_multiDiffuseColor", m_multiDiffuseColorOffset, CVector4(0, 0, 0, 0));
		auto multiSpecularColor = GetMaterialEffectParamOrDefault(material, "ps_multiSpecularColor", m_multiSpecularColorOffset, CVector4(0, 0, 0, 0));
		auto multiShininess = GetMaterialEffectParamOrDefault(material, "ps_multiShininess", m_multiShininessOffset, 128.f);
		auto multiReflectivity = GetMaterialEffectParamOrDefault(material, "ps_multiReflectivity", m_multiReflectivityOffset, CVector3(0, 0, 0));
		auto multiNormalPower = GetMaterialEffectParamOrDefault(material, "ps_multiNormalPower", m_multiNormalPowerOffset, 1.f);

		auto fresnelExp = GetMaterialEffectParamOrDefault(material, "ps_fresnelExp", m_fresnelExpOffset, 1.f);
		auto fresnelLightDiffBias = GetMaterialEffectParamOrDefault(material, "ps_fresnelLightDiffBias", m_fresnelLightDiffBiasOffset, 1.f);
		auto lightDiffusePower = GetMaterialEffectParamOrDefault(material, "ps_lightDiffusePower", m_lightDiffusePowerOffset, 0.67f);
		auto lightDiffuseInfluence = GetMaterialEffectParamOrDefault(material, "ps_lightDiffuseInfluence", m_lightDiffuseInfluenceOffset, 0.56f);
		auto reflectMapInfluence = GetMaterialEffectParamOrDefault(material, "ps_reflectMapInfluence", m_reflectMapInfluenceOffset, 0.8f);

		auto glareLdrScale = GetMaterialEffectParamOrDefault(material, "ps_glareLdrScale", m_glareLdrScaleOffset, 1.0f);
		auto refAlphaRestrain = GetMaterialEffectParamOrDefault(material, "ps_refAlphaRestrain", m_refAlphaRestrainOffset, 0.f);
		auto normalVector = GetMaterialEffectParamOrDefault(material, "ps_normalVector", m_normalVectorOffset, CVector4(0, 0, 0, 0));
		auto depthBias = GetMaterialEffectParamOrDefault(material, "ps_depthBias", m_depthBiasOffset, 0.f);

		auto ambientOcclusionColor = GetMaterialEffectParamOrDefault(material, "ps_ambentOcclusionColor", m_ambientOcclusionColorOffset, CVector3(0, 0, 0));
		auto specularOcclusionColor = GetMaterialEffectParamOrDefault(material, "ps_specularOcclusionColor", m_specularOcclusionColorOffset, CVector3(0, 0, 0));
		auto pointLightOcclusionColor = GetMaterialEffectParamOrDefault(material, "ps_pointLightOcclusionColor", m_pointLightOcclusionColorOffset, CVector3(0, 0, 0));
		auto mainLightOcclusionColor = GetMaterialEffectParamOrDefault(material, "ps_mainLightOcclusionColor", m_mainLightOcclusionColorOffset, CVector3(0, 0, 0));
		auto subLightOcclusionColor = GetMaterialEffectParamOrDefault(material, "ps_subLightOcclusionColor", m_subLightOcclusionColorOffset, CVector3(0, 0, 0));
		auto lightMapOcclusionColor = GetMaterialEffectParamOrDefault(material, "ps_lightMapOcclusionColor", m_lightMapOcclusionColorOffset, CVector3(0, 0, 0));
		auto reflectMapOcclusionColor = GetMaterialEffectParamOrDefault(material, "ps_reflectMapOcclusionColor", m_reflectMapOcclusionColorOffset, CVector3(0, 0, 0));

		float lightDiffuseMapLod = 0;
		float reflectMapLod = 0;

		CVector2 pixelClippingDistance(-10000, 10000);
		CVector3 enableShadowFlag(1, 0, 1);
		CColor ambientLightColor(0, 0, 0, 0);
		CVector3 latitudeParam(1, 0, 1);

		CVector3 dirLightDirections[2] = 
		{
			CVector3(0, -5, 1).Normalize(),
			CVector3(0, 0, 0)
		};

		CColor dirLightColors[2] =
		{
			CColor(1, 1, 1, 0),
			CColor(0, 0, 0, 0),
		};

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT result = m_deviceContext->Map(m_pixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(result));
		auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);

		memset(constantBufferPtr, 0, mappedResource.RowPitch);

		SetParamValue(constantBufferPtr, m_pixelClippingDistanceOffset, pixelClippingDistance);
		SetParamValue(constantBufferPtr, m_ambientColorOffset, ambientColor);
		SetParamValue(constantBufferPtr, m_diffuseColorOffset, diffuseColor);
		SetParamValue(constantBufferPtr, m_specularColorOffset, specularColor);
		SetParamValue(constantBufferPtr, m_multiDiffuseColorOffset, multiDiffuseColor);
		SetParamValue(constantBufferPtr, m_multiSpecularColorOffset, multiSpecularColor);
		SetParamValue(constantBufferPtr, m_shininessOffset, shininess);
		SetParamValue(constantBufferPtr, m_multiShininessOffset, multiShininess);
		SetParamValue(constantBufferPtr, m_reflectivityOffset, reflectivity);
		SetParamValue(constantBufferPtr, m_multiReflectivityOffset, multiReflectivity);
		SetParamValue(constantBufferPtr, m_reflectMapInfluenceOffset, reflectMapInfluence);
		SetParamValue(constantBufferPtr, m_reflectMapLodOffset, reflectMapLod);
		SetParamValue(constantBufferPtr, m_normalPowerOffset, normalPower);
		SetParamValue(constantBufferPtr, m_multiNormalPowerOffset, multiNormalPower);
		SetParamValue(constantBufferPtr, m_fresnelExpOffset, fresnelExp);
		SetParamValue(constantBufferPtr, m_fresnelLightDiffBiasOffset, fresnelLightDiffBias);
		SetParamValue(constantBufferPtr, m_lightDiffusePowerOffset, lightDiffusePower);
		SetParamValue(constantBufferPtr, m_lightDiffuseInfluenceOffset, lightDiffuseInfluence);
		SetParamValue(constantBufferPtr, m_lightDiffuseMapLodOffset, lightDiffuseMapLod);
		SetParamValue(constantBufferPtr, m_glareLdrScaleOffset, glareLdrScale);
		SetParamValue(constantBufferPtr, m_normalVectorOffset, normalVector);
		SetParamValue(constantBufferPtr, m_depthBiasOffset, depthBias);
		SetParamValue(constantBufferPtr, m_latitudeParamOffset, latitudeParam);
		SetParamValue(constantBufferPtr, m_modulateColorOffset, modulateColor);
		SetParamValue(constantBufferPtr, m_refAlphaRestrainOffset, refAlphaRestrain);
		SetParamValue(constantBufferPtr, m_ambientLightColorOffset, ambientLightColor);

		SetParamValue(constantBufferPtr, m_ambientOcclusionColorOffset, ambientOcclusionColor);
		SetParamValue(constantBufferPtr, m_specularOcclusionColorOffset, specularOcclusionColor);
		SetParamValue(constantBufferPtr, m_pointLightOcclusionColorOffset, pointLightOcclusionColor);
		SetParamValue(constantBufferPtr, m_mainLightOcclusionColorOffset, mainLightOcclusionColor);
		SetParamValue(constantBufferPtr, m_subLightOcclusionColorOffset, subLightOcclusionColor);
		SetParamValue(constantBufferPtr, m_lightMapOcclusionColorOffset, lightMapOcclusionColor);
		SetParamValue(constantBufferPtr, m_reflectMapOcclusionColorOffset, reflectMapOcclusionColor);

		SetParamValue(constantBufferPtr, m_enableShadowFlagOffset, enableShadowFlag);
		if(m_dirLightDirectionsOffset != -1) reinterpret_cast<CVector3*>(constantBufferPtr + m_dirLightDirectionsOffset)[0] = dirLightDirections[0];
		if(m_dirLightDirectionsOffset != -1) reinterpret_cast<CVector3*>(constantBufferPtr + m_dirLightDirectionsOffset)[1] = dirLightDirections[1];
		if(m_dirLightColorsOffset != -1) reinterpret_cast<CColor*>(constantBufferPtr + m_dirLightColorsOffset)[0] = dirLightColors[0];
		if(m_dirLightColorsOffset != -1) reinterpret_cast<CColor*>(constantBufferPtr + m_dirLightColorsOffset)[1] = dirLightColors[1];

		m_deviceContext->Unmap(m_pixelConstantBuffer, 0);
	}
}

void CDx11UmbralEffect::ParseVertexShaderConstantTable(OffsetKeeper& constantOffset, 
	const CD3DShaderConstantTable& vertexShaderConstantTable, CD3DShaderConstantTable::REGISTER_SET registerSetFilter)
{
	std::map<std::string, uint32&> parameterOffsets =
	{
		{ "ModelBBoxOffSet",		m_modelBBoxOffsetOffset			},
		{ "ModelBBoxScale",			m_modelBBoxScaleOffset			},
		{ "vertexOcclusionScale",	m_vertexOcclusionScaleOffset	},
		{ "vertexColorBias",		m_vertexColorBiasOffset			},
		{ "PointLightColors",		m_pointLightColorsOffset		},
		{ "PointLightParams",		m_pointLightParamsOffset		},
		{ "PointLightPositions",	m_pointLightPositionsOffset		},
		{ "viewITMatrix",			m_viewITMatrixOffset			},
		{ "worldITMatrix",			m_worldITMatrixOffset			},
		{ "worldMatrix",			m_worldMatrixOffset				},
		{ "worldViewMatrix",		m_worldViewMatrixOffset			},
		{ "worldViewProjMatrix",	m_worldViewProjMatrixOffset		},
		{ "isUseInstancing",		m_isUseInstancingOffset			},
	};

	for(const auto& constant : vertexShaderConstantTable.GetConstants())
	{
		if(constant.info.registerSet != registerSetFilter) continue;
		uint32 size = 0;
		switch(constant.typeInfo.typeClass)
		{
		case CD3DShaderConstantTable::CONSTANT_CLASS_SCALAR:
		case CD3DShaderConstantTable::CONSTANT_CLASS_VECTOR:
			size = 0x10 * constant.typeInfo.elements;
			break;
		case CD3DShaderConstantTable::CONSTANT_CLASS_MATRIX_COLS:
			assert(constant.typeInfo.rows == 4 && constant.typeInfo.columns == 4);
			size = 0x40;
			break;
		default:
			assert(0);
			break;
		}
		
		auto parameterOffsetIterator = parameterOffsets.find(constant.name);
		if(parameterOffsetIterator != std::end(parameterOffsets))
		{
			parameterOffsetIterator->second = constantOffset.Allocate(size);
		}
		else
		{
			constantOffset.Allocate(size);
		}
	}
}

void CDx11UmbralEffect::ParsePixelShaderConstantTable(OffsetKeeper& constantOffset, 
	const CD3DShaderConstantTable& pixelShaderConstantTable, CD3DShaderConstantTable::REGISTER_SET registerSetFilter)
{
	std::map<std::string, uint32&> parameterOffsets =
	{
		{ "pixelClippingDistance",		m_pixelClippingDistanceOffset		},
		{ "diffuseColor",				m_diffuseColorOffset				},
		{ "specularColor",				m_specularColorOffset				},
		{ "multiDiffuseColor",			m_multiDiffuseColorOffset			},
		{ "multiSpecularColor",			m_multiSpecularColorOffset			},
		{ "shininess",					m_shininessOffset					},
		{ "multiShininess",				m_multiShininessOffset				},
		{ "reflectivity",				m_reflectivityOffset				},
		{ "multiReflectivity",			m_multiReflectivityOffset			},
		{ "reflectMapInfluence",		m_reflectMapInfluenceOffset			},
		{ "reflectMapLod",				m_reflectMapLodOffset				},
		{ "normalVector",				m_normalVectorOffset				},
		{ "depthBias",					m_depthBiasOffset					},
		{ "normalPower",				m_normalPowerOffset					},
		{ "multiNormalPower",			m_multiNormalPowerOffset			},
		{ "fresnelExp",					m_fresnelExpOffset					},
		{ "fresnelLightDiffBias",		m_fresnelLightDiffBiasOffset		},
		{ "lightDiffusePower",			m_lightDiffusePowerOffset			},
		{ "lightDiffuseInfluence",		m_lightDiffuseInfluenceOffset		},
		{ "lightDiffuseMapLod",			m_lightDiffuseMapLodOffset			},
		{ "glareLdrScale",				m_glareLdrScaleOffset				},
		{ "latitudeParam",				m_latitudeParamOffset				},
		{ "modulateColor",				m_modulateColorOffset				},
		{ "refAlphaRestrain",			m_refAlphaRestrainOffset			},
		{ "ambientColor",				m_ambientColorOffset				},
		{ "ambientLightColor",			m_ambientLightColorOffset			},

		{ "ambentOcclusionColor",		m_ambientOcclusionColorOffset		},
		{ "specularOcclusionColor",		m_specularOcclusionColorOffset		},
		{ "pointLightOcclusionColor",	m_pointLightOcclusionColorOffset	},
		{ "mainLightOcclusionColor",	m_mainLightOcclusionColorOffset		},
		{ "subLightOcclusionColor",		m_subLightOcclusionColorOffset		},
		{ "lightMapOcclusionColor",		m_lightMapOcclusionColorOffset		},
		{ "reflectMapOcclusionColor",	m_reflectMapOcclusionColorOffset	},

		{ "EnableShadowFlag",			m_enableShadowFlagOffset			},
		{ "DirLightDirections",			m_dirLightDirectionsOffset			},
		{ "DirLightColors",				m_dirLightColorsOffset				},
	};

	for(const auto& constant : pixelShaderConstantTable.GetConstants())
	{
		if(constant.info.registerSet != registerSetFilter) continue;
		uint32 size = 0;
		switch(constant.typeInfo.typeClass)
		{
		case CD3DShaderConstantTable::CONSTANT_CLASS_SCALAR:
		case CD3DShaderConstantTable::CONSTANT_CLASS_VECTOR:
			size = 0x10 * constant.typeInfo.elements;
			break;
		case CD3DShaderConstantTable::CONSTANT_CLASS_MATRIX_COLS:
			assert(constant.typeInfo.rows == 4 && constant.typeInfo.columns == 4);
			size = 0x40;
			break;
		default:
			assert(0);
			break;
		}
		
		auto parameterOffsetIterator = parameterOffsets.find(constant.name);
		if(parameterOffsetIterator != std::end(parameterOffsets))
		{
			parameterOffsetIterator->second = constantOffset.Allocate(size);
		}
		else
		{
			constantOffset.Allocate(size);
		}
	}
}

Athena::CDx11Effect::D3D11InputLayoutPtr CDx11UmbralEffect::CreateInputLayout(const Athena::VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementArray;

	InputElementArray inputElements;

	if(const auto& item = descriptor.GetVertexItem(Athena::VERTEX_ITEM_ID_POSITION))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "POSITION";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(Athena::VERTEX_ITEM_ID_NORMAL))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "NORMAL";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(Athena::VERTEX_ITEM_ID_UV0))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(Athena::VERTEX_ITEM_ID_UV1))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 1;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(VERTEX_ITEM_ID_UV2))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 2;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(Athena::VERTEX_ITEM_ID_COLOR))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "COLOR";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);

////////
//TEMPORARY FOR SEVENTH UMBRAL
		inputElement.SemanticName			= "COLOR";
		inputElement.SemanticIndex			= 1;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
//TEMPORARY FOR SEVENTH UMBRAL
////////
	}

	if(const auto& item = descriptor.GetVertexItem(VERTEX_ITEM_ID_TANGENT))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 5;
		inputElement.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	//Used for instancing or skinning, pointing to unrelated data for now
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "BLENDINDICES";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= 0;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 6;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= 0;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 7;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= 0;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	D3D11InputLayoutPtr inputLayout;
	HRESULT result = m_device->CreateInputLayout(inputElements.data(), inputElements.size(), 
		m_vertexShaderCode->GetBufferPointer(), m_vertexShaderCode->GetBufferSize(), &inputLayout);
	assert(SUCCEEDED(result));

	return inputLayout;
}
