#pragma once

#include "BaseSection.h"

class CPhbSection : public CBaseSection
{
public:
					CPhbSection();
	virtual			~CPhbSection();

	virtual void	Read(Framework::CStream&) override;
};
