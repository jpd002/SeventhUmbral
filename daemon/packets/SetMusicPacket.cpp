#include "SetMusicPacket.h"

CSetMusicPacket::CSetMusicPacket()
: m_musicId(MUSIC_GRIDANIA)
{
	m_commandId = 0x0C;
	m_packetSize = 0x28;
}

CSetMusicPacket::~CSetMusicPacket()
{

}

PacketPtr CSetMusicPacket::Clone()
{
	return std::make_shared<CSetMusicPacket>(*this);
}

void CSetMusicPacket::SetMusicId(uint32 musicId)
{
	m_musicId = musicId;
}

PacketData CSetMusicPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();

	*reinterpret_cast<uint16*>(&result[0x20]) = m_musicId;
	*reinterpret_cast<uint16*>(&result[0x22]) = 0x04;		//Unknown

	return result;
}
