#include <assert.h>
#include "UmbralMesh.h"
#include "Dx11UmbralEffectProvider.h"

CDx11UmbralEffectProvider::CDx11UmbralEffectProvider(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
: m_device(device)
, m_deviceContext(deviceContext)
{

}

CDx11UmbralEffectProvider::~CDx11UmbralEffectProvider()
{

}

Athena::EffectPtr CDx11UmbralEffectProvider::GetEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader)
{
	EFFECTINFO effectInfo;
	effectInfo.vertexShader = vertexShader;
	effectInfo.pixelShader = pixelShader;
	auto effectIterator = m_effects.find(effectInfo);
	if(effectIterator != m_effects.end())
	{
		return effectIterator->second;
	}
	auto effect = std::make_shared<CDx11UmbralEffect>(m_device, m_deviceContext, vertexShader, pixelShader);
	m_effects.insert(std::make_pair(effectInfo, effect));
	return effect;
}

Athena::EffectPtr CDx11UmbralEffectProvider::GetEffectForRenderable(Athena::CMesh* mesh, bool hasShadowMap)
{
	assert(dynamic_cast<CUmbralMesh*>(mesh));
	auto umbralMesh = static_cast<CUmbralMesh*>(mesh);
	return umbralMesh->GetEffect();
}
