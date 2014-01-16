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

void CDx11UmbralEffect::UpdateConstants(const Athena::MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewMatrix, const CMatrix4& projMatrix, 
	const CMatrix4& shadowViewProjMatrix)
{
	//Update vertex shader params
	{
		auto worldITMatrix = worldMatrix.Inverse().Transpose();
		auto viewITMatrix = viewMatrix.Inverse().Transpose();
		auto worldViewProjMatrix = worldMatrix * viewMatrix * projMatrix;
		CVector3 modelBBoxOffset(0, 0, 0);
		CVector3 modelBBoxScale(1, 1, 1);

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(result));
		auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);

		*reinterpret_cast<CVector3*>(constantBufferPtr + m_modelBBoxOffsetOffset) = modelBBoxOffset;
		*reinterpret_cast<CVector3*>(constantBufferPtr + m_modelBBoxScaleOffset) = modelBBoxScale;
		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldITMatrixOffset) = worldITMatrix.Transpose();
		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix.Transpose();
		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewITMatrixOffset) = viewITMatrix.Transpose();
		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldViewProjMatrixOffset) = worldViewProjMatrix.Transpose();

		m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
	}

	//Update pixel shader params
	{
		CColor diffuseColor(1, 1, 1, 1);
		CColor multiDiffuseColor(1, 1, 1, 1);
		CColor multiSpecularColor(1, 1, 1, 1);
		float shininess = 12;
		float multiShininess = 12;
		CVector3 reflectivity(0.5f, 0.5f, 0.5f);
		CVector3 multiReflectivity(0.5f, 0.5f, 0.5f);
		float reflectMapInfluence = 0.8f;
		float normalPower = 1;
		float fresnelExp = 1;
		float fresnelLightDiffBias = 1;
		float lightDiffusePower = 0.67f;
		float glareLdrScale = 1;
		CVector3 latitudeParam(1, 0, 1);
		CColor modulateColor(1, 1, 1, 1);
		float refAlphaRestrain = 1;
		CColor ambientColor(0, 0, 0, 0);
		CColor ambientLightColor(0, 0, 0, 0);

		static float t = 0;

		CVector3 dirLightDirections[2] = 
		{
			CVector3(0, 0, -1).Normalize(),
			CVector3(0, 0, 0)
		};

		CColor dirLightColors[2] =
		{
			CColor(1, 1, 1, 0),
			CColor(0, 0, 0, 0)
		};

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT result = m_deviceContext->Map(m_pixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(result));
		auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);

		*reinterpret_cast<CColor*>(constantBufferPtr + m_diffuseColorOffset) = diffuseColor;
		*reinterpret_cast<CColor*>(constantBufferPtr + m_multiDiffuseColorOffset) = multiDiffuseColor;
		*reinterpret_cast<CColor*>(constantBufferPtr + m_multiSpecularColorOffset) = multiSpecularColor;
		*reinterpret_cast<float*>(constantBufferPtr + m_shininessOffset) = shininess;
		*reinterpret_cast<float*>(constantBufferPtr + m_multiShininessOffset) = multiShininess;
		*reinterpret_cast<CVector3*>(constantBufferPtr + m_reflectivityOffset) = reflectivity;
		*reinterpret_cast<CVector3*>(constantBufferPtr + m_multiReflectivityOffset) = multiReflectivity;
		*reinterpret_cast<float*>(constantBufferPtr + m_reflectMapInfluenceOffset) = reflectMapInfluence;
		*reinterpret_cast<float*>(constantBufferPtr + m_normalPowerOffset) = normalPower;
		*reinterpret_cast<float*>(constantBufferPtr + m_fresnelExpOffset) = fresnelExp;
		*reinterpret_cast<float*>(constantBufferPtr + m_fresnelLightDiffBiasOffset) = fresnelLightDiffBias;
		*reinterpret_cast<float*>(constantBufferPtr + m_lightDiffusePowerOffset) = lightDiffusePower;
		*reinterpret_cast<float*>(constantBufferPtr + m_glareLdrScaleOffset) = glareLdrScale;
		*reinterpret_cast<CVector3*>(constantBufferPtr + m_latitudeParamOffset) = latitudeParam;
		*reinterpret_cast<CColor*>(constantBufferPtr + m_modulateColorOffset) = modulateColor;
		*reinterpret_cast<float*>(constantBufferPtr + m_refAlphaRestrainOffset) = refAlphaRestrain;
		*reinterpret_cast<CColor*>(constantBufferPtr + m_ambientColorOffset) = ambientColor;
		*reinterpret_cast<CColor*>(constantBufferPtr + m_ambientLightColorOffset) = ambientLightColor;
		reinterpret_cast<CVector3*>(constantBufferPtr + m_dirLightDirectionsOffset)[0] = dirLightDirections[0];
		reinterpret_cast<CVector3*>(constantBufferPtr + m_dirLightDirectionsOffset)[1] = dirLightDirections[1];
		reinterpret_cast<CColor*>(constantBufferPtr + m_dirLightColorsOffset)[0] = dirLightColors[0];
		reinterpret_cast<CColor*>(constantBufferPtr + m_dirLightColorsOffset)[1] = dirLightColors[1];

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
		{ "worldITMatrix",			m_worldITMatrixOffset			},
		{ "worldMatrix",			m_worldMatrixOffset				},
		{ "viewITMatrix",			m_viewITMatrixOffset			},
		{ "worldViewProjMatrix",	m_worldViewProjMatrixOffset		},
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
		{ "diffuseColor",			m_diffuseColorOffset			},
		{ "multiDiffuseColor",		m_multiDiffuseColorOffset		},
		{ "multiSpecularColor",		m_multiSpecularColorOffset		},
		{ "shininess",				m_shininessOffset				},
		{ "multiShininess",			m_multiShininessOffset			},
		{ "reflectivity",			m_reflectivityOffset			},
		{ "multiReflectivity",		m_multiReflectivityOffset		},
		{ "reflectMapInfluence",	m_reflectMapInfluenceOffset		},
		{ "normalPower",			m_normalPowerOffset				},
		{ "fresnelExp",				m_fresnelExpOffset				},
		{ "fresnelLightDiffBias",	m_fresnelLightDiffBiasOffset	},
		{ "lightDiffusePower",		m_lightDiffusePowerOffset		},
		{ "glareLdrScale",			m_glareLdrScaleOffset			},
		{ "latitudeParam",			m_latitudeParamOffset			},
		{ "modulateColor",			m_modulateColorOffset			},
		{ "refAlphaRestrain",		m_refAlphaRestrainOffset		},
		{ "ambientColor",			m_ambientColorOffset			},
		{ "ambientLightColor",		m_ambientLightColorOffset		},
		{ "DirLightDirections",		m_dirLightDirectionsOffset		},
		{ "DirLightColors",			m_dirLightColorsOffset			},
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
