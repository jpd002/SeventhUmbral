#pragma once

#include "PacketUtils.h"

class CCompositePacket
{
public:
					CCompositePacket();
	virtual			~CCompositePacket();

	void			AddPacket(const PacketData&);

	PacketData		ToPacketData() const;

private:
	PacketData		m_contents;
};
