#include "PacketUtils.h"
#include "../common/BLOWFISH.H"
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
		auto subPacket = PacketData(packetData, packetData + subHeader.subPacketSize);
		subPackets.push_back(subPacket);
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

uint16 CPacketUtils::GetSubPacketCommand(const PacketData& subPacket)
{
	return *reinterpret_cast<const uint16*>(subPacket.data() + 0x12);
}

void CPacketUtils::EncryptPacket(PacketData& packet)
{
	if(packet.size() <= sizeof(PACKETHEADER))
	{
		assert(0);
		return;
	}

	uint8* data = &packet[0];
	uint32 size = packet.size();

	data += 0x10;
	size -= 0x10;

	while(1)
	{
		uint32 subPacketSize = *reinterpret_cast<uint16*>(data);
		size -= subPacketSize;
		subPacketSize -= 0x10;
		data += 0x10;
		for(unsigned int i = 0; i < subPacketSize; i += 8)
		{
			Blowfish_encipher(
				reinterpret_cast<uint32*>(data + i), 
				reinterpret_cast<uint32*>(data + i + 4));
		}
		data += subPacketSize;
		if(size == 0) break;
	}
}

PacketData CPacketUtils::DecryptSubPacket(const PacketData& packet)
{
	PacketData result(packet);
	SUBPACKETHEADER subHeader = *reinterpret_cast<const SUBPACKETHEADER*>(result.data());
	if(subHeader.unknown0 != 0x03) return result;
	uint8* subPacketData = result.data() + 8;
	uint32 decryptSize = result.size() - 8;
	for(uint32 i = 0; i < decryptSize; i += 8)
	{
		Blowfish_decipher(
			reinterpret_cast<uint32*>(subPacketData + i), 
			reinterpret_cast<uint32*>(subPacketData + i + 4));
	}
	return result;
}
