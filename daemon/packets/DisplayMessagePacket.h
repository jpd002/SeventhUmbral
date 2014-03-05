#pragma once

#include "BasePacket.h"

class CDisplayMessagePacket : public CBasePacket
{
public:
							CDisplayMessagePacket();
	virtual					~CDisplayMessagePacket();

	void					SetMessage(const std::string&);

	virtual PacketData		ToPacketData() const override;

private:
	std::string				m_message;
};
