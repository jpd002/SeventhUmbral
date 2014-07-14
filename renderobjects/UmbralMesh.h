#pragma once

#include "PalleonEngine.h"
#include "../dataobjects/ResourceDefs.h"
#include "D3DShader.h"

class CUmbralMesh;
typedef std::shared_ptr<CUmbralMesh> UmbralMeshPtr;

class CUmbralMesh : public Palleon::CMesh
{
public:
										CUmbralMesh();
										CUmbralMesh(const MeshChunkPtr&, const ShaderSectionPtr&);
	virtual								~CUmbralMesh();

	UmbralMeshPtr						CreateInstance() const;

	Palleon::EffectPtr					GetEffect() const;

	virtual void						Update(float) override;

	void								SetLocalTexture(const ResourceNodePtr&);
	void								SetActivePolyGroups(uint32);

	void								RebuildIndices();

private:
	typedef std::map<std::string, unsigned int> SamplerRegisterMap;
	typedef CPgrpChunk::TriangleArray TriangleIndexArray;
	typedef std::map<unsigned int, TriangleIndexArray> PolyGroupMap;

	Palleon::VERTEX_BUFFER_DESCRIPTOR	GenerateVertexBufferDescriptor(const StreamChunkPtr&, const StreamChunkPtr&);

	void								SetupGeometry();
	void								SetupPolyGroups();
	void								SetupEffect();
	void								SetupTextures();

	Palleon::EffectPtr					m_effect;

	MeshChunkPtr						m_meshChunk;
	ShaderSectionPtr					m_shaderSection;
	ResourceNodePtr						m_localTexture;
	SamplerRegisterMap					m_samplerRegisters;
	
	TriangleIndexArray					m_basePolyGroup;
	PolyGroupMap						m_polyGroups;
	uint32								m_activePolyGroups = 0;

	bool								m_indexRebuildNeeded = false;
};
