#include "BoundingBoxChunk.h"

CBoundingBoxChunk::CBoundingBoxChunk()
{

}

CBoundingBoxChunk::~CBoundingBoxChunk()
{

}

void CBoundingBoxChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);

	uint32 minXInt = inputStream.Read32_MSBF();
	uint32 minYInt = inputStream.Read32_MSBF();
	uint32 minZInt = inputStream.Read32_MSBF();
	uint32 maxXInt = inputStream.Read32_MSBF();
	uint32 maxYInt = inputStream.Read32_MSBF();
	uint32 maxZInt = inputStream.Read32_MSBF();

	m_minX = *reinterpret_cast<float*>(&minXInt);
	m_minY = *reinterpret_cast<float*>(&minYInt);
	m_minZ = *reinterpret_cast<float*>(&minZInt);

	m_maxX = *reinterpret_cast<float*>(&maxXInt);
	m_maxY = *reinterpret_cast<float*>(&maxYInt);
	m_maxZ = *reinterpret_cast<float*>(&maxZInt);
}
