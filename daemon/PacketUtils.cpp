#include "PacketUtils.h"
#include <assert.h>

bool CPacketUtils::HasPacket(Framework::CMemStream& stream)
{
	if(stream.GetSize() < sizeof(PACKETHEADER))
	{
		return false;
	}

	stream.Seek(0, Framework::STREAM_SEEK_SET);

	PACKETHEADER header;
	stream.Read(&header, sizeof(PACKETHEADER));
	stream.Seek(0, Framework::STREAM_SEEK_END);

	if(stream.GetSize() < header.packetSize)
	{
		return false;
	}

	return true;
}

std::vector<uint8> CPacketUtils::ReadPacket(Framework::CMemStream& stream)
{
	std::vector<uint8> result;

	if(!CPacketUtils::HasPacket(stream))
	{
		assert(0);
		return result;
	}

	stream.Seek(0, Framework::STREAM_SEEK_SET);
	PACKETHEADER header;
	stream.Read(&header, sizeof(PACKETHEADER));

	stream.Seek(0, Framework::STREAM_SEEK_SET);
	result.resize(header.packetSize);
	stream.Read(&result[0], header.packetSize);
	stream.Truncate();

	stream.Seek(0, Framework::STREAM_SEEK_END);

	return result;
}

SubPacketArray CPacketUtils::SplitPacket(const PacketData& packet)
{
	SubPacketArray subPackets;

	assert(packet.size() >= sizeof(PACKETHEADER));
	const uint8* packetData = packet.data();

	PACKETHEADER header = *reinterpret_cast<const PACKETHEADER*>(packetData);
	assert(packet.size() == header.packetSize);

	uint32 currentSize = header.packetSize - 0x10;
	packetData += 0x10;

	while(currentSize != 0)
	{
		SUBPACKETHEADER subHeader = *reinterpret_cast<const SUBPACKETHEADER*>(packetData);
		assert(currentSize >= subHeader.subPacketSize);
		subPackets.push_back(PacketData(packetData, packetData + subHeader.subPacketSize));
		currentSize -= subHeader.subPacketSize;
		packetData += subHeader.subPacketSize;
	}

	return subPackets;
}

std::string CPacketUtils::DumpPacket(const PacketData& packet)
{
	std::string result;

	static const unsigned int lineWidth = 0x10;

	for(unsigned int i = 0; i < (packet.size() + lineWidth - 1) / lineWidth; i++)
	{
		for(unsigned int j = 0; j < lineWidth; j++)
		{
			unsigned int offset = i * lineWidth + j;
			if(offset >= packet.size())
			{
				result += "   ";
			}
			else
			{
				char byteString[4];
				sprintf(byteString, "%0.2X ", packet[offset]);
				result += byteString;
			}
		}

		result += "     ";

		for(unsigned int j = 0; j < lineWidth; j++)
		{
			unsigned int offset = i * lineWidth + j;
			if(offset >= packet.size()) continue;
			char character = packet[offset];
			if((character >= 0) && (isdigit(character) || isalpha(character)))
			{
				result += character;
			}
			else
			{
				result += ".";
			}
		}

		result += "\r\n";
	}

	return result;
}
