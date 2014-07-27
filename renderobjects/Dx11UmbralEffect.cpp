#include "Dx11UmbralEffect.h"
#include "Dx11UmbralEffectGenerator.h"
#include "UmbralEffect.h"
#include "palleon/Viewport.h"

CDx11UmbralEffect::CDx11UmbralEffect(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
	const CD3DShader& vertexShader, const CD3DShader& pixelShader, bool hasAlphaTest)
: CDx11Effect(device, deviceContext)
, m_vertexShader(vertexShader)
, m_pixelShader(pixelShader)
, m_hasAlphaTest(hasAlphaTest)
{
	auto vertexShaderText = CDx11UmbralEffectGenerator::GenerateVertexShader(m_vertexShader);
	auto pixelShaderText = CDx11UmbralEffectGenerator::GeneratePixelShader(m_vertexShader, m_pixelShader, hasAlphaTest);

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

void CDx11UmbralEffect::UpdateConstants(const Palleon::DX11VIEWPORT_PARAMS& viewportParams, Palleon::CMaterial* material, const CMatrix4& worldMatrix)
{
	//Update vertex shader params
	{
		auto worldITMatrix = worldMatrix.Inverse().Transpose();
		auto viewITMatrix = viewportParams.viewMatrix.Inverse().Transpose();
		auto worldViewMatrix = worldMatrix * viewportParams.viewMatrix;
		auto worldViewProjMatrix = worldViewMatrix * viewportParams.projMatrix;
		CVector3 modelBBoxOffset(0, 0, 0);
		CVector3 modelBBoxScale(1, 1, 1);

		auto vertexOcclusionScale = material->GetEffectParamOrDefault("vs_vertexOcclusionScale", 0.0f, m_vertexOcclusionScaleOffset != -1);
		auto vertexColorBias = material->GetEffectParamOrDefault("vs_vertexColorBias", CVector4(0, 0, 0, 0), m_vertexColorBiasOffset != -1);

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(result));
		auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);

		memset(constantBufferPtr, 0, mappedResource.RowPitch);

		SetParamValue(constantBufferPtr, m_modelBBoxOffsetOffset, modelBBoxOffset);
		SetParamValue(constantBufferPtr, m_modelBBoxScaleOffset, modelBBoxScale);
		SetParamValue(constantBufferPtr, m_vertexOcclusionScaleOffset, vertexOcclusionScale);
		SetParamValue(constantBufferPtr, m_vertexColorBiasOffset, vertexColorBias);
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
		auto modulateColor = material->GetEffectParamOrDefault("ps_modulateColor", CVector4(1, 1, 1, 1), m_modulateColorOffset != -1);
		auto ambientColor = material->GetEffectParamOrDefault("ps_ambientColor", CVector4(0, 0, 0, 0), m_ambientColorOffset != -1);
		auto diffuseColor = material->GetEffectParamOrDefault("ps_diffuseColor", CVector4(0, 0, 0, 0), m_diffuseColorOffset != -1);
		auto specularColor = material->GetEffectParamOrDefault("ps_specularColor", CVector4(0, 0, 0, 0), m_specularColorOffset != -1);
		auto shininess = material->GetEffectParamOrDefault("ps_shininess", 128.f, m_shininessOffset != -1);
		auto reflectivity = material->GetEffectParamOrDefault("ps_reflectivity", CVector3(0, 0, 0), m_reflectivityOffset != -1);
		auto normalPower = material->GetEffectParamOrDefault("ps_normalPower", 1.f, m_normalPowerOffset != -1);

		auto multiDiffuseColor = material->GetEffectParamOrDefault("ps_multiDiffuseColor", CVector4(0, 0, 0, 0), m_multiDiffuseColorOffset != -1);
		auto multiSpecularColor = material->GetEffectParamOrDefault("ps_multiSpecularColor", CVector4(0, 0, 0, 0), m_multiSpecularColorOffset != -1);
		auto multiShininess = material->GetEffectParamOrDefault("ps_multiShininess", 128.f, m_multiShininessOffset != -1);
		auto multiReflectivity = material->GetEffectParamOrDefault("ps_multiReflectivity", CVector3(0, 0, 0), m_multiReflectivityOffset != -1);
		auto multiNormalPower = material->GetEffectParamOrDefault("ps_multiNormalPower", 1.f, m_multiNormalPowerOffset != -1);

		auto fresnelExp = material->GetEffectParamOrDefault("ps_fresnelExp", 1.f, m_fresnelExpOffset != -1);
		auto fresnelLightDiffBias = material->GetEffectParamOrDefault("ps_fresnelLightDiffBias", 1.f, m_fresnelLightDiffBiasOffset != -1);
		auto specularInfluence = material->GetEffectParamOrDefault("ps_specularInfluence", 0.f, m_specularInfluenceOffset != -1);
		auto lightDiffusePower = material->GetEffectParamOrDefault("ps_lightDiffusePower", 0.67f, m_lightDiffusePowerOffset != -1);
		auto lightDiffuseInfluence = material->GetEffectParamOrDefault("ps_lightDiffuseInfluence", 0.56f, m_lightDiffuseInfluenceOffset != -1);
		auto reflectMapInfluence = material->GetEffectParamOrDefault("ps_reflectMapInfluence", 0.8f, m_reflectMapInfluenceOffset != -1);

		auto glareLdrScale = material->GetEffectParamOrDefault("ps_glareLdrScale", 1.0f, m_glareLdrScaleOffset != -1);
		auto refAlphaRestrain = material->GetEffectParamOrDefault("ps_refAlphaRestrain", 0.f, m_refAlphaRestrainOffset != -1);
		auto normalVector = material->GetEffectParamOrDefault("ps_normalVector", CVector4(0, 0, 0, 0), m_normalVectorOffset != -1);
		auto depthBias = material->GetEffectParamOrDefault("ps_depthBias", 0.f, m_depthBiasOffset != -1);
		auto velvetParam = material->GetEffectParamOrDefault("ps_velvetParam", CVector2(0, 0), m_velvetParamOffset != -1);

		auto ambientOcclusionColor = material->GetEffectParamOrDefault("ps_ambentOcclusionColor", CVector3(1, 1, 1), m_ambientOcclusionColorOffset != -1);
		auto mainLightOcclusionColor = material->GetEffectParamOrDefault("ps_mainLightOcclusionColor", CVector3(0, 0, 0), m_mainLightOcclusionColorOffset != -1);
		auto subLightOcclusionColor = material->GetEffectParamOrDefault("ps_subLightOcclusionColor", CVector3(0, 0, 0), m_subLightOcclusionColorOffset != -1);
		auto pointLightOcclusionColor = material->GetEffectParamOrDefault("ps_pointLightOcclusionColor", CVector3(0.5f, 0.5f, 0.5f), m_pointLightOcclusionColorOffset != -1);
		auto lightMapOcclusionColor = material->GetEffectParamOrDefault("ps_lightMapOcclusionColor", CVector3(0.3f, 0.3f, 0.3f), m_lightMapOcclusionColorOffset != -1);
		auto reflectMapOcclusionColor = material->GetEffectParamOrDefault("ps_reflectMapOcclusionColor", CVector3(0.3f, 0.3f, 0.3f), m_reflectMapOcclusionColorOffset != -1);
		auto specularOcclusionColor = material->GetEffectParamOrDefault("ps_specularOcclusionColor", CVector3(0, 0, 0), m_specularOcclusionColorOffset != -1);

		float lightDiffuseMapLod = 0;
		float reflectMapLod = 0;

		CVector2 pixelClippingDistance(-10000, 10000);
		CVector3 enableShadowFlag(1, 0, 1);
		CVector3 latitudeParam(1, 0, 1);
		CVector4 ambientLightColor = viewportParams.viewport->GetEffectParamOrDefault("ps_ambientLightColor", CVector4(0.1f, 0.1f, 0.1f, 0));

		CVector3 dirLightDirections[2] = 
		{
			viewportParams.viewport->GetEffectParamOrDefault("ps_dirLightDirection0", CVector3(0, 0, 0)),
			viewportParams.viewport->GetEffectParamOrDefault("ps_dirLightDirection1", CVector3(0, 0, 0))
		};

		CVector4 dirLightColors[2] =
		{
			viewportParams.viewport->GetEffectParamOrDefault("ps_dirLightColor0", CVector4(0, 0, 0, 0)),
			viewportParams.viewport->GetEffectParamOrDefault("ps_dirLightColor1", CVector4(0, 0, 0, 0))
		};

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT result = m_deviceContext->Map(m_pixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(result));
		auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);

		memset(constantBufferPtr, 0, mappedResource.RowPitch);

		SetParamValue(constantBufferPtr, m_pixelClippingDistanceOffset, pixelClippingDistance);

		SetParamValue(constantBufferPtr, m_modulateColorOffset, modulateColor);
		SetParamValue(constantBufferPtr, m_ambientColorOffset, ambientColor);
		SetParamValue(constantBufferPtr, m_diffuseColorOffset, diffuseColor);
		SetParamValue(constantBufferPtr, m_specularColorOffset, specularColor);
		SetParamValue(constantBufferPtr, m_shininessOffset, shininess);
		SetParamValue(constantBufferPtr, m_reflectivityOffset, reflectivity);
		SetParamValue(constantBufferPtr, m_normalPowerOffset, normalPower);

		SetParamValue(constantBufferPtr, m_multiDiffuseColorOffset, multiDiffuseColor);
		SetParamValue(constantBufferPtr, m_multiSpecularColorOffset, multiSpecularColor);
		SetParamValue(constantBufferPtr, m_multiShininessOffset, multiShininess);
		SetParamValue(constantBufferPtr, m_multiReflectivityOffset, multiReflectivity);
		SetParamValue(constantBufferPtr, m_multiNormalPowerOffset, multiNormalPower);

		SetParamValue(constantBufferPtr, m_fresnelExpOffset, fresnelExp);
		SetParamValue(constantBufferPtr, m_fresnelLightDiffBiasOffset, fresnelLightDiffBias);
		SetParamValue(constantBufferPtr, m_specularInfluenceOffset, specularInfluence);
		SetParamValue(constantBufferPtr, m_lightDiffusePowerOffset, lightDiffusePower);
		SetParamValue(constantBufferPtr, m_lightDiffuseInfluenceOffset, lightDiffuseInfluence);
		SetParamValue(constantBufferPtr, m_lightDiffuseMapLodOffset, lightDiffuseMapLod);
		SetParamValue(constantBufferPtr, m_reflectMapInfluenceOffset, reflectMapInfluence);
		SetParamValue(constantBufferPtr, m_reflectMapLodOffset, reflectMapLod);

		SetParamValue(constantBufferPtr, m_glareLdrScaleOffset, glareLdrScale);
		SetParamValue(constantBufferPtr, m_normalVectorOffset, normalVector);
		SetParamValue(constantBufferPtr, m_depthBiasOffset, depthBias);
		SetParamValue(constantBufferPtr, m_refAlphaRestrainOffset, refAlphaRestrain);
		SetParamValue(constantBufferPtr, m_velvetParamOffset, velvetParam);

		SetParamValue(constantBufferPtr, m_ambientOcclusionColorOffset, ambientOcclusionColor);
		SetParamValue(constantBufferPtr, m_specularOcclusionColorOffset, specularOcclusionColor);
		SetParamValue(constantBufferPtr, m_pointLightOcclusionColorOffset, pointLightOcclusionColor);
		SetParamValue(constantBufferPtr, m_mainLightOcclusionColorOffset, mainLightOcclusionColor);
		SetParamValue(constantBufferPtr, m_subLightOcclusionColorOffset, subLightOcclusionColor);
		SetParamValue(constantBufferPtr, m_lightMapOcclusionColorOffset, lightMapOcclusionColor);
		SetParamValue(constantBufferPtr, m_reflectMapOcclusionColorOffset, reflectMapOcclusionColor);

		SetParamValue(constantBufferPtr, m_ambientLightColorOffset, ambientLightColor);
		SetParamValue(constantBufferPtr, m_latitudeParamOffset, latitudeParam);
		SetParamValue(constantBufferPtr, m_enableShadowFlagOffset, enableShadowFlag);
		if(m_dirLightDirectionsOffset != -1) reinterpret_cast<CVector3*>(constantBufferPtr + m_dirLightDirectionsOffset)[0] = dirLightDirections[0];
		if(m_dirLightDirectionsOffset != -1) reinterpret_cast<CVector3*>(constantBufferPtr + m_dirLightDirectionsOffset)[1] = dirLightDirections[1];
		if(m_dirLightColorsOffset != -1) reinterpret_cast<CVector4*>(constantBufferPtr + m_dirLightColorsOffset)[0] = dirLightColors[0];
		if(m_dirLightColorsOffset != -1) reinterpret_cast<CVector4*>(constantBufferPtr + m_dirLightColorsOffset)[1] = dirLightColors[1];

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

		{ "modulateColor",				m_modulateColorOffset				},
		{ "diffuseColor",				m_diffuseColorOffset				},
		{ "specularColor",				m_specularColorOffset				},
		{ "shininess",					m_shininessOffset					},
		{ "reflectivity",				m_reflectivityOffset				},
		{ "normalPower",				m_normalPowerOffset					},

		{ "multiDiffuseColor",			m_multiDiffuseColorOffset			},
		{ "multiSpecularColor",			m_multiSpecularColorOffset			},
		{ "multiShininess",				m_multiShininessOffset				},
		{ "multiReflectivity",			m_multiReflectivityOffset			},
		{ "multiNormalPower",			m_multiNormalPowerOffset			},

		{ "fresnelExp",					m_fresnelExpOffset					},
		{ "fresnelLightDiffBias",		m_fresnelLightDiffBiasOffset		},
		{ "lightDiffusePower",			m_lightDiffusePowerOffset			},
		{ "specularInfluence",			m_specularInfluenceOffset			},
		{ "lightDiffuseInfluence",		m_lightDiffuseInfluenceOffset		},
		{ "lightDiffuseMapLod",			m_lightDiffuseMapLodOffset			},
		{ "reflectMapInfluence",		m_reflectMapInfluenceOffset			},
		{ "reflectMapLod",				m_reflectMapLodOffset				},

		{ "glareLdrScale",				m_glareLdrScaleOffset				},
		{ "normalVector",				m_normalVectorOffset				},
		{ "depthBias",					m_depthBiasOffset					},
		{ "refAlphaRestrain",			m_refAlphaRestrainOffset			},
		{ "velvetParam",				m_velvetParamOffset					},

		{ "ambentOcclusionColor",		m_ambientOcclusionColorOffset		},
		{ "specularOcclusionColor",		m_specularOcclusionColorOffset		},
		{ "pointLightOcclusionColor",	m_pointLightOcclusionColorOffset	},
		{ "mainLightOcclusionColor",	m_mainLightOcclusionColorOffset		},
		{ "subLightOcclusionColor",		m_subLightOcclusionColorOffset		},
		{ "lightMapOcclusionColor",		m_lightMapOcclusionColorOffset		},
		{ "reflectMapOcclusionColor",	m_reflectMapOcclusionColorOffset	},

		{ "ambientColor",				m_ambientColorOffset				},
		{ "ambientLightColor",			m_ambientLightColorOffset			},
		{ "latitudeParam",				m_latitudeParamOffset				},
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

