#include "NameChunk.h"

void CNameChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);
	m_name = inputStream.ReadString();
}
