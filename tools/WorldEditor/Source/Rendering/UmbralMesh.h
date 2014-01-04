#pragma once

#include "AthenaEngine.h"
#include "../ResourceDefs.h"

class CUmbralMesh : public Athena::CMesh
{
public:
								CUmbralMesh(const MeshChunkPtr&, const ShaderSectionPtr&);
	virtual						~CUmbralMesh();

	void						SetLocalTexture(const ResourceNodePtr&);

private:
	typedef std::map<std::string, Athena::TexturePtr> TextureMap;

	void						SetupGeometry(const MeshChunkPtr&);
	void						SetupEffect();
	void						SetupTextures();

	static Athena::TexturePtr	GetTexture(const std::string&);
	static Athena::TexturePtr	CreateTextureFromGtex(const GtexDataPtr&);

	static TextureMap			m_textures;

	ShaderSectionPtr			m_shaderSection;
	ResourceNodePtr				m_localTexture;
};
