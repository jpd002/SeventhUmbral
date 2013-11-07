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
	auto chunk = CChunkLoader::Load(inputStream);
	m_children.push_back(chunk);
}
