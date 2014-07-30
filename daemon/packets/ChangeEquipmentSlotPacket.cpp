#include "ChangeEquipmentSlotPacket.h"

CChangeEquipmentSlotPacket::CChangeEquipmentSlotPacket()
{
	m_commandId = 0x14D;
	m_packetSize = 0x28;
}

CChangeEquipmentSlotPacket::~CChangeEquipmentSlotPacket()
{

}

PacketPtr CChangeEquipmentSlotPacket::Clone() const
{
	return std::make_shared<CChangeEquipmentSlotPacket>(*this);
}

void CChangeEquipmentSlotPacket::SetSlotId(uint16 slotId)
{
	m_info.slotId = slotId;
}

void CChangeEquipmentSlotPacket::SetItemIndex(uint32 itemIndex)
{
	m_info.itemIndex = itemIndex;
}

PacketData CChangeEquipmentSlotPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	uint8* resultData = result.data();
	memcpy(resultData + PACKET_HEADER_SIZE, &m_info, sizeof(CHANGE_EQUIPMENT_SLOT_INFO));
	return result;
}
