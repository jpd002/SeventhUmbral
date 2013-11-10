#include "GtexData.h"
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
