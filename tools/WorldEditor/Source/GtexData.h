#pragma once

#include "Stream.h"
#include "ResourceNode.h"

class CGtexData : public CResourceNode
{
public:
	enum TEXTURE_FORMAT
	{
		TEXTURE_FORMAT_NULL			= 0x00,
		TEXTURE_FORMAT_A8R8G8B8		= 0x03,
		TEXTURE_FORMAT_X8R8G8B8		= 0x04,
		TEXTURE_FORMAT_DXT1			= 0x18,
		TEXTURE_FORMAT_DXT3			= 0x19,
		TEXTURE_FORMAT_DXT5			= 0x1A
	};

	struct MIPMAPINFO
	{
		uint32		offset;
		uint32		length;
	};
	typedef std::vector<MIPMAPINFO> MipMapInfoArray;

							CGtexData();
	virtual					~CGtexData();

	void					Read(Framework::CStream&);
	void					ReadSurfaces(Framework::CStream&);

	TEXTURE_FORMAT			GetTextureFormat() const;
	uint32					GetTextureWidth() const;
	uint32					GetTextureHeight() const;
	const MipMapInfoArray&	GetMipMapInfos() const;

	uint8*					GetTextureData() const;

private:
	uint32					GetRealDataOffset() const;

	uint32					m_magic;
	uint8					m_unknown1;
	uint8					m_unknown2;
	TEXTURE_FORMAT			m_format;
	uint8					m_mipmapCount;
	uint8					m_unknown3;
	bool					m_isCubeMap;
	uint16					m_width;
	uint16					m_height;
	uint16					m_depth;
	uint32					m_unknown5;
	uint32					m_dataOffset;

	uint8*					m_textureData;

	MipMapInfoArray			m_mipMapInfos;
};
