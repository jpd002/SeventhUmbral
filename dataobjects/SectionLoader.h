#pragma once

#include "Stream.h"
#include "ResourceNode.h"

class CSectionLoader
{
public:
	static ResourceNodePtr		ReadSection(const ResourceNodePtr&, Framework::CStream&);

private:
	static ResourceNodePtr		ReadSEDBSection(const ResourceNodePtr&, Framework::CStream&);
};
