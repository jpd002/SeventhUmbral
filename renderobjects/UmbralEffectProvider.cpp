#include <assert.h>
#include "UmbralMesh.h"
#include "UmbralEffectProvider.h"

Palleon::EffectPtr CUmbralEffectProvider::GetEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader, bool hasAlphaTest)
{
	EFFECTINFO effectInfo;
	effectInfo.vertexShader = vertexShader;
	effectInfo.pixelShader = pixelShader;
	effectInfo.hasAlphaTest = hasAlphaTest;
	auto effectIterator = m_effects.find(effectInfo);
	if(effectIterator != m_effects.end())
	{
		return effectIterator->second;
	}
	auto effect = CreateEffect(vertexShader, pixelShader, hasAlphaTest);
	m_effects.insert(std::make_pair(effectInfo, effect));
	return effect;
}

Palleon::EffectPtr CUmbralEffectProvider::GetEffectForRenderable(Palleon::CMesh* mesh, bool hasShadowMap)
{
	assert(dynamic_cast<CUmbralMesh*>(mesh));
	auto umbralMesh = static_cast<CUmbralMesh*>(mesh);
	return umbralMesh->GetEffect();
}
