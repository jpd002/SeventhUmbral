#include "BaseChunk.h"

CBaseChunk::CBaseChunk()
{

}

CBaseChunk::~CBaseChunk()
{

}

void CBaseChunk::Read(Framework::CStream& inputStream)
{
	m_type = inputStream.Read32();
	m_unknown1 = inputStream.Read32();
	m_dataSize = inputStream.Read32_MSBF();
	m_nextChunkOffset = inputStream.Read32_MSBF();
}
