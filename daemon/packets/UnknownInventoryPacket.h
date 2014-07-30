#pragma once

#include "BasePacket.h"

////////////////////////////////////////////////////////////////////////
//Packet 0x0146
////////////////////////////////////////////////////////////////////////

class CUnknownInventoryPacket_0146 : public CBasePacket
{
public:
							CUnknownInventoryPacket_0146();
	virtual					~CUnknownInventoryPacket_0146();

	void					SetActorId(uint32);
	void					SetUnknown0(uint16);
	void					SetUnknown1(uint16);

	virtual PacketPtr		Clone() const override;
	virtual PacketData		ToPacketData() const override;

private:
	struct PACKET_INFO
	{
		uint32	actorId = 0;
		uint16	unknown0 = 0;
		uint16	unknown1 = 0;
	};
	static_assert(sizeof(PACKET_INFO) == 8, "PACKET_INFO must be 8 bytes.");

	PACKET_INFO				m_info;
};

////////////////////////////////////////////////////////////////////////
//Packet 0x0147
////////////////////////////////////////////////////////////////////////

class CUnknownInventoryPacket_0147 : public CBasePacket
{
public:
							CUnknownInventoryPacket_0147();
	virtual					~CUnknownInventoryPacket_0147();

	virtual PacketPtr		Clone() const override;
	virtual PacketData		ToPacketData() const override;

private:
	struct PACKET_INFO
	{
		uint32	unknown0 = 0;
		uint32	unknown1 = 0;
	};
	static_assert(sizeof(PACKET_INFO) == 8, "PACKET_INFO must be 8 bytes.");

	PACKET_INFO				m_info;
};

////////////////////////////////////////////////////////////////////////
//Packet 0x016D
////////////////////////////////////////////////////////////////////////

class CUnknownInventoryPacket_016D : public CBasePacket
{
public:
							CUnknownInventoryPacket_016D();
	virtual					~CUnknownInventoryPacket_016D();

	virtual PacketPtr		Clone() const override;
	virtual PacketData		ToPacketData() const override;

private:
	struct PACKET_INFO
	{
		uint32	unknown0 = 0;
		uint32	unknown1 = 0;
	};
	static_assert(sizeof(PACKET_INFO) == 8, "PACKET_INFO must be 8 bytes.");

	PACKET_INFO				m_info;
};

////////////////////////////////////////////////////////////////////////
//Packet 0x016E
////////////////////////////////////////////////////////////////////////

class CUnknownInventoryPacket_016E : public CBasePacket
{
public:
							CUnknownInventoryPacket_016E();
	virtual					~CUnknownInventoryPacket_016E();

	virtual PacketPtr		Clone() const override;
	virtual PacketData		ToPacketData() const override;

private:
	struct PACKET_INFO
	{
		uint32	unknown0 = 0;
		uint32	unknown1 = 0;
	};
	static_assert(sizeof(PACKET_INFO) == 8, "PACKET_INFO must be 8 bytes.");

	PACKET_INFO				m_info;
};
