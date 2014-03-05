#pragma once

#include "BasePacket.h"

class CSetMusicPacket : public CBasePacket
{
public:
	enum MUSIC_ID
	{
		MUSIC_BLACKSHROUD_BATTLE	= 0x0D,
		MUSIC_TOTORAK				= 0x14,
		MUSIC_MORDHONA				= 0x31,
		MUSIC_GRIDANIA				= 0x33,
		MUSIC_DEEPSHROUD			= 0x34,
		MUSIC_NOSCEA				= 0x35,
		MUSIC_COERTHAS				= 0x37,
		MUSIC_SHROUD				= 0x39,
		MUSIC_THANALAN				= 0x43,
	};

							CSetMusicPacket();
	virtual					~CSetMusicPacket();

	virtual PacketData		ToPacketData() const override;

	void					SetMusicId(uint32);

private:
	uint32					m_musicId;
};
