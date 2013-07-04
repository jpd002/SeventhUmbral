#include "CompositePacket.h"

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
	uint32 totalPacketSize = m_contents.size() + 0x10;

	PacketData result;
	result.resize(totalPacketSize);

	//Write packet header
	result[0x00] = 0x01;
	//result[0x01] = 0x01;		//Compressed flag
	*reinterpret_cast<uint16*>(result.data() + 0x04) = totalPacketSize;
	*reinterpret_cast<uint16*>(result.data() + 0x06) = 0x1D;		//Unknown
	*reinterpret_cast<uint32*>(result.data() + 0x08) = 0xEE8B1C75;	//Unknown
	*reinterpret_cast<uint32*>(result.data() + 0x0C) = 0x13B;		//Unknown

	memcpy(result.data() + 0x10, m_contents.data(), m_contents.size());

	return result;
}
