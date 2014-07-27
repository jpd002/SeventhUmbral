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

Palleon::EffectPtr CDx11UmbralEffectProvider::CreateEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader, bool hasAlphaTest)
{
	return std::make_shared<CDx11UmbralEffect>(m_device, m_deviceContext, vertexShader, pixelShader, hasAlphaTest);
}
