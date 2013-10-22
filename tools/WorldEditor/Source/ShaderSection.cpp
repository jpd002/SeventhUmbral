#include "ShaderSection.h"
#include "ChunkLoader.h"

CShaderSection::CShaderSection()
{

}

CShaderSection::~CShaderSection()
{

}

void CShaderSection::Read(Framework::CStream& inputStream)
{
	CBaseSection::Read(inputStream);
	m_rootChunk = CChunkLoader::Load(inputStream);
}
