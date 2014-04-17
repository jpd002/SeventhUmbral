#include <assert.h>
#include "UmbralMesh.h"
#include "GlEsUmbralEffectProvider.h"
#include "GlEsUmbralEffect.h"

Palleon::EffectPtr CGlEsUmbralEffectProvider::CreateEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader)
{
	return std::make_shared<CGlEsUmbralEffect>(vertexShader, pixelShader);
}
