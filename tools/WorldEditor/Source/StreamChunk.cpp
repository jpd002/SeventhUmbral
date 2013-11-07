#include "StreamChunk.h"
#include <assert.h>
#include <algorithm>

CStreamChunk::CStreamChunk()
: m_numElements(0)
, m_vertexCount(0)
, m_bytesPerVertex(0)
, m_unknown1(0)
{

}

CStreamChunk::~CStreamChunk()
{

}

void CStreamChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);
	m_numElements = inputStream.Read32_MSBF();
	m_vertexCount = inputStream.Read32_MSBF();
	m_bytesPerVertex = inputStream.Read32_MSBF();
	m_unknown1 = inputStream.Read32_MSBF();
	ReadElements(inputStream);
	ReadStream(inputStream);
}

uint32 CStreamChunk::GetVertexCount() const
{
	return m_vertexCount;
}

uint32 CStreamChunk::GetVertexSize() const
{
	return m_bytesPerVertex;
}

const uint8* CStreamChunk::GetData() const
{
	return m_data.data();
}

const CStreamChunk::ELEMENT* CStreamChunk::FindElement(ELEMENT_DATA_TYPE dataType) const
{
	for(const auto& element : m_elements)
	{
		if(element.dataType == dataType)
		{
			return &element;
		}
	}
	return nullptr;
}

void CStreamChunk::ReadElements(Framework::CStream& inputStream)
{
	for(unsigned int i = 0; i < m_numElements; i++)
	{
		ELEMENT element;
		element.unknown1		= inputStream.Read16_MSBF();
		element.offsetInVertex	= inputStream.Read16_MSBF();
		element.dataFormat		= static_cast<ELEMENT_DATA_FORMAT>(inputStream.Read32_MSBF());
		element.numElements		= inputStream.Read32_MSBF();
		element.dataType		= static_cast<ELEMENT_DATA_TYPE>(inputStream.Read16_MSBF());
		element.unknown2		= inputStream.Read16_MSBF();
		m_elements.push_back(element);
	}
	assert(std::is_sorted(std::begin(m_elements), std::end(m_elements), [](const ELEMENT& el1, const ELEMENT& el2) { return el1.offsetInVertex < el2.offsetInVertex; } ));
}

void CStreamChunk::ReadStream(Framework::CStream& inputStream)
{
	m_data.resize(m_vertexCount * m_bytesPerVertex);
	inputStream.Read(m_data.data(), m_vertexCount * m_bytesPerVertex);
}

uint32 CStreamChunk::GetElementDataSize(ELEMENT_DATA_FORMAT dataFormat)
{
	switch(dataFormat)
	{
	case ELEMENT_DATA_FORMAT_BYTE:
	case ELEMENT_DATA_FORMAT_SBYTE:
		return 1;
		break;
	case ELEMENT_DATA_FORMAT_INT16:
	case ELEMENT_DATA_FORMAT_UINT16:
	case ELEMENT_DATA_FORMAT_HALF:
		return 2;
		break;
	case ELEMENT_DATA_FORMAT_FLOAT:
		return 4;
		break;
	default:
		assert(0);
		return 0;
	}
}
