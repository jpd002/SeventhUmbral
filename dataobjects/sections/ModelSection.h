#pragma once

#include "BaseSection.h"

class CModelSection : public CBaseSection
{
public:
					CModelSection();
	virtual			~CModelSection();

	virtual void	Read(Framework::CStream&) override;
};
