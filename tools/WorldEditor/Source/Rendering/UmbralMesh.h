#pragma once

#include "AthenaEngine.h"
#include "../ResourceDefs.h"
#include "D3DShader.h"

class CUmbralMesh : public Athena::CMesh
{
public:
										CUmbralMesh(const MeshChunkPtr&, const ShaderSectionPtr&);
	virtual								~CUmbralMesh();

	Athena::EffectPtr					GetEffect() const;

	void								SetLocalTexture(const ResourceNodePtr&);

private:
	typedef std::map<std::string, unsigned int> SamplerRegisterMap;

	Athena::VERTEX_BUFFER_DESCRIPTOR	GenerateVertexBufferDescriptor(const StreamChunkPtr&, const StreamChunkPtr&);

	void								SetupGeometry(const MeshChunkPtr&);
	void								SetupEffect();
	void								SetupTextures();

	Athena::EffectPtr					m_effect;

	ShaderSectionPtr					m_shaderSection;
	ResourceNodePtr						m_localTexture;
	SamplerRegisterMap					m_samplerRegisters;
};
