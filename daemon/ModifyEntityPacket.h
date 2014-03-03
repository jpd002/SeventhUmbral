#pragma once

#include "BasePacket.h"
#include "MemStream.h"

class CModifyEntityPacket : public CBasePacket
{
public:
							CModifyEntityPacket();
	virtual					~CModifyEntityPacket();

	void					AddModifyShort(uint32, uint16);
	void					AddTargetVariable(const std::string&);
	void					Finish();

	virtual PacketData		ToPacketData() const override;

private:
	Framework::CMemStream	m_modificationStream;
};
