#include "GlobalResources.h"
#include "../ResourceManager.h"

#ifdef WIN32
#include "Dx11UmbralEffectProvider.h"
#include "palleon/win32/Dx11GraphicDevice.h"
#elif defined(__APPLE__)
#include "IosUmbralEffectProvider.h"
#endif

CGlobalResources::CGlobalResources()
{

}

CGlobalResources::~CGlobalResources()
{

}

void CGlobalResources::Initialize()
{
	{
		auto diffuseMapTexture = Palleon::CGraphicDevice::GetInstance().CreateCubeTexture(Palleon::TEXTURE_FORMAT_RGB888, 32);
		for(unsigned int i = 0; i < 6; i++)
		{
			std::vector<uint8> pixels(32 * 32 * 3, 0xCC + i);
			diffuseMapTexture->UpdateCubeFace(static_cast<Palleon::TEXTURE_CUBE_FACE>(i), pixels.data());
		}
		m_diffuseMapTexture = diffuseMapTexture;
	}

	auto skyTexturePath = Palleon::CResourceManager::GetInstance().MakeResourcePath("global/skybox.dds");
	m_skyTexture = Palleon::CTextureLoader::CreateCubeTextureFromFile(skyTexturePath);
	m_proxyShadowTexture = Palleon::CGraphicDevice::GetInstance().CreateTexture(Palleon::TEXTURE_FORMAT_RGBA8888, 32, 32, 1);

#ifdef WIN32
	auto& graphicDevice = static_cast<Palleon::CDx11GraphicDevice&>(Palleon::CGraphicDevice::GetInstance());
	m_effectProvider = std::make_shared<CDx11UmbralEffectProvider>(graphicDevice.GetDevice(), graphicDevice.GetDeviceContext());
#elif defined(__APPLE__)
	m_effectProvider = std::make_shared<CIosUmbralEffectProvider>();
#endif
}

void CGlobalResources::Release()
{
	m_textures.clear();
	m_diffuseMapTexture.reset();
	m_skyTexture.reset();
	m_proxyShadowTexture.reset();
	m_effectProvider.reset();
}

Palleon::TexturePtr CGlobalResources::GetTexture(const std::string& textureName)
{
	auto textureIterator = m_textures.find(textureName);
	if(textureIterator != std::end(m_textures)) return textureIterator->second;

	auto textureResource = CResourceManager::GetInstance().GetResource(textureName);
	if(!textureResource) return Palleon::TexturePtr();

	auto textureDataInfo = textureResource->SelectNode<CGtexData>();
	assert(textureDataInfo);

	auto texture = CreateTextureFromGtex(textureDataInfo);

	m_textures.insert(std::make_pair(textureName, texture));
	return texture;
}

Palleon::TexturePtr CGlobalResources::CreateTextureFromGtex(const GtexDataPtr& textureDataInfo)
{
	auto textureFormat = textureDataInfo->GetTextureFormat();
	auto textureWidth = textureDataInfo->GetTextureWidth();
	auto textureHeight = textureDataInfo->GetTextureHeight();
	Palleon::TEXTURE_FORMAT specTextureFormat = Palleon::TEXTURE_FORMAT_UNKNOWN;
	switch(textureFormat)
	{
	case CGtexData::TEXTURE_FORMAT_DXT1:
		specTextureFormat = Palleon::TEXTURE_FORMAT_DXT1;
		break;
	case CGtexData::TEXTURE_FORMAT_DXT3:
		specTextureFormat = Palleon::TEXTURE_FORMAT_DXT3;
		break;
	case CGtexData::TEXTURE_FORMAT_DXT5:
		specTextureFormat = Palleon::TEXTURE_FORMAT_DXT5;
		break;
	case CGtexData::TEXTURE_FORMAT_A8R8G8B8:
	case CGtexData::TEXTURE_FORMAT_X8R8G8B8:
		specTextureFormat = Palleon::TEXTURE_FORMAT_RGBA8888;
		break;
	default:
		assert(0);
		break;
	}

	unsigned int mipCount = textureDataInfo->GetMipMapInfos().size();
	auto texture = Palleon::CGraphicDevice::GetInstance().CreateTexture(specTextureFormat, textureWidth, textureHeight, mipCount);
	for(unsigned int i = 0; i < mipCount; i++)
	{
		const auto mipData = textureDataInfo->GetMipMapData(i);
		texture->Update(i, mipData);
	}
	return texture;
}

Palleon::TexturePtr CGlobalResources::GetDiffuseMapTexture() const
{
	return m_diffuseMapTexture;
}

Palleon::TexturePtr CGlobalResources::GetSkyTexture() const
{
	return m_skyTexture;
}

Palleon::TexturePtr CGlobalResources::GetProxyShadowTexture() const
{
	return m_proxyShadowTexture;
}

Palleon::EffectProviderPtr CGlobalResources::GetEffectProvider() const
{
	return m_effectProvider;
}
