#pragma once

#include "UmbralEffectProvider.h"

class CGlEsUmbralEffectProvider : public CUmbralEffectProvider
{
public:
	
protected:
	virtual Palleon::EffectPtr	CreateEffect(const CD3DShader&, const CD3DShader&, bool) override;
	
private:
	
};
