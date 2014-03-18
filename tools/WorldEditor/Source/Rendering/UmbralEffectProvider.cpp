#include <assert.h>
#include "UmbralMesh.h"
#include "UmbralEffectProvider.h"

Athena::EffectPtr CUmbralEffectProvider::GetEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader)
{
	EFFECTINFO effectInfo;
	effectInfo.vertexShader = vertexShader;
	effectInfo.pixelShader = pixelShader;
	auto effectIterator = m_effects.find(effectInfo);
	if(effectIterator != m_effects.end())
	{
		return effectIterator->second;
	}
	auto effect = CreateEffect(vertexShader, pixelShader);
	m_effects.insert(std::make_pair(effectInfo, effect));
	return effect;
}

Athena::EffectPtr CUmbralEffectProvider::GetEffectForRenderable(Athena::CMesh* mesh, bool hasShadowMap)
{
	assert(dynamic_cast<CUmbralMesh*>(mesh));
	auto umbralMesh = static_cast<CUmbralMesh*>(mesh);
	return umbralMesh->GetEffect();
}
