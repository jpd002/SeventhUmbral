#pragma once

#include "BaseChunk.h"

class CStreamChunk : public CBaseChunk
{
public:
	enum ELEMENT_DATA_TYPE
	{
		ELEMENT_DATA_TYPE_POSITION		= 0,
		ELEMENT_DATA_TYPE_NORMAL		= 2,
		ELEMENT_DATA_TYPE_COLOR			= 3,
		ELEMENT_DATA_TYPE_BINORMAL		= 4,
		ELEMENT_DATA_TYPE_UV1			= 8,
		ELEMENT_DATA_TYPE_UV2			= 9,
		ELEMENT_DATA_TYPE_UV3			= 10,
		ELEMENT_DATA_TYPE_UV4			= 11,
		ELEMENT_DATA_TYPE_TANGENT		= 13,
		ELEMENT_DATA_TYPE_BONE_WEIGHT	= 14,
		ELEMENT_DATA_TYPE_BONE_INDEX	= 15,
		ELEMENT_DATA_TYPE_INDEX			= 255
	};

	enum ELEMENT_DATA_FORMAT
	{
		ELEMENT_DATA_FORMAT_UINT16	= 0,
		ELEMENT_DATA_FORMAT_FLOAT	= 1,
		ELEMENT_DATA_FORMAT_HALF	= 2,
		ELEMENT_DATA_FORMAT_BYTE	= 3,
		ELEMENT_DATA_FORMAT_INT16	= 4,
		ELEMENT_DATA_FORMAT_SBYTE	= 6
	};

	struct ELEMENT
	{
		uint16					unknown1;
		uint16					offsetInVertex;
		ELEMENT_DATA_FORMAT		dataFormat;
		uint32					numElements;
		ELEMENT_DATA_TYPE		dataType;
		uint16					unknown2;
	};

						CStreamChunk();
	virtual				~CStreamChunk();

	virtual void		Read(Framework::CStream&) override;

	uint32				GetVertexCount() const;
	uint32				GetVertexSize() const;
	const uint8*		GetData() const;

	const ELEMENT*		FindElement(ELEMENT_DATA_TYPE) const;

private:
	typedef std::vector<ELEMENT> ElementArray;

	void				ReadElements(Framework::CStream&);
	void				ReadStream(Framework::CStream&);
	
	static uint32		GetElementDataSize(ELEMENT_DATA_FORMAT);

	uint32				m_numElements;
	uint32				m_vertexCount;
	uint32				m_bytesPerVertex;
	uint32				m_unknown1;

	ElementArray		m_elements;
	std::vector<uint8>	m_data;
};

typedef std::shared_ptr<CStreamChunk> StreamChunkPtr;
