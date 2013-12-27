#pragma once

#include "AthenaEngine.h"
#include "D3DShader.h"
#include "Dx11UmbralEffect.h"

class CDx11UmbralEffectProvider : public Athena::CEffectProvider
{
public:
								CDx11UmbralEffectProvider(ID3D11Device*, ID3D11DeviceContext*, const CD3DShader&, const CD3DShader&);
	virtual						~CDx11UmbralEffectProvider();

	virtual Athena::EffectPtr	GetEffectForRenderable(Athena::CMesh*, bool) override;

private:
	Athena::Dx11EffectPtr		m_effect;
};
