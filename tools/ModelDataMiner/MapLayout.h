#pragma once

#include "Stream.h"

class CMapLayout
{
public:
					CMapLayout(Framework::CStream&);
	virtual			~CMapLayout();

private:
	void			Load(Framework::CStream&);
};
