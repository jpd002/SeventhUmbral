#pragma once

#include "Dx11UmbralEffect.h"
#include "UmbralEffectProvider.h"

class CDx11UmbralEffectProvider : public CUmbralEffectProvider
{
public:
								CDx11UmbralEffectProvider(ID3D11Device*, ID3D11DeviceContext*);
	virtual						~CDx11UmbralEffectProvider();

protected:
	virtual Palleon::EffectPtr	CreateEffect(const CD3DShader&, const CD3DShader&) override;

private:
	ID3D11Device*				m_device;
	ID3D11DeviceContext*		m_deviceContext;
};
