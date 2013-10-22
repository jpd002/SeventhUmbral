#pragma once

#include "StdStream.h"
#include <memory>

class CBaseChunk
{
public:
					CBaseChunk();
	virtual			~CBaseChunk();

	virtual void	Read(Framework::CStream&);

	uint32			GetNextChunkOffset() const;

protected:
	uint32			m_type;
	uint32			m_unknown1;
	uint32			m_dataSize;
	uint32			m_nextChunkOffset;
};

typedef std::shared_ptr<CBaseChunk> ChunkPtr;
