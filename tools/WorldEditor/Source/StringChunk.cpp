#include "StringChunk.h"

void CStringChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);

	m_string = inputStream.ReadString();
}
