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

	OffsetKeeper constantOffset;

	auto vertexShaderConstantTable = vertexShader.GetConstantTable();
	for(const auto& constant : vertexShaderConstantTable.GetConstants())
	{
		uint32 size = 0;
		switch(constant.typeInfo.typeClass)
		{
		case CD3DShaderConstantTable::CONSTANT_CLASS_MATRIX_COLS:
			assert(constant.typeInfo.rows == 4 && constant.typeInfo.columns == 4);
			size = 0x40;
			break;
		default:
			assert(0);
			break;
		}

		if(constant.name == "c_worldMatrix")
		{
			m_worldMatrixOffset = constantOffset.Allocate(size);
		}
		else if(constant.name == "c_viewProjMatrix")
		{
			m_viewProjMatrixOffset = constantOffset.Allocate(size);
		}
		else
		{
			constantOffset.Allocate(size);
		}
	}

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

	auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewProjMatrixOffset) = viewProjMatrix;
	m_deviceContext->Unmap(m_constantBuffer, 0);
}
