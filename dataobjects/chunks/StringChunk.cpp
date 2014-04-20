#include "StringChunk.h"

void CStringChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);

	m_string = inputStream.ReadString();
}

std::string CStringChunk::GetString() const
{
	return m_string;
}
