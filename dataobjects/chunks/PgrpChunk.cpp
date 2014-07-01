#include "PgrpChunk.h"

CPgrpChunk::CPgrpChunk()
{

}

CPgrpChunk::~CPgrpChunk()
{

}

void CPgrpChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);

	auto basePosition = inputStream.Tell();

	uint32 numTriangles = inputStream.Read32_MSBF();
	uint32 trianglesOffset = inputStream.Read32_MSBF();
	uint32 nameOffset = inputStream.Read32_MSBF();

	inputStream.Seek(basePosition + trianglesOffset, Framework::STREAM_SEEK_SET);
	m_triangles.resize(numTriangles);
	for(unsigned int i = 0; i < numTriangles; i++)
	{
		m_triangles[i] = inputStream.Read16_MSBF();
	}

	inputStream.Seek(basePosition + nameOffset, Framework::STREAM_SEEK_SET);
	m_name = inputStream.ReadString();
}

const CPgrpChunk::TriangleArray& CPgrpChunk::GetTriangles() const
{
	return m_triangles;
}

std::string CPgrpChunk::GetName() const
{
	return m_name;
}
