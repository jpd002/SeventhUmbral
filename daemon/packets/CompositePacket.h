#pragma once

#include "../PacketUtils.h"

class CCompositePacket
{
public:
						CCompositePacket();
	virtual				~CCompositePacket();

	void				AddPacket(const PacketData&);

	PacketData			ToPacketData() const;

private:
	static PacketData	CompressPacketData(const PacketData&);

	PacketData			m_contents;
};
