#pragma once

#include "BasePacket.h"

class CSetMapPacket : public CBasePacket
{
public:
	enum MAP_ID
	{
		MAP_NOSCEA			= 0x65,
		MAP_COERTHAS		= 0x66,
		MAP_BLACKSHROUD		= 0x67,
		MAP_THANALAN		= 0x68,
		MAP_MORDHONA		= 0x69,

		MAP_RIVENROAD		= 0x6D,
		MAP_LARGEBOAT		= 0x6F,
		MAP_SMALLBOAT		= 0x71,
	};

							CSetMapPacket();
	virtual					~CSetMapPacket();

	virtual PacketData		ToPacketData() const override;

	void					SetMapId(uint32);

private:
	uint32					m_mapId;
};
