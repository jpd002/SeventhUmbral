#pragma once

#include "UmbralEffectProvider.h"
#include "IosUmbralEffect.h"

class CIosUmbralEffectProvider : public CUmbralEffectProvider
{
public:
	
protected:
	virtual Athena::EffectPtr	CreateEffect(const CD3DShader&, const CD3DShader&) override;
	
private:
	
};
