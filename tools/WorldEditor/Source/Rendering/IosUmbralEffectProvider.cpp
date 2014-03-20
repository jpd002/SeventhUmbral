#include <assert.h>
#include "UmbralMesh.h"
#include "IosUmbralEffectProvider.h"

Athena::EffectPtr CIosUmbralEffectProvider::CreateEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader)
{
	return std::make_shared<CIosUmbralEffect>(vertexShader, pixelShader);
}
