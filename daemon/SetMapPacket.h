#pragma once

#include "BasePacket.h"

class CSetMapPacket : public CBasePacket
{
public:
	enum MUSIC_ID
	{
		MAP_NOSCEA			= 0x65,
		MAP_COERTHAS		= 0x66,
		MAP_BLACKSHROUD		= 0x67,
		MAP_THANALAN		= 0x68,
		MAP_MORDHONA		= 0x69,
	};

							CSetMapPacket();
	virtual					~CSetMapPacket();

	virtual PacketData		ToPacketData() const override;

	void					SetMapId(uint32);

private:
	uint32					m_mapId;
};
