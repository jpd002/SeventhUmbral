#include "BaseChunk.h"

CBaseChunk::CBaseChunk()
: m_type(0)
, m_unknown1(0)
, m_dataSize(0)
, m_nextChunkOffset(0)
{

}

CBaseChunk::~CBaseChunk()
{

}

uint32 CBaseChunk::GetNextChunkOffset() const
{
	return m_nextChunkOffset;
}

void CBaseChunk::Read(Framework::CStream& inputStream)
{
	m_type = inputStream.Read32();
	m_unknown1 = inputStream.Read32();
	m_dataSize = inputStream.Read32_MSBF();
	m_nextChunkOffset = inputStream.Read32_MSBF();
}
