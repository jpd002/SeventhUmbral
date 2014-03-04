#pragma once

#include "../BasePacket.h"
#include "MemStream.h"

class CSetActorPropertyPacket : public CBasePacket
{
public:
	enum VALUE_ID
	{
		VALUE_TP		= 0x0B99C876,
		VALUE_MP		= 0x13F89710,
		VALUE_MAXHP		= 0x3C95D5C5,
		VALUE_HP		= 0x4232BCAA,
		VALUE_LEVEL		= 0x96063588
	};

							CSetActorPropertyPacket();
	virtual					~CSetActorPropertyPacket();

	void					AddSetShort(uint32, uint16);
	void					AddTargetProperty(const std::string&);
	void					Finish();

	virtual PacketData		ToPacketData() const override;

private:
	Framework::CMemStream	m_modificationStream;
};
