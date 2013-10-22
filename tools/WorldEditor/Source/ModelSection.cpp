#include "ModelSection.h"
#include "ChunkLoader.h"

CModelSection::CModelSection()
{

}

CModelSection::~CModelSection()
{

}

void CModelSection::Read(Framework::CStream& inputStream)
{
	CBaseSection::Read(inputStream);
	m_rootChunk = CChunkLoader::Load(inputStream);
}
