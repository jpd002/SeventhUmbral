#pragma once

#include "BaseChunk.h"

class CCompChunk : public CBaseChunk
{
public:
					CCompChunk();
	virtual			~CCompChunk();

	virtual void	Read(Framework::CStream&) override;

	float			GetMinX() const;
	float			GetMinY() const;
	float			GetMinZ() const;

	float			GetMaxX() const;
	float			GetMaxY() const;
	float			GetMaxZ() const;

private:
	float			m_minX = 0;
	float			m_minY = 0;
	float			m_minZ = 0;

	float			m_maxX = 0;
	float			m_maxY = 0;
	float			m_maxZ = 0;
};
