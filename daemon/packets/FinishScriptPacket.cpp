#include "FinishScriptPacket.h"

CFinishScriptPacket::CFinishScriptPacket()
{
	m_commandId = 0x131;
	m_packetSize = 0x50;
}

CFinishScriptPacket::~CFinishScriptPacket()
{

}

PacketPtr CFinishScriptPacket::Clone()
{
	return std::make_shared<CFinishScriptPacket>(*this);
}

void CFinishScriptPacket::SetScriptSourceId(uint32 sourceId)
{
	m_info.sourceId = sourceId;
}

void CFinishScriptPacket::SetScriptName(const std::string& scriptName)
{
	strncpy(m_info.scriptName, scriptName.c_str(), sizeof(m_info.scriptName));
}

PacketData CFinishScriptPacket::ToPacketData() const
{
	auto result = CBasePacket::ToPacketData();
	memcpy(result.data() + PACKET_HEADER_SIZE, &m_info, sizeof(FINISH_SCRIPT_INFO));
	return result;
}
