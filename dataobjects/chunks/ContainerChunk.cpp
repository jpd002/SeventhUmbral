#include "ContainerChunk.h"
#include "../ChunkLoader.h"

CContainerChunk::CContainerChunk()
{

}

CContainerChunk::~CContainerChunk()
{

}

void CContainerChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);

	m_childrenCount = inputStream.Read32_MSBF();
	m_unknown1 = inputStream.Read32_MSBF();
	m_unknown2 = inputStream.Read32_MSBF();
	m_unknown3 = inputStream.Read32_MSBF();

	LoadChildren(inputStream);
}

void CContainerChunk::LoadChildren(Framework::CStream& inputStream)
{
	for(uint32 i = 0; i < m_childrenCount; i++)
	{
		uint64 baseOffset = inputStream.Tell();
		auto chunk = CChunkLoader::Load(shared_from_this(), inputStream);
		AddChild(chunk);
		uint64 nextOffset = baseOffset + chunk->GetNextChunkOffset();
		inputStream.Seek(nextOffset, Framework::STREAM_SEEK_SET);
	}
}
