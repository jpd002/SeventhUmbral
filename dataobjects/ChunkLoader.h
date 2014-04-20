#pragma once

#include "Stream.h"
#include "chunks/BaseChunk.h"

class CChunkLoader
{
public:
	static ChunkPtr		Load(const ResourceNodePtr&, Framework::CStream&);
};
