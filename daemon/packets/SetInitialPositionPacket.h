#pragma once

#include "BasePacket.h"

class CSetInitialPositionPacket : public CBasePacket
{
public:
							CSetInitialPositionPacket();
	virtual					~CSetInitialPositionPacket();

	virtual PacketData		ToPacketData() const override;

	void					SetX(float);
	void					SetY(float);
	void					SetZ(float);
	void					SetAngle(float);

private:
	float					m_x;
	float					m_y;
	float					m_z;
	float					m_angle;
};
