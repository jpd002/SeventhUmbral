#pragma once

#include "AthenaEngine.h"
#include "../ResourceDefs.h"

class CUmbralMesh : public Athena::CMesh
{
public:
						CUmbralMesh(const MeshChunkPtr&, const ShaderSectionPtr&);
	virtual				~CUmbralMesh();

private:
	typedef std::map<std::string, Athena::TexturePtr> TextureMap;

	void						SetupGeometry(const MeshChunkPtr&);
	void						SetupMaterial(const ShaderSectionPtr&);

	static Athena::TexturePtr	GetTexture(const std::string&);

	static TextureMap			m_textures;
};
