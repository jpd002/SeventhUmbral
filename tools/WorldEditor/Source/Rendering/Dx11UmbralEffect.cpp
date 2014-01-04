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

	auto vertexShaderConstantTable = vertexShader.GetConstantTable();
	OffsetKeeper constantOffset;
	ParseVertexShaderConstantTable(constantOffset, vertexShaderConstantTable, CD3DShaderConstantTable::REGISTER_SET_FLOAT4);
	ParseVertexShaderConstantTable(constantOffset, vertexShaderConstantTable, CD3DShaderConstantTable::REGISTER_SET_BOOL);
	CreateConstantBuffer(constantOffset.currentOffset);
}

CDx11UmbralEffect::~CDx11UmbralEffect()
{

}

void CDx11UmbralEffect::UpdateConstants(const Athena::MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewProjMatrix, const CMatrix4& shadowViewProjMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	auto worldViewProjMatrix = worldMatrix * viewProjMatrix;
	CVector3 modelBBoxOffset(0, 0, 0);
	CVector3 modelBBoxScale(1, 1, 1);

	auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	*reinterpret_cast<CVector3*>(constantBufferPtr + m_modelBBoxOffsetOffset) = modelBBoxOffset;
	*reinterpret_cast<CVector3*>(constantBufferPtr + m_modelBBoxScaleOffset) = modelBBoxScale;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix.Transpose();
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldViewProjMatrixOffset) = worldViewProjMatrix.Transpose();
	m_deviceContext->Unmap(m_constantBuffer, 0);
}

void CDx11UmbralEffect::ParseVertexShaderConstantTable(OffsetKeeper& constantOffset, 
	const CD3DShaderConstantTable& vertexShaderConstantTable, CD3DShaderConstantTable::REGISTER_SET registerSetFilter)
{
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
		
		if(constant.name == "ModelBBoxOffSet")
		{
			m_modelBBoxOffsetOffset = constantOffset.Allocate(size);
		}
		else if(constant.name == "ModelBBoxScale")
		{
			m_modelBBoxScaleOffset = constantOffset.Allocate(size);
		}
		else if(constant.name == "worldMatrix")
		{
			m_worldMatrixOffset = constantOffset.Allocate(size);
		}
		else if(constant.name == "worldViewProjMatrix")
		{
			m_worldViewProjMatrixOffset = constantOffset.Allocate(size);
		}
		else
		{
			constantOffset.Allocate(size);
		}
	}
}
