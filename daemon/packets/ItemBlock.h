#pragma once

#include <cstring>
#include "Types.h"

struct ITEMBLOCK
{
	ITEMBLOCK()
	{
		memset(&unknown20, 0, sizeof(unknown20));
	}

	uint32 id			= 0;
	uint32 unknown1		= 0;
	uint32 quantity		= 1;
	uint32 definitionId	= 0;				//(from item data sheet)

	uint32 index		= 0;
	uint32 unknown5		= 0;
	uint32 unknown6		= 0;
	uint32 unknown7		= 0;

	uint32 unknown8		= 0;				//Flags? 0x03 = Untradable
	uint32 unknown9		= 0;
	uint32 unknown10	= 0x02FD0101;		//0x02FD0101;		Changes to +1 when equals 0x102
	uint32 unknown11	= 0;				//2;

	uint32 unknown12	= 0;
	uint32 unknown13	= 0;
	uint32 unknown14	= 0x01010101;		//? 0x02020202 also possible
	uint32 unknown15	= 0;

	uint32 unknown16	= 0;
	uint32 unknown17	= 0;
	uint32 unknown18	= 0;
	uint32 unknown19	= 0x2710;			//10000

	uint32 unknown20[8];
};
static_assert(sizeof(ITEMBLOCK) == 0x70, "ITEMBLOCK size must be 112 bytes.");
