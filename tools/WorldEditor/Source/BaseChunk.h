#pragma once

#include "StdStream.h"

class CBaseChunk
{
public:
					CBaseChunk();
	virtual			~CBaseChunk();

	virtual void	Read(Framework::CStream&);

protected:
	uint32			m_type;
	uint32			m_unknown1;
	uint32			m_dataSize;
	uint32			m_nextChunkOffset;
};
