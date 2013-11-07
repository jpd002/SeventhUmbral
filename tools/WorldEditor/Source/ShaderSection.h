#pragma once

#include "BaseSection.h"
#include "BaseChunk.h"

class CShaderSection : public CBaseSection
{
public:
					CShaderSection();
	virtual			~CShaderSection();

	virtual void	Read(Framework::CStream&);
};
