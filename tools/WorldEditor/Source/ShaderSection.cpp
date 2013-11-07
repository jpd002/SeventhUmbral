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
	auto chunk = CChunkLoader::Load(inputStream);
	m_children.push_back(chunk);
}
