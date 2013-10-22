#include "ContainerChunk.h"

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
}
