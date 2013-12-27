#include "Dx11UmbralEffectProvider.h"

CDx11UmbralEffectProvider::CDx11UmbralEffectProvider(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	const CD3DShader& vertexShader, const CD3DShader& pixelShader)
{
	m_effect = std::make_shared<CDx11UmbralEffect>(device, deviceContext, vertexShader, pixelShader);
}

CDx11UmbralEffectProvider::~CDx11UmbralEffectProvider()
{

}

Athena::EffectPtr CDx11UmbralEffectProvider::GetEffectForRenderable(Athena::CMesh*, bool hasShadowMap)
{
	return m_effect;
}
