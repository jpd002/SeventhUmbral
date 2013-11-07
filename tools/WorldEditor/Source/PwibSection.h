#pragma once

#include "Stream.h"
#include "ResourceNode.h"

class CPwibSection : public CResourceNode
{
public:
								CPwibSection();
	virtual						~CPwibSection();

	void						Read(Framework::CStream&);
};
