#include "SetWeatherPacket.h"

CSetWeatherPacket::CSetWeatherPacket()
: m_weatherId(WEATHER_CLEAR)
{
	m_commandId = 0x0D;
	m_packetSize = 0x28;
}

CSetWeatherPacket::~CSetWeatherPacket()
{

}

PacketPtr CSetWeatherPacket::Clone()
{
	return std::make_shared<CSetWeatherPacket>(*this);
}

void CSetWeatherPacket::SetWeatherId(uint32 weatherId)
{
	m_weatherId = weatherId;
}

PacketData CSetWeatherPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();

	*reinterpret_cast<uint32*>(&result[0x20]) = m_weatherId;

	return result;
}
