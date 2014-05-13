#pragma once

#include "BasePacket.h"

class CFinishScriptPacket : public CBasePacket
{
public:
							CFinishScriptPacket();
	virtual					~CFinishScriptPacket();

	virtual PacketPtr		Clone() override;

	void					SetScriptSourceId(uint32);
	void					SetScriptName(const std::string&);

	virtual PacketData		ToPacketData() const override;

private:
	struct FINISH_SCRIPT_INFO
	{
		FINISH_SCRIPT_INFO()
		{
			memset(&scriptName, 0, sizeof(scriptName));
		}

		uint32	sourceId = 0;
		uint32	unknown0 = 0;
		uint8	unknown1 = 0;
		char	scriptName[0x17];
		uint32	unknown2 = 0;
		uint32	unknown3 = 0;
		uint32	unknown4 = 0x09D4F200;
		uint32	unknown5 = 0x0A09A930;   //0x09EE6D80;
	};
	static_assert(sizeof(FINISH_SCRIPT_INFO) == 0x30, "Size of FINISH_SCRIPT_INFO must be 48 bytes.");

	FINISH_SCRIPT_INFO		m_info;
};
