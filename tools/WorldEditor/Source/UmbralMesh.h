#pragma once

#include "AthenaEngine.h"
#include "MeshChunk.h"

class CUmbralMesh : public Athena::CMesh
{
public:
				CUmbralMesh(const MeshChunkPtr&);
	virtual		~CUmbralMesh();

private:

};
