#include "GlobalResources.h"
#include "../ResourceManager.h"
#include "Dx11UmbralEffectProvider.h"
#include "athena/win32/Dx11GraphicDevice.h"

CGlobalResources::CGlobalResources()
{

}

CGlobalResources::~CGlobalResources()
{

}

void CGlobalResources::Initialize()
{
	{
		auto diffuseMapTexture = Athena::CGraphicDevice::GetInstance().CreateCubeTexture(Athena::TEXTURE_FORMAT_RGB888, 32);
		for(unsigned int i = 0; i < 6; i++)
		{
			std::vector<uint8> pixels(32 * 32 * 3, 0xCC + i);
			diffuseMapTexture->UpdateCubeFace(static_cast<Athena::TEXTURE_CUBE_FACE>(i), pixels.data());
		}
		m_diffuseMapTexture = diffuseMapTexture;
	}

	m_skyTexture = Athena::CGraphicDevice::GetInstance().CreateCubeTextureFromFile("./data/global/skybox.dds");
	m_proxyShadowTexture = Athena::CGraphicDevice::GetInstance().CreateTexture(Athena::TEXTURE_FORMAT_RGBA8888, 32, 32, 1);

	auto& graphicDevice = static_cast<Athena::CDx11GraphicDevice&>(Athena::CGraphicDevice::GetInstance());
	m_effectProvider = std::make_shared<CDx11UmbralEffectProvider>(graphicDevice.GetDevice(), graphicDevice.GetDeviceContext());
}

void CGlobalResources::Release()
{
	m_textures.clear();
	m_diffuseMapTexture.reset();
	m_skyTexture.reset();
	m_proxyShadowTexture.reset();
	m_effectProvider.reset();
}

Athena::TexturePtr CGlobalResources::GetTexture(const std::string& textureName)
{
	auto textureIterator = m_textures.find(textureName);
	if(textureIterator != std::end(m_textures)) return textureIterator->second;

	auto textureResource = CResourceManager::GetInstance().GetResource(textureName);
	if(!textureResource) return Athena::TexturePtr();

	auto textureDataInfo = textureResource->SelectNode<CGtexData>();
	assert(textureDataInfo);

	auto texture = CreateTextureFromGtex(textureDataInfo);

	m_textures.insert(std::make_pair(textureName, texture));
	return texture;
}

Athena::TexturePtr CGlobalResources::CreateTextureFromGtex(const GtexDataPtr& textureDataInfo)
{
	auto textureFormat = textureDataInfo->GetTextureFormat();
	auto textureWidth = textureDataInfo->GetTextureWidth();
	auto textureHeight = textureDataInfo->GetTextureHeight();
	Athena::TEXTURE_FORMAT specTextureFormat = Athena::TEXTURE_FORMAT_UNKNOWN;
	switch(textureFormat)
	{
	case CGtexData::TEXTURE_FORMAT_DXT1:
		specTextureFormat = Athena::TEXTURE_FORMAT_DXT1;
		break;
	case CGtexData::TEXTURE_FORMAT_DXT3:
		specTextureFormat = Athena::TEXTURE_FORMAT_DXT3;
		break;
	case CGtexData::TEXTURE_FORMAT_DXT5:
		specTextureFormat = Athena::TEXTURE_FORMAT_DXT5;
		break;
	case CGtexData::TEXTURE_FORMAT_A8R8G8B8:
	case CGtexData::TEXTURE_FORMAT_X8R8G8B8:
		specTextureFormat = Athena::TEXTURE_FORMAT_RGBA8888;
		break;
	default:
		assert(0);
		break;
	}

	unsigned int mipCount = textureDataInfo->GetMipMapInfos().size();
	auto texture = Athena::CGraphicDevice::GetInstance().CreateTexture(specTextureFormat, textureWidth, textureHeight, mipCount);
	for(unsigned int i = 0; i < mipCount; i++)
	{
		const auto mipData = textureDataInfo->GetMipMapData(i);
		texture->Update(i, mipData);
	}
	return texture;
}

Athena::TexturePtr CGlobalResources::GetDiffuseMapTexture() const
{
	return m_diffuseMapTexture;
}

Athena::TexturePtr CGlobalResources::GetSkyTexture() const
{
	return m_skyTexture;
}

Athena::TexturePtr CGlobalResources::GetProxyShadowTexture() const
{
	return m_proxyShadowTexture;
}

Athena::EffectProviderPtr CGlobalResources::GetEffectProvider() const
{
	return m_effectProvider;
}
