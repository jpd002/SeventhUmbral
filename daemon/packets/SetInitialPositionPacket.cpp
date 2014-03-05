#include "SetInitialPositionPacket.h"

CSetInitialPositionPacket::CSetInitialPositionPacket()
: m_x(0)
, m_y(0)
, m_z(0)
, m_angle(0)
{
	m_commandId = 0xCE;
	m_packetSize = 0x48;
}

CSetInitialPositionPacket::~CSetInitialPositionPacket()
{

}

void CSetInitialPositionPacket::SetX(float x)
{
	m_x = x;
}

void CSetInitialPositionPacket::SetY(float y)
{
	m_y = y;
}

void CSetInitialPositionPacket::SetZ(float z)
{
	m_z = z;
}

void CSetInitialPositionPacket::SetAngle(float angle)
{
	m_angle = angle;
}

PacketData CSetInitialPositionPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();

	*reinterpret_cast<uint32*>(&result[0x24]) = ~0;		//?

	*reinterpret_cast<float*>(&result[0x28]) = m_x;
	*reinterpret_cast<float*>(&result[0x2C]) = m_y;
	*reinterpret_cast<float*>(&result[0x30]) = m_z;
	*reinterpret_cast<float*>(&result[0x34]) = m_angle;

	*reinterpret_cast<uint32*>(&result[0x44]) = 1;		//?

	return result;
}
