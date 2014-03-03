#include <assert.h>
#include "ModifyEntityPacket.h"

//Known valueIds
//0x0B99C876 -> TP (short)
//0x4232BCAA -> HP (short)
//0x13F89710 -> MP (short)
//0x3C95D5C5 -> Max MP (short)
//0x96063588 -> Level (short)

CModifyEntityPacket::CModifyEntityPacket()
{
	m_commandId = 0x137;
	m_packetSize = 0xA8;
	m_modificationStream.Allocate(m_packetSize - PACKET_HEADER_SIZE - 1);
	memset(m_modificationStream.GetBuffer(), 0, m_modificationStream.GetSize());
}

CModifyEntityPacket::~CModifyEntityPacket()
{

}

void CModifyEntityPacket::AddModifyShort(uint32 valueId, uint16 value)
{
	m_modificationStream.Write8(0x02);
	m_modificationStream.Write32(valueId);
	m_modificationStream.Write16(value);
}

void CModifyEntityPacket::AddTargetVariable(const std::string& name)
{
	//Why 0x82?
	m_modificationStream.Write8(0x82 + name.length());
	m_modificationStream.Write(name.c_str(), name.length());
}

PacketData CModifyEntityPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	assert(m_modificationStream.GetSize() == (m_packetSize - PACKET_HEADER_SIZE - 1));
	result[0x20] = const_cast<Framework::CMemStream&>(m_modificationStream).Tell();
	memcpy(result.data() + 0x21, m_modificationStream.GetBuffer(), m_modificationStream.GetSize());
	return result;
}