Palleon::CDx11Effect::D3D11InputLayoutPtr CDx11UmbralEffect::CreateInputLayout(const Palleon::VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementArray;

	InputElementArray inputElements;

	const auto& placeholderItem = descriptor.GetVertexItem(CUmbralEffect::VERTEX_ITEM_ID_PLACEHOLDER);

	if(const auto& item = descriptor.GetVertexItem(Palleon::VERTEX_ITEM_ID_POSITION))
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

	if(const auto& item = descriptor.GetVertexItem(Palleon::VERTEX_ITEM_ID_NORMAL))
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

	if(const auto& item = descriptor.GetVertexItem(Palleon::VERTEX_ITEM_ID_UV0))
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

	if(const auto& item = descriptor.GetVertexItem(Palleon::VERTEX_ITEM_ID_UV1))
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
	else
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 1;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= placeholderItem->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(CUmbralEffect::VERTEX_ITEM_ID_UV2))
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
	else
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 2;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= placeholderItem->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(CUmbralEffect::VERTEX_ITEM_ID_UV3))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 3;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}
	else
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 3;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= placeholderItem->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(Palleon::VERTEX_ITEM_ID_COLOR))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "COLOR";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}
	else
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "COLOR";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= placeholderItem->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(CUmbralEffect::VERTEX_ITEM_ID_TANGENT))
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

	//Other unused vertex elements, pointing to placeholder data for now
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "COLOR";
		inputElement.SemanticIndex			= 1;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= placeholderItem->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "BLENDINDICES";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= placeholderItem->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 6;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= placeholderItem->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 7;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= placeholderItem->offset;
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
