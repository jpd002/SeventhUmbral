#pragma once

#include "MemStream.h"
#include <vector>
#include <string>

struct PACKETHEADER
{
	uint32 packetType;
	uint16 packetSize;
	uint16 unknown0;
	uint32 unknown1[6];
};
static_assert(sizeof(PACKETHEADER) == 0x20, "Packet header size must be 32 bytes.");

struct SUBPACKETHEADER
{
	uint16 subPacketSize;
	uint16 unknown0;
	uint32 unknown1;
	uint32 unknown2;
	uint32 unknown3;
};
static_assert(sizeof(SUBPACKETHEADER) == 0x10, "Packet header size must be 16 bytes.");

typedef std::vector<uint8> PacketData;
typedef std::vector<PacketData> SubPacketArray;

class CPacketUtils
{
public:
	static bool					HasPacket(Framework::CMemStream&);
	static PacketData			ReadPacket(Framework::CMemStream&);
	static SubPacketArray		SplitPacket(const PacketData&);
	static std::string			DumpPacket(const PacketData&);
};
