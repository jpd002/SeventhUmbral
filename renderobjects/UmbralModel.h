#pragma once

#include "PalleonEngine.h"
#include "../dataobjects/ResourceDefs.h"

class CUmbralModel : public Palleon::CSceneNode
{
public:
						CUmbralModel(const ModelChunkPtr&);
	virtual				~CUmbralModel();

	const CSphere&		GetBoundingSphere() const;

	void				SetLocalTexture(const ResourceNodePtr&);

private:
	ShaderSectionPtr	FindShaderForName(const std::vector<ShaderSectionPtr>&, const std::string&);

	CSphere				m_boundingSphere;
};

typedef std::shared_ptr<CUmbralModel> UmbralModelPtr;
