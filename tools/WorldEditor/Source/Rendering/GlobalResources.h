#pragma once

#include "Singleton.h"
#include "AthenaEngine.h"
#include "../ResourceDefs.h"

class CGlobalResources : public CSingleton<CGlobalResources>
{
public:
								CGlobalResources();
	virtual						~CGlobalResources();

	void						Initialize();
	void						Release();

	Athena::TexturePtr			GetTexture(const std::string&);
	static Athena::TexturePtr	CreateTextureFromGtex(const GtexDataPtr&);

	Athena::TexturePtr			GetDiffuseMapTexture() const;
	Athena::TexturePtr			GetSkyTexture() const;
	Athena::TexturePtr			GetProxyShadowTexture() const;

	Athena::EffectProviderPtr	GetEffectProvider() const;

private:
	typedef std::map<std::string, Athena::TexturePtr> TextureMap;

	TextureMap					m_textures;
	Athena::TexturePtr			m_diffuseMapTexture;
	Athena::TexturePtr			m_skyTexture;
	Athena::TexturePtr			m_proxyShadowTexture;
	Athena::EffectProviderPtr	m_effectProvider;
};
