#pragma once

#include "Singleton.h"
#include "PalleonEngine.h"
#include "../dataobjects/ResourceDefs.h"

class CGlobalResources : public CSingleton<CGlobalResources>
{
public:
								CGlobalResources();
	virtual						~CGlobalResources();

	void						Initialize();
	void						Release();

	Palleon::TexturePtr			GetTexture(const std::string&);
	static Palleon::TexturePtr	CreateTextureFromGtex(const GtexDataPtr&);

	Palleon::TexturePtr			GetDiffuseMapTexture() const;
	Palleon::TexturePtr			GetLightToneMapTexture() const;
	Palleon::TexturePtr			GetSkyTexture() const;
	Palleon::TexturePtr			GetProxyShadowTexture() const;

	Palleon::EffectProviderPtr	GetEffectProvider() const;

private:
	typedef std::map<std::string, Palleon::TexturePtr> TextureMap;

	TextureMap					m_textures;
	Palleon::TexturePtr			m_diffuseMapTexture;
	Palleon::TexturePtr			m_lightToneMapTexture;
	Palleon::TexturePtr			m_skyTexture;
	Palleon::TexturePtr			m_proxyShadowTexture;
	Palleon::EffectProviderPtr	m_effectProvider;
};
