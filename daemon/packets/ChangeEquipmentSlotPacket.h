#pragma once

#include "BasePacket.h"

class CChangeEquipmentSlotPacket : public CBasePacket
{
public:
	enum SLOT_ID
	{
		SLOT_MAINHAND			= 0x00,
		SLOT_OFFHAND			= 0x01,
		SLOT_THROWINGWEAPON		= 0x04,
		SLOT_PACK				= 0x05,
		SLOT_POUCH				= 0x06,
		SLOT_HEAD				= 0x08,
		SLOT_UNDERSHIRT			= 0x09,
		SLOT_BODY				= 0x0A,
		SLOT_UNDERGARMENT		= 0x0B,
		SLOT_LEGS				= 0x0C,
		SLOT_HANDS				= 0x0D,
		SLOT_BOOTS				= 0x0E,
		SLOT_WAIST				= 0x0F,
		SLOT_NECK				= 0x10,
		SLOT_EARS				= 0x11,
		SLOT_WRISTS				= 0x13,
		SLOT_RIGHTFINGER		= 0x15,
		SLOT_LEFTFINGER			= 0x16
	};

									CChangeEquipmentSlotPacket();
	virtual							~CChangeEquipmentSlotPacket();

	virtual PacketPtr				Clone() const override;

	void							SetSlotId(uint16);
	void							SetItemIndex(uint32);

	virtual PacketData				ToPacketData() const;

private:
#pragma pack(push, 1)
	struct CHANGE_EQUIPMENT_SLOT_INFO
	{
		uint16			slotId = 0;
		uint32			itemIndex = 0;
		uint16			unknown = 0;
	};
#pragma pack(pop)
	static_assert(sizeof(CHANGE_EQUIPMENT_SLOT_INFO) == 8, "Size of CHANGE_EQUIPMENT_SLOT_INFO must be 8 bytes.");

	CHANGE_EQUIPMENT_SLOT_INFO		m_info;
};
