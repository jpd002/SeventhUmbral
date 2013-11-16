#pragma once

#include "AthenaEngine.h"
#include "ResourceDefs.h"

class CUmbralMesh : public Athena::CMesh
{
public:
						CUmbralMesh(const MeshChunkPtr&, const ShaderSectionPtr&);
	virtual				~CUmbralMesh();

private:
	void				SetupGeometry(const MeshChunkPtr&);
	void				SetupMaterial(const ShaderSectionPtr&);
};
