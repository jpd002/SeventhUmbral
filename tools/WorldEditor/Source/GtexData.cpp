#include "GtexData.h"
#include "PwibSection.h"
#include "TextureSection.h"
#include <assert.h>

CGtexData::CGtexData()
: m_magic(0)
, m_unknown1(0)
, m_unknown2(0)
, m_format(TEXTURE_FORMAT_NULL)
, m_mipmapCount(0)
, m_isCubeMap(false)
, m_width(0)
, m_height(0)
, m_depth(0)
, m_unknown5(0)
, m_dataOffset(0)
{

}

CGtexData::~CGtexData()
{

}

void CGtexData::Read(Framework::CStream& inputStream)
{
	m_magic			= inputStream.Read32();
	assert(m_magic = 'XETG');
	m_unknown1		= inputStream.Read8();
	m_unknown2		= inputStream.Read8();
	m_format		= static_cast<TEXTURE_FORMAT>(inputStream.Read8());
	m_mipmapCount	= inputStream.Read8();
	m_unknown3		= inputStream.Read8();
	m_isCubeMap		= inputStream.Read8() != 0;
	m_width			= inputStream.Read16_MSBF();
	m_height		= inputStream.Read16_MSBF();
	m_depth			= inputStream.Read16_MSBF();
	m_unknown5		= inputStream.Read32_MSBF();
	m_dataOffset	= inputStream.Read32_MSBF();

	m_mipMapInfos.resize(m_mipmapCount);
	for(auto& mipMapInfo : m_mipMapInfos)
	{
		mipMapInfo.offset = inputStream.Read32_MSBF();
		mipMapInfo.length = inputStream.Read32_MSBF();
	}
	
	ReadSurfaces(inputStream);
}

void CGtexData::ReadSurfaces(Framework::CStream& inputStream)
{
	uint32 dataOffset = GetRealDataOffset();

	//Humm, is this a better way to compute the offset?
#if 0
	{
		uint32 totalSurfacesSize = 0;
		for(const auto& mipMapInfo : m_mipMapInfos)
		{
			totalSurfacesSize += mipMapInfo.length;
		}

		auto parent = GetParent();
		while(parent)
		{
			if(auto textureSection = std::dynamic_pointer_cast<CTextureSection>(parent))
			{
				uint32 totalSize = textureSection->GetSize();
				dataOffset = totalSize - totalSurfacesSize;
			}
			parent = parent->GetParent();
		}
	}
#endif

	auto currentOffset = inputStream.Tell();

	for(const auto& mipMapInfo : m_mipMapInfos)
	{
		uint32 textureDataOffset = dataOffset + mipMapInfo.offset;
		SurfaceDataArray surface(mipMapInfo.length);
		inputStream.Seek(textureDataOffset, Framework::STREAM_SEEK_SET);
		inputStream.Read(surface.data(), mipMapInfo.length);
		m_surfaces.push_back(std::move(surface));
	}

	inputStream.Seek(currentOffset, Framework::STREAM_SEEK_SET);
}

CGtexData::TEXTURE_FORMAT CGtexData::GetTextureFormat() const
{
	return m_format;
}

uint32 CGtexData::GetTextureWidth() const
{
	return m_width;
}

uint32 CGtexData::GetTextureHeight() const
{
	return m_height;
}

const CGtexData::MipMapInfoArray& CGtexData::GetMipMapInfos() const
{
	return m_mipMapInfos;
}

const uint8* CGtexData::GetMipMapData(unsigned int mipLevel) const
{
	return m_surfaces[mipLevel].data();
}

uint32 CGtexData::GetRealDataOffset() const
{
	if(m_dataOffset != 0)
	{
		return m_dataOffset;
	}

	auto parent = GetParent();
	while(parent)
	{
		if(auto pwibSection = std::dynamic_pointer_cast<CPwibSection>(parent))
		{
			return pwibSection->GetDataOffset();
		}
		parent = parent->GetParent();
	}

	assert(0);
	return 0;
}
