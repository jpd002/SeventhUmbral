#pragma once

#include "BaseSection.h"

class CSkeletonSection : public CBaseSection
{
public:
					CSkeletonSection();
	virtual			~CSkeletonSection();

	virtual void	Read(Framework::CStream&) override;
};
