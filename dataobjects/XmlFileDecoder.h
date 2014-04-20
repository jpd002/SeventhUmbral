#pragma once

#include <vector>
#include "Types.h"
#include "Stream.h"

class CXmlFileDecoder
{
public:
	static std::vector<uint8>	Decode(Framework::CStream&);

private:
	static void					ScrambleBuffer(uint8* buffer, size_t bufferSize);
};
