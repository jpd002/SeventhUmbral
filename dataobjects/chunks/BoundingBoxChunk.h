#pragma once

#include "BaseChunk.h"

class CBoundingBoxChunk : public CBaseChunk
{
public:
					CBoundingBoxChunk();
	virtual			~CBoundingBoxChunk();

	virtual void	Read(Framework::CStream&) override;

private:
	float			m_minX = 0;
	float			m_minY = 0;
	float			m_minZ = 0;

	float			m_maxX = 0;
	float			m_maxY = 0;
	float			m_maxZ = 0;
};
