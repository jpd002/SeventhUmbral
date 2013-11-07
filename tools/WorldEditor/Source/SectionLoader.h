#pragma once

#include "Stream.h"
#include "ResourceNode.h"

class CSectionLoader
{
public:
	static ResourceNodePtr		ReadSection(Framework::CStream&);

private:
	static ResourceNodePtr		ReadSEDBSection(Framework::CStream&);
};
