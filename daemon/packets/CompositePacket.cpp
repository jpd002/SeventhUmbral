#include "CompositePacket.h"
#include "../Log.h"
#include <zlib.h>

#define LOG_NAME "CompositePacket"

CCompositePacket::CCompositePacket()
{

}

CCompositePacket::~CCompositePacket()
{

}

void CCompositePacket::AddPacket(const PacketData& packet)
{
	m_contents.insert(std::end(m_contents), std::begin(packet), std::end(packet));
}

PacketData CCompositePacket::ToPacketData() const
{
	auto compressedContents = CompressPacketData(m_contents);

	uint32 totalPacketSize = compressedContents.size() + 0x10;

	PacketData result;
	result.resize(totalPacketSize);

	//Write packet header
	result[0x00] = 0x01;
	result[0x01] = 0x01;		//Compressed flag
	*reinterpret_cast<uint16*>(result.data() + 0x04) = totalPacketSize;
	*reinterpret_cast<uint16*>(result.data() + 0x06) = 0x1D;		//Unknown
	*reinterpret_cast<uint32*>(result.data() + 0x08) = 0xEE8B1C75;	//Unknown
	*reinterpret_cast<uint32*>(result.data() + 0x0C) = 0x13B;		//Unknown

	memcpy(result.data() + 0x10, compressedContents.data(), compressedContents.size());

	return result;
}

PacketData CCompositePacket::CompressPacketData(const PacketData& packet)
{
	PacketData compressedPacket;

	const uint32 BUFFERSIZE = 0x100;
	Bytef outBuffer[BUFFERSIZE];

	compressedPacket.reserve(packet.size() * 2);

	z_stream stream = {};
	int result = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
	if(result != Z_OK)
	{
		CLog::GetInstance().LogError(LOG_NAME, "deflateInit2 failed.");
		return PacketData();
	}

	stream.avail_in = static_cast<uInt>(packet.size());
	stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(packet.data()));

	while(1)
	{
		stream.avail_out = BUFFERSIZE;
		stream.next_out = outBuffer;

		int ret = deflate(&stream, Z_FINISH);
		if(ret < 0)
		{
			CLog::GetInstance().LogError(LOG_NAME, "deflate failed.");
			deflateEnd(&stream);
			return PacketData();
		}

		compressedPacket.insert(std::end(compressedPacket), outBuffer, stream.next_out);

		if(ret == Z_STREAM_END) break;
	}

	deflateEnd(&stream);

	return compressedPacket;
}
