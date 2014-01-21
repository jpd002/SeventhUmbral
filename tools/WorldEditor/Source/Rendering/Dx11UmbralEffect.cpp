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

		SetParamValue<CVector3>(constantBufferPtr, m_modelBBoxOffsetOffset, modelBBoxOffset);
		SetParamValue<CVector3>(constantBufferPtr, m_modelBBoxScaleOffset, modelBBoxScale);
		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldITMatrixOffset) = worldITMatrix.Transpose();
		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix.Transpose();
		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewITMatrixOffset) = viewITMatrix.Transpose();
		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldViewProjMatrixOffset) = worldViewProjMatrix.Transpose();

		m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
	}

	//Update pixel shader params
	{
		CVector2 pixelClippingDistance(-100, 100);
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
		CColor ambientLightColor(0.75f, 0.75f, 0.75f, 0);
		float ambientOcclusionColor = 1;
		float mainLightOcclusionColor = 1;
		float subLightOcclusionColor = 1;

		CVector3 dirLightDirections[2] = 
		{
			CVector3(1, 0, 1).Normalize(),
			CVector3(0, 0, 0)
		};

		CColor dirLightColors[2] =
		{
			CColor(1, 1, 1, 0),
			CColor(1, 1, 1, 0)
		};

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		HRESULT result = m_deviceContext->Map(m_pixelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(result));
		auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);

		SetParamValue<CVector2>(constantBufferPtr, m_pixelClippingDistanceOffset, pixelClippingDistance);
		SetParamValue<CColor>(constantBufferPtr, m_diffuseColorOffset, diffuseColor);
		SetParamValue<CColor>(constantBufferPtr, m_multiDiffuseColorOffset, multiDiffuseColor);
		SetParamValue<CColor>(constantBufferPtr, m_multiSpecularColorOffset, multiSpecularColor);
		SetParamValue<float>(constantBufferPtr, m_shininessOffset, shininess);
		SetParamValue<float>(constantBufferPtr, m_multiShininessOffset, multiShininess);
		SetParamValue<CVector3>(constantBufferPtr, m_reflectivityOffset, reflectivity);
		SetParamValue<CVector3>(constantBufferPtr, m_multiReflectivityOffset, multiReflectivity);
		SetParamValue<float>(constantBufferPtr, m_reflectMapInfluenceOffset, reflectMapInfluence);
		SetParamValue<float>(constantBufferPtr, m_normalPowerOffset, normalPower);
		SetParamValue<float>(constantBufferPtr, m_fresnelExpOffset, fresnelExp);
		SetParamValue<float>(constantBufferPtr, m_fresnelLightDiffBiasOffset, fresnelLightDiffBias);
		SetParamValue<float>(constantBufferPtr, m_lightDiffusePowerOffset, lightDiffusePower);
		SetParamValue<float>(constantBufferPtr, m_glareLdrScaleOffset, glareLdrScale);
		SetParamValue<CVector3>(constantBufferPtr, m_latitudeParamOffset, latitudeParam);
		SetParamValue<CColor>(constantBufferPtr, m_modulateColorOffset, modulateColor);
		SetParamValue<float>(constantBufferPtr, m_refAlphaRestrainOffset, refAlphaRestrain);
		SetParamValue<CColor>(constantBufferPtr, m_ambientColorOffset, ambientColor);
		SetParamValue<CColor>(constantBufferPtr, m_ambientLightColorOffset, ambientLightColor);
		SetParamValue<float>(constantBufferPtr, m_ambientOcclusionColorOffset, ambientOcclusionColor);
		SetParamValue<float>(constantBufferPtr, m_mainLightOcclusionColorOffset, mainLightOcclusionColor);
		SetParamValue<float>(constantBufferPtr, m_subLightOcclusionColorOffset, subLightOcclusionColor);
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
		{ "pixelClippingDistance",		m_pixelClippingDistanceOffset	},
		{ "diffuseColor",				m_diffuseColorOffset			},
		{ "multiDiffuseColor",			m_multiDiffuseColorOffset		},
		{ "multiSpecularColor",			m_multiSpecularColorOffset		},
		{ "shininess",					m_shininessOffset				},
		{ "multiShininess",				m_multiShininessOffset			},
		{ "reflectivity",				m_reflectivityOffset			},
		{ "multiReflectivity",			m_multiReflectivityOffset		},
		{ "reflectMapInfluence",		m_reflectMapInfluenceOffset		},
		{ "normalPower",				m_normalPowerOffset				},
		{ "fresnelExp",					m_fresnelExpOffset				},
		{ "fresnelLightDiffBias",		m_fresnelLightDiffBiasOffset	},
		{ "lightDiffusePower",			m_lightDiffusePowerOffset		},
		{ "glareLdrScale",				m_glareLdrScaleOffset			},
		{ "latitudeParam",				m_latitudeParamOffset			},
		{ "modulateColor",				m_modulateColorOffset			},
		{ "refAlphaRestrain",			m_refAlphaRestrainOffset		},
		{ "ambientColor",				m_ambientColorOffset			},
		{ "ambientLightColor",			m_ambientLightColorOffset		},
		{ "ambentOcclusionColor",		m_ambientOcclusionColorOffset	},
		{ "mainLightOcclusionColor",	m_mainLightOcclusionColorOffset	},
		{ "subLightOcclusionColor",		m_subLightOcclusionColorOffset	},
		{ "DirLightDirections",			m_dirLightDirectionsOffset		},
		{ "DirLightColors",				m_dirLightColorsOffset			},
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
