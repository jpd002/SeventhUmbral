#pragma once

#include "PalleonEngine.h"
#include "../../../../dataobjects/ResourceDefs.h"

class CUmbralModel : public Palleon::CSceneNode
{
public:
						CUmbralModel(const ModelChunkPtr&);
	virtual				~CUmbralModel();

	void				SetLocalTexture(const ResourceNodePtr&);

private:
	ShaderSectionPtr	FindShaderForName(const std::vector<ShaderSectionPtr>&, const std::string&);
};

typedef std::shared_ptr<CUmbralModel> UmbralModelPtr;
