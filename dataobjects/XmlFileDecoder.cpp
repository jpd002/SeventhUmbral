#include "XmlFileDecoder.h"

std::vector<uint8> CXmlFileDecoder::Decode(Framework::CStream& inputStream)
{
	uint32 srcLength = inputStream.GetLength();
	std::vector<uint8> srcData;

	srcData.resize(srcLength);
	inputStream.Read(srcData.data(), srcLength);

	if(srcData.size() == 0) return srcData;

	if(srcData[srcLength - 1] != 0xF1)
	{
		return srcData;
	}

	std::vector<uint8> dstData;
	uint32 dstLength = srcLength - 1;
	dstData.resize(dstLength);

	int32 encodedLength = srcLength - 1;
	memcpy(dstData.data(), srcData.data(), encodedLength);
	ScrambleBuffer(dstData.data(), encodedLength);

	int16 xA = static_cast<int16>(encodedLength * 7);
	int16 xB = *reinterpret_cast<int16*>(dstData.data() + 6) ^ 0x6C6D;

	int16* dstEnd = reinterpret_cast<int16*>(dstData.data() + encodedLength - 1);

	{
		int16* ptr = reinterpret_cast<int16*>(dstData.data());
		while(ptr < dstEnd)
		{
			(*ptr) ^= xA;
			ptr += 2;
		}
	}

	{
		int16* ptr = reinterpret_cast<int16*>(dstData.data() + 2);
		while(ptr < dstEnd)
		{
			(*ptr) ^= xB;
			ptr += 2;
		}
	}

	if(encodedLength & 1)
	{
		*(dstData.data() + encodedLength - 1) ^= static_cast<uint8>(xA);
		*(dstData.data() + encodedLength - 1) ^= static_cast<uint8>(xB);
	}

	return dstData;
}

void CXmlFileDecoder::ScrambleBuffer(uint8* buffer, size_t bufferSize)
{
	uint8* ptr = buffer;
	uint8* end = buffer + bufferSize - 1;
	while(ptr < end)
	{
		std::swap(*ptr, *end);
		ptr += 2;
		end -= 2;
	}
}
