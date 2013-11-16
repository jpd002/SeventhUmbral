#pragma once

#include "AthenaEngine.h"
#include "ResourceDefs.h"

class CUmbralModel : public Athena::CSceneNode
{
public:
						CUmbralModel(const ModelChunkPtr&);
	virtual				~CUmbralModel();

private:
	ShaderSectionPtr	FindShaderForName(const std::vector<ShaderSectionPtr>&, const std::string&);
};
