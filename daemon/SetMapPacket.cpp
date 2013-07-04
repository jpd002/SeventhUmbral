#include "SetMapPacket.h"

CSetMapPacket::CSetMapPacket()
: m_mapId(MAP_BLACKSHROUD)
{
	m_commandId = 0x05;
	m_packetSize = 0x30;
}

CSetMapPacket::~CSetMapPacket()
{

}

void CSetMapPacket::SetMapId(uint32 mapId)
{
	m_mapId = mapId;
}

PacketData CSetMapPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();

	*reinterpret_cast<uint32*>(&result[0x20]) = m_mapId;
	*reinterpret_cast<uint32*>(&result[0x24]) = 0x9B;		//Unknown
	*reinterpret_cast<uint32*>(&result[0x28]) = 0x28;		//Unknown

	return result;
}
