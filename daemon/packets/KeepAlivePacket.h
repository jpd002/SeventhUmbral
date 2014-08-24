#pragma once

#include "BasePacket.h"

class CKeepAlivePacket : public CBasePacket
{
public:
							CKeepAlivePacket();
	virtual					~CKeepAlivePacket();

	virtual PacketPtr		Clone() const override;

	virtual PacketData		ToPacketData() const override;

private:
	struct KEEP_ALIVE_INFO
	{
		uint32 unknown1 = 0x0A9C184A;
		uint32 unknown2 = 0x14D;
		uint32 unknown3 = 0;
		uint32 unknown4 = 0;
		uint32 unknown5 = 0;
		uint32 unknown6 = 0;
		uint32 unknown7 = 0;
		uint32 unknown8 = 0;
	};
	static_assert(sizeof(KEEP_ALIVE_INFO) == 0x20, "Size of KEEP_ALIVE_INFO must be 32 bytes.");

	KEEP_ALIVE_INFO			m_info;
};
