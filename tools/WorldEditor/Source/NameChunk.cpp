#include "NameChunk.h"

void CNameChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);
	m_name = inputStream.ReadString();
}

std::string CNameChunk::GetName() const
{
	return m_name;
}
