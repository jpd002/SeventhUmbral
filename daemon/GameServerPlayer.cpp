#include <stdio.h>
#include <map>
#include <thread>
#include "GameServerPlayer.h"
#include "PacketUtils.h"
#include "GameServer_Login.h"
#include "GameServer_MoveOutOfRoom.h"
#include "Character.h"
#include "PathUtils.h"
#include "StdStreamUtils.h"

#include "SetInitialPositionPacket.h"
#include "SetWeatherPacket.h"
#include "SetMusicPacket.h"
#include "SetMapPacket.h"

#include "CompositePacket.h"

#define LOGNAME "GameServerPlayer"

#define PLAYER_ID	0x029B2941

#define INITIAL_POSITION_GRIDANIA_INN	   58.92f,   4.00f, -1219.07f, 0.52f
#define INITIAL_POSITION_MOR_DHONA		 -208.08f,  19.00f,  -669.79f, 0.00f
#define INITIAL_POSITION_COERTHAS		  219.59f, 302.00f,  -246.00f, 0.00f
#define INITIAL_POSITION_NOSCEA			 1111.33f,  54.00f,  -456.08f, 0.00f
#define INITIAL_POSITION_THANALAN		 1247.79f, 264.10f,  -562.08f, 0.00f

static const uint8 g_chocoboRider1[] =
{
	0x01, 0x00, 0x00, 0x00, 0x50, 0x03, 0x08, 0x00, 0xAF, 0x4D, 0x95, 0xEE, 0x3B, 0x01, 0x00, 0x00, 

	0x28, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
	0x14, 0x00, 0x97, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD4, 0xEA, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 

	0x48, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
	0x14, 0x00, 0x3C, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD4, 0xEA, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x41, 0x29, 0x9B, 0x02, 0x62, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 

	0xA8, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
	0x14, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD4, 0xEA, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x40, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x10, 0x41, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x41, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

	0xA8, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
	0x14, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD4, 0xEA, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x40, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x10, 0x41, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x41, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

	0xA8, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
	0x14, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD4, 0xEA, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x40, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x10, 0x41, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x41, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

	0x30, 0x00, 0x03, 0x00, 0x01, 0x00, 0xF8, 0x5F, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
	0x14, 0x00, 0x57, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD4, 0xEA, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x41, 0x29, 0x9B, 0x02, 0x01, 0x00, 0xF8, 0x5F, 0x91, 0x65, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 

	0x58, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
	0x14, 0x00, 0x39, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD4, 0xEA, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x41, 0x29, 0x9B, 0x02, 0x62, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0x00, 0x00, 0x00, 0xEE, 0x2E, 0x10, 0x08, 0x41, 0x29, 0x9B, 0x02, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 

	0x50, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
	0x14, 0x00, 0x31, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD4, 0xEA, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0x41, 0x29, 0x9B, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x6F, 0x6D, 0x6D, 0x61, 0x6E, 0x64, 
	0x46, 0x6F, 0x72, 0x63, 0x65, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF2, 0xD4, 0x09, 0x58, 0x8E, 0x15, 0x0A, 
};

static const uint8 g_chocoboRider2[] =
{
	0x01, 0x01, 0x00, 0x00, 0xC8, 0x02, 0x01, 0x00, 0x6B, 0x4E, 0x95, 0xEE, 0x3B, 0x01, 0x00, 0x00, 

	0xB8, 0x02, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
	0x14, 0x00, 0x8D, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD4, 0xEA, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
	0xAC, 0xE9, 0x77, 0x00, 0x00, 0x00, 0x00, 0x80, 0x11, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x41, 0x29, 0x9B, 0x02, 0x00, 0x00, 0x00, 0x00, 0x09, 0x79, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xCE, 0x83, 0xC1, 0x43, 0xF0, 0x47, 0xA5, 0x40, 0x17, 0xA5, 0x5A, 0xC4, 
	0x00, 0x04, 0x92, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

CGameServerPlayer::CGameServerPlayer(SOCKET clientSocket)
: m_clientSocket(clientSocket)
, m_disconnect(false)
, m_alreadyMovedOutOfRoom(false)
, m_zoneMasterCreated(false)
{
	PrepareInitialPackets();
}

CGameServerPlayer::~CGameServerPlayer()
{

}

bool CGameServerPlayer::IsConnected() const
{
	return !m_disconnect;
}

static PacketData GetMotd()
{
	std::vector<const char*> messages;
	messages.push_back("Welcome to the Seventh Umbral Server");
	messages.push_back("Compiled on: " __DATE__);
	messages.push_back("---------------");

	const static uint8 packetHeader[] =
	{
		0x01, 0x00, 0x00, 0x00, 0x58, 0x02, 0x06, 0x00, 0xF9, 0x18, 0x8B, 0xEE, 0x3B, 0x01, 0x00, 0x00
	};

	PacketData outgoingPacket;
	outgoingPacket.insert(std::end(outgoingPacket), std::begin(packetHeader), std::end(packetHeader));

	for(auto message : messages)
	{
		uint8 messagePacket[0x248] = 
		{
			0x48, 0x02, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
			0x14, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0xE8, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};

		memcpy(messagePacket + 0x44, message, strlen(message) + 1);

		outgoingPacket.insert(std::end(outgoingPacket), std::begin(messagePacket), std::end(messagePacket));
	}

	uint32 packetSize = outgoingPacket.size();
	assert(packetSize < 0x10000);

	*reinterpret_cast<uint16*>(outgoingPacket.data() + 0x4) = static_cast<uint16>(packetSize);

	return outgoingPacket;
}

static PacketData GetCharacterInfo()
{
	PacketData outgoingPacket(std::begin(g_client0_login8), std::end(g_client0_login8));

	{
		const uint32 setInitialPositionBase = 0x320;
		CSetInitialPositionPacket setInitialPosition;
		setInitialPosition.SetSourceId(PLAYER_ID);
		setInitialPosition.SetTargetId(PLAYER_ID);
		setInitialPosition.SetX(157.55f);
		setInitialPosition.SetY(0);
		setInitialPosition.SetZ(165.05f);
		setInitialPosition.SetAngle(-1.53f);
		auto setInitialPositionPacket = setInitialPosition.ToPacketData();

		memcpy(outgoingPacket.data() + setInitialPositionBase, setInitialPositionPacket.data(), setInitialPositionPacket.size());
	}

	CCharacter character;
	auto personalDataPath = Framework::PathUtils::GetPersonalDataPath();
	auto characterPath = personalDataPath / "ffxivd_character.xml";
	if(boost::filesystem::exists(characterPath))
	{
		auto inputStream = Framework::CreateInputStdStream(characterPath.native());
		character.Load(inputStream);
	}

	const uint32 characterInfoBase = 0x368;

	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x20]) = CCharacter::GetModelFromTribe(character.tribe);
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x28]) = character.size;
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x30]) = character.GetColorInfo();
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x38]) = character.GetFaceInfo();
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x40]) = character.hairStyle << 10;
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x48]) = character.voice;
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x50]) = 0;						//weapon

	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x88]) = character.headGear;		//headGear
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x90]) = character.bodyGear;		//bodyGear
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x98]) = character.legsGear;		//legsGear
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0xA0]) = character.handsGear;	//handsGear
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0xA8]) = character.feetGear;		//feetGear

	//Insert character name
	for(unsigned int i = 0; i < character.name.size(); i++)
	{
		outgoingPacket[characterInfoBase + 0x14C + i] = character.name[i];
	}
	outgoingPacket[characterInfoBase + 0x14C + character.name.size()] = 0;

	return outgoingPacket;
}

void CGameServerPlayer::QueuePacket(const PacketData& packet)
{
	m_packetQueue.push_back(packet);
}

void CGameServerPlayer::Update()
{
	//Write to socket
	{
		int totalSent = 0;
		while(!m_packetQueue.empty())
		{
			if(totalSent >= 0x1000)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				break;
			}
			const auto& nextPacket = m_packetQueue.front();
			int sent = send(m_clientSocket, reinterpret_cast<const char*>(nextPacket.data()), nextPacket.size(), 0);
			assert(sent == nextPacket.size());
			m_packetQueue.pop_front();
			totalSent += sent;
		}
	}

	//Read from socket
	{
		static const unsigned int maxPacketSize = 0x10000;
		uint8 incomingPacket[maxPacketSize];
		int read = recv(m_clientSocket, reinterpret_cast<char*>(incomingPacket), maxPacketSize, 0);
		if(read == 0)
		{
			//Client disconnected
			printf("%s: Client disconnected.\r\n", LOGNAME);
			m_disconnect = true;
			return;
		}
		if(read > 0)
		{
			m_incomingStream.Write(incomingPacket, read);
		}
	}

	if(CPacketUtils::HasPacket(m_incomingStream))
	{
		auto incomingPacket = CPacketUtils::ReadPacket(m_incomingStream);
		auto subPackets = CPacketUtils::SplitPacket(incomingPacket);
		bool killConnection = false;

		for(const auto& subPacket : subPackets)
		{
			uint16 commandId = CPacketUtils::GetSubPacketCommand(subPacket);
			switch(commandId)
			{
			case 0x0001:
				ProcessKeepAlive(subPacket);
				break;
			case 0x0003:
				ProcessChat(subPacket);
				break;
			case 0x00CA:
				ProcessSetPlayerPosition(subPacket);
				break;
			case 0x00CD:
				ProcessSetSelection(subPacket);
				break;
			case 0x012D:
				ProcessScriptCommand(subPacket);
				break;
			case 0x012E:
				ProcessScriptResult(subPacket);
				break;
			default:
				printf("%s: Unknown command 0x%0.4X received.\r\n", LOGNAME, commandId);
				break;
			}
		}
	}
}

void CGameServerPlayer::PrepareInitialPackets()
{
	QueuePacket(PacketData(std::begin(g_client0_login1), std::end(g_client0_login1)));
	QueuePacket(PacketData(std::begin(g_client0_login2), std::end(g_client0_login2)));
	QueuePacket(PacketData(std::begin(g_client0_login3), std::end(g_client0_login3)));
	QueuePacket(PacketData(std::begin(g_client0_login4), std::end(g_client0_login4)));
	QueuePacket(PacketData(GetMotd()));
	QueuePacket(PacketData(std::begin(g_client0_login7), std::end(g_client0_login7)));
	QueuePacket(PacketData(GetCharacterInfo()));
	QueuePacket(PacketData(std::begin(g_client0_login9), std::end(g_client0_login9)));
	QueuePacket(PacketData(std::begin(g_client0_login10), std::end(g_client0_login10)));
	QueuePacket(PacketData(std::begin(g_client0_login11), std::end(g_client0_login11)));
	QueuePacket(PacketData(std::begin(g_client0_login12), std::end(g_client0_login12)));
	QueuePacket(PacketData(std::begin(g_client0_login13), std::end(g_client0_login13)));
	QueuePacket(PacketData(std::begin(g_client0_login14), std::end(g_client0_login14)));
}

void CGameServerPlayer::ProcessKeepAlive(const PacketData& subPacket)
{
	//Some keep alive thing? (only time is updated here)
	uint32 clientTime = *reinterpret_cast<const uint32*>(&subPacket[0x18]);
	uint32 moreTime = *reinterpret_cast<const uint32*>(&subPacket[0x20]);

	uint8 keepAlivePacket[0x50] =
	{
		0x01, 0x00, 0x00, 0x00, 0x50, 0x00, 0x01, 0x00, 0xEF, 0xCB, 0xA4, 0xEE, 0x3B, 0x01, 0x00, 0x00,
		0x40, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9b, 0x02, 0x41, 0x29, 0x9b, 0x02, 0x00, 0xe0, 0xd2, 0xfe,
		0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcb, 0xee, 0xe0, 0x50, 0x00, 0x00, 0x00, 0x00,
		0x4a, 0x18, 0x9c, 0x0a, 0x4d, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	*reinterpret_cast<uint32*>(&keepAlivePacket[0x28]) = clientTime;
	*reinterpret_cast<uint32*>(&keepAlivePacket[0x30]) = moreTime;

	QueuePacket(PacketData(std::begin(keepAlivePacket), std::end(keepAlivePacket)));
}

void CGameServerPlayer::ProcessChat(const PacketData& subPacket)
{
	const char* chatText = reinterpret_cast<const char*>(subPacket.data() + 0x3C);

	static std::map<std::string, uint32> weatherCommands;
	if(weatherCommands.empty())
	{
		weatherCommands["weather_clear"]		= CSetWeatherPacket::WEATHER_CLEAR;
		weatherCommands["weather_fine"]			= CSetWeatherPacket::WEATHER_FINE;
		weatherCommands["weather_cloudy"]		= CSetWeatherPacket::WEATHER_CLOUDY;
		weatherCommands["weather_foggy"]		= CSetWeatherPacket::WEATHER_FOGGY;
		weatherCommands["weather_blustery"]		= CSetWeatherPacket::WEATHER_BLUSTERY;
		weatherCommands["weather_rainy"]		= CSetWeatherPacket::WEATHER_RAINY;
		weatherCommands["weather_stormy"]		= CSetWeatherPacket::WEATHER_STORMY;
		weatherCommands["weather_sandy"]		= CSetWeatherPacket::WEATHER_SANDY;
		weatherCommands["weather_gloomy"]		= CSetWeatherPacket::WEATHER_GLOOMY;
		weatherCommands["weather_dalamund"]		= CSetWeatherPacket::WEATHER_DALAMUND;
	}

	auto weatherCommandIterator = weatherCommands.find(chatText);
	if(weatherCommandIterator != std::end(weatherCommands))
	{
		CCompositePacket result;
		
		{
			CSetWeatherPacket packet;
			packet.SetSourceId(PLAYER_ID);
			packet.SetTargetId(PLAYER_ID);
			packet.SetWeatherId(weatherCommandIterator->second);
			result.AddPacket(packet.ToPacketData());
		}

		QueuePacket(result.ToPacketData());
	}
	else if(!strcmp(chatText, "teleport_mordhona"))
	{
		SendTeleportSequence(CSetMapPacket::MAP_MORDHONA, CSetMusicPacket::MUSIC_MORDHONA, INITIAL_POSITION_MOR_DHONA);
	}
	else if(!strcmp(chatText, "teleport_coerthas"))
	{
		SendTeleportSequence(CSetMapPacket::MAP_COERTHAS, CSetMusicPacket::MUSIC_COERTHAS, INITIAL_POSITION_COERTHAS);
	}
	else if(!strcmp(chatText, "teleport_thanalan"))
	{
		SendTeleportSequence(CSetMapPacket::MAP_THANALAN, CSetMusicPacket::MUSIC_THANALAN, INITIAL_POSITION_THANALAN);
	}
	else if(!strcmp(chatText, "teleport_lanocsea"))
	{
		SendTeleportSequence(CSetMapPacket::MAP_NOSCEA, CSetMusicPacket::MUSIC_NOSCEA, INITIAL_POSITION_NOSCEA);
	}
	else if(!strcmp(chatText, "teleport_gridania"))
	{
		SendTeleportSequence(CSetMapPacket::MAP_BLACKSHROUD, CSetMusicPacket::MUSIC_GRIDANIA, INITIAL_POSITION_GRIDANIA_INN);
	}

//	printf("%s\r\n", chatText);
}

void CGameServerPlayer::ProcessSetPlayerPosition(const PacketData& subPacket)
{
	//Some keep alive thing?
	uint32 clientTime = *reinterpret_cast<const uint32*>(&subPacket[0x18]);
	float posX = *reinterpret_cast<const float*>(&subPacket[0x28]);
	float posY = *reinterpret_cast<const float*>(&subPacket[0x2C]);
	float posZ = *reinterpret_cast<const float*>(&subPacket[0x30]);

//	printf("%s: Keeping Alive. Time: 0x%0.8X, Pos: (X: %f, Y: %f, Z: %f).\r\n",
//		LOGNAME, clientTime, posX, posY, posZ);
}

void CGameServerPlayer::ProcessSetSelection(const PacketData& subPacket)
{
	uint32 selectedId = *reinterpret_cast<const uint32*>(&subPacket[0x20]);
	uint32 lockOnId = *reinterpret_cast<const uint32*>(&subPacket[0x24]);

	printf("%s: Selected Id: 0x%0.8X, Lock On Id: 0x%0.8X\r\n", LOGNAME, selectedId, lockOnId);
}

void CGameServerPlayer::ProcessScriptCommand(const PacketData& subPacket)
{
	uint32 clientTime = *reinterpret_cast<const uint32*>(&subPacket[0x18]);
	uint32 sourceId = *reinterpret_cast<const uint32*>(&subPacket[0x20]);
	uint32 targetId = *reinterpret_cast<const uint32*>(&subPacket[0x24]);
	const char* commandName = reinterpret_cast<const char*>(subPacket.data()) + 0x31;

	printf("%s: ProcessScriptCommand: %s Source Id = 0x%0.8X, Target Id = 0x%0.8X.\r\n", LOGNAME, commandName, sourceId, targetId);

	//printf("%s\r\n", CPacketUtils::DumpPacket(subPacket).c_str());

	if(!strcmp(commandName, "commandRequest"))
	{
		//commandRequest (emote, changing equipment, ...)

//		{
//			static int currentFileNumber = 0;
//			std::string fileName = std::string("dump_") + std::to_string(currentFileNumber++);
//			FILE* output = fopen(fileName.c_str(), "wb");
//			fprintf(output, "%s\r\n", CPacketUtils::DumpPacket(subPacket).c_str());
//			fclose(output);
//		}

		switch(targetId)
		{
		case 0xA0F02EE9:
			ScriptCommand_EquipItem(subPacket, clientTime);
			break;
		case 0xA0F05E26:
			ScriptCommand_Emote(subPacket, clientTime);
			break;
		case 0xA0F05EA2:
			//Trash Item
			ScriptCommand_TrashItem(subPacket, clientTime);
			break;
		default:
			printf("%s: Unknown target id (0x%0.8X).\r\n", LOGNAME, targetId);
			break;
		}
	}
	else if(!strcmp(commandName, "commandContent"))
	{
		//Called when trying to teleport
	}
	else if(!strcmp(commandName, "commandForced"))
	{
		QueuePacket(PacketData(std::begin(g_chocoboRider1), std::end(g_chocoboRider1)));
		QueuePacket(PacketData(std::begin(g_chocoboRider2), std::end(g_chocoboRider2)));
	}
	else if(!strcmp(commandName, "talkDefault"))
	{
		switch(targetId)
		{
		case 0x47A00007:
			//Talking to the door inside the room
			{
				static const uint8 commandRequestPacket[] =
				{
					0x01, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x01, 0x00, 0x52, 0xE2, 0xA4, 0xEE, 0x3B, 0x01, 0x00, 0x00,
					0xb0, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9b, 0x02, 0x41, 0x29, 0x9b, 0x02, 0x00, 0xe0, 0xd2, 0xfe,
					0x14, 0x00, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00, 0xd3, 0xe9, 0xe0, 0x50, 0x00, 0x00, 0x00, 0x00,
					0x41, 0x29, 0x9b, 0x02, 0x07, 0x00, 0xa0, 0x47, 0x01, 0x74, 0x61, 0x6c, 0x6b, 0x44, 0x65, 0x66,
					0x61, 0x75, 0x6c, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x65, 0x6c, 0x65, 0x67, 0x61, 0x74,
					0x65, 0x45, 0x76, 0x65, 0x6e, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x02, 0x9b, 0x29, 0x41, 0x06, 0xa0,
					0xf1, 0xaf, 0xcd, 0x02, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x54, 0x61, 0x6c, 0x6b, 0x57,
					0x69, 0x74, 0x68, 0x49, 0x6e, 0x6e, 0x5f, 0x45, 0x78, 0x69, 0x74, 0x44, 0x6f, 0x6f, 0x72, 0x00,
					0x05, 0x05, 0x05, 0x05, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xe8, 0x4e, 0x40, 0x00, 0x00, 0x00,
				};

				QueuePacket(PacketData(std::begin(commandRequestPacket), std::end(commandRequestPacket)));
			}
			break;
		default:
			//Talking Test (doesn't work)
			{
				static const uint8 commandRequestPacket[] =
				{
					0x01, 0x01, 0x00, 0x00, 0xC0, 0x00, 0x01, 0x00, 0xD2, 0x16, 0x9E, 0xEE, 0x3B, 0x01, 0x00, 0x00, 
					0xB0, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
					0x14, 0x00, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0xED, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
					0x41, 0x29, 0x9B, 0x02, 0x82, 0x00, 0x70, 0x46, 0x01, 0x74, 0x61, 0x6C, 0x6B, 0x44, 0x65, 0x66, 
					0x61, 0x75, 0x6C, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x77, 0x69, 0x74, 0x63, 0x68, 0x45, 
					0x76, 0x65, 0x6E, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xA0, 0xF1, 0xAF, 0xCD, 0x06, 0xA0, 
					0xF1, 0xB4, 0x00, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
					0x00, 0x00, 0x03, 0xF1, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6C, 0xB8, 0x45, 0x40, 0x00, 0x00, 0x00, 
				};

				QueuePacket(PacketData(std::begin(commandRequestPacket), std::end(commandRequestPacket)));
			}
			break;
		}
	}
	else
	{
		//Anything else will probably crash, so just bail
		m_disconnect = true;
	}
}

void CGameServerPlayer::ScriptCommand_EquipItem(const PacketData& subPacket, uint32 clientTime)
{
	uint32 itemId = *reinterpret_cast<const uint32*>(&subPacket[0x6E]);

	printf("%s: Equipping Item: 0x%0.8X\r\n", LOGNAME, itemId);

	//REMOVE
	SendTeleportSequence(CSetMapPacket::MAP_BLACKSHROUD, CSetMusicPacket::MUSIC_GRIDANIA, INITIAL_POSITION_GRIDANIA_INN);
	//REMOVE
}

void CGameServerPlayer::ScriptCommand_Emote(const PacketData& subPacket, uint32 clientTime)
{
	uint8 emoteId = subPacket[0x55];

	printf("%s: Executing Emote 0x%0.2X\r\n", LOGNAME, emoteId);

	uint8 commandRequestPacket[0x40] =
	{
		0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x52, 0xE2, 0xA4, 0xEE, 0x3B, 0x01, 0x00, 0x00,
		0x30, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9b, 0x02, 0x41, 0x29, 0x9b, 0x02, 0x00, 0xe0, 0xd2, 0xfe,
		0x14, 0x00, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd1, 0xee, 0xe0, 0x50, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xb0, 0x00, 0x05, 0x41, 0x29, 0x9b, 0x02, 0x6e, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};

	//In: 0x6F, Out: (0x0500B000, 0x526E) -> Dance
	//In: 0x??, Out: (0x5000C000, 0x????) -> Angry Pointing
	//In: 0x??, Out: (0x5000D000, 0x????) -> Snooze
	//In: 0x??, Out: (0x5000E000, 0x????) -> Frustrated
	//In: 0x??, Out: (0x5000F000, 0x????) -> Military Sign
	//In: 0x??, Out: (0x50011000, 0x????) -> Shrug
	//In: 0x??, Out: (0x50012000, 0x????) -> Success Baby
	//In: 0x77, Out: (0x05013000, 0x52BE) -> Kneel
	//In: 0x??, Out: (0x50014000, 0x????) -> Chuckle
	//In: 0x??, Out: (0x50015000, 0x????) -> Laugh
	//In: 0x??, Out: (0x50016000, 0x????) -> Look
	//In: 0x??, Out: (0x50018000, 0x????) -> No
	//In: 0x??, Out: (0x50019000, 0x????) -> Never
					
	uint32 animationId = 0x0500B000;
	uint32 descriptionId = 0x526E;

	//Wrong emotes
	//gcsalute		-> grovel
	//grovel		-> serpent salute
	//blowkiss		-> disappointed
	//pray			-> firedance
	//airquote		-> pray
	//pose			-> blowkiss
	//happy			-> maelstorm salute
	//disappointed	-> pose

	if(emoteId >= 0x64 && emoteId < 0xA0)
	{
		animationId = 0x05000000 + ((emoteId - 0x64) << 12);
	}
/*
	switch(emoteId)
	{
	case 0x6A:		//Cheer
		animationId = 0x05006000;
		break;
	case 0x6F:		//Dance
		animationId = 0x0500B000;
		break;
	case 0x71:		//Doze
		animationId = 0x0500D000;
		break;
	case 0x75:		//Huh
		animationId = 0x05011000;
		break;
	case 0x78:		//Chuckle
		animationId = 0x05014000;
		break;
	case 0x79:		//Laugh
		animationId = 0x05015000;
		break;
	}
*/

	*reinterpret_cast<uint32*>(&commandRequestPacket[0x28]) = clientTime;
	*reinterpret_cast<uint32*>(&commandRequestPacket[0x30]) = animationId;
	*reinterpret_cast<uint32*>(&commandRequestPacket[0x38]) = descriptionId;

//	printf("Anim Id = 0x%0.8X, Desc Id = 0x%0.8X\r\n", animationId, descriptionId);
//	animationId += 0x1000;
//	descriptionId += 1;

	QueuePacket(PacketData(std::begin(commandRequestPacket), std::end(commandRequestPacket)));
}

void CGameServerPlayer::ScriptCommand_TrashItem(const PacketData& subPacket, uint32 clientTime)
{
	uint32 itemId = *reinterpret_cast<const uint32*>(&subPacket[0x6A]);
	printf("%s: Trashing Item: 0x%0.8X\r\n", LOGNAME, itemId);
}

void CGameServerPlayer::ProcessScriptResult(const PacketData& subPacket)
{
	uint32 someId1 = *reinterpret_cast<const uint32*>(&subPacket[0x2C]);
	uint32 someId2 = *reinterpret_cast<const uint32*>(&subPacket[0x30]);
	uint32 someId3 = *reinterpret_cast<const uint32*>(&subPacket[0x34]);

	printf("%s: ProcessScriptResult: Id1 = 0x%0.8X, Id2 = 0x%0.8X, Id3 = 0x%0.8X.\r\n", LOGNAME, someId1, someId2, someId3);

	if(!m_alreadyMovedOutOfRoom)
	{
		printf("%s: Command 0x12E: Moving out of room\r\n", LOGNAME);

		SendTeleportSequence(CSetMapPacket::MAP_BLACKSHROUD, CSetMusicPacket::MUSIC_GRIDANIA, INITIAL_POSITION_GRIDANIA_INN);

		m_alreadyMovedOutOfRoom = true;
	}
}

void CGameServerPlayer::SendTeleportSequence(uint32 levelId, uint32 musicId, float x, float y, float z, float angle)
{
	QueuePacket(PacketData(std::begin(g_client0_moor1), std::end(g_client0_moor1)));
	QueuePacket(PacketData(std::begin(g_client0_moor2), std::end(g_client0_moor2)));
	QueuePacket(PacketData(std::begin(g_client0_moor3), std::end(g_client0_moor3)));
	QueuePacket(PacketData(std::begin(g_client0_moor4), std::end(g_client0_moor4)));
	QueuePacket(PacketData(std::begin(g_client0_moor5), std::end(g_client0_moor5)));
	QueuePacket(PacketData(std::begin(g_client0_moor6), std::end(g_client0_moor6)));
	QueuePacket(PacketData(std::begin(g_client0_moor7), std::end(g_client0_moor7)));
	QueuePacket(PacketData(std::begin(g_client0_moor8), std::end(g_client0_moor8)));

	QueuePacket(PacketData(std::begin(g_client0_moor9), std::end(g_client0_moor9)));

	{
		CCompositePacket result;

		{
			CSetMusicPacket packet;
			packet.SetSourceId(PLAYER_ID);
			packet.SetTargetId(PLAYER_ID);
			packet.SetMusicId(musicId);
			result.AddPacket(packet.ToPacketData());
		}

		{
			CSetWeatherPacket packet;
			packet.SetSourceId(PLAYER_ID);
			packet.SetTargetId(PLAYER_ID);
			packet.SetWeatherId(CSetWeatherPacket::WEATHER_CLEAR);
			result.AddPacket(packet.ToPacketData());
		}

		{
			CSetMapPacket packet;
			packet.SetSourceId(PLAYER_ID);
			packet.SetTargetId(PLAYER_ID);
			packet.SetMapId(levelId);
			result.AddPacket(packet.ToPacketData());
		}

		QueuePacket(result.ToPacketData());
	}

	QueuePacket(PacketData(std::begin(g_client0_moor11), std::end(g_client0_moor11)));
	QueuePacket(PacketData(std::begin(g_client0_moor12), std::end(g_client0_moor12)));

	{
		PacketData outgoingPacket(std::begin(g_client0_moor13), std::end(g_client0_moor13));

		{
			const uint32 setInitialPositionBase = 0x360;

			CSetInitialPositionPacket setInitialPosition;
			setInitialPosition.SetSourceId(PLAYER_ID);
			setInitialPosition.SetTargetId(PLAYER_ID);
			setInitialPosition.SetX(x);
			setInitialPosition.SetY(y);
			setInitialPosition.SetZ(z);
			setInitialPosition.SetAngle(angle);
			auto setInitialPositionPacket = setInitialPosition.ToPacketData();

			memcpy(outgoingPacket.data() + setInitialPositionBase, setInitialPositionPacket.data(), setInitialPositionPacket.size());
		}

		QueuePacket(outgoingPacket);
	}

	QueuePacket(PacketData(std::begin(g_client0_moor14), std::end(g_client0_moor14)));
	QueuePacket(PacketData(std::begin(g_client0_moor15), std::end(g_client0_moor15)));
	QueuePacket(PacketData(std::begin(g_client0_moor16), std::end(g_client0_moor16)));
	QueuePacket(PacketData(std::begin(g_client0_moor17), std::end(g_client0_moor17)));
	QueuePacket(PacketData(std::begin(g_client0_moor18), std::end(g_client0_moor18)));
	QueuePacket(PacketData(std::begin(g_client0_moor19), std::end(g_client0_moor19)));
	QueuePacket(PacketData(std::begin(g_client0_moor20), std::end(g_client0_moor20)));
	QueuePacket(PacketData(std::begin(g_client0_moor21), std::end(g_client0_moor21)));
	//QueuePacket(PacketData(std::begin(g_client0_moor22), std::end(g_client0_moor22)));
	
	if(!m_zoneMasterCreated)
	{
		//Zone Master
		QueuePacket(PacketData(std::begin(g_client0_moor23), std::end(g_client0_moor23)));

	/*
		QueuePacket(PacketData(std::begin(g_client0_moor24), std::end(g_client0_moor24)));
		QueuePacket(PacketData(std::begin(g_client0_moor25), std::end(g_client0_moor25)));

		QueuePacket(PacketData(std::begin(g_client0_moor26), std::end(g_client0_moor26)));
		QueuePacket(PacketData(std::begin(g_client0_moor27), std::end(g_client0_moor27)));
		QueuePacket(PacketData(std::begin(g_client0_moor28), std::end(g_client0_moor28)));
		QueuePacket(PacketData(std::begin(g_client0_moor29), std::end(g_client0_moor29)));

		QueuePacket(PacketData(std::begin(g_client0_moor30), std::end(g_client0_moor30)));
		QueuePacket(PacketData(std::begin(g_client0_moor31), std::end(g_client0_moor31)));

		QueuePacket(PacketData(std::begin(g_client0_moor32), std::end(g_client0_moor32)));
		QueuePacket(PacketData(std::begin(g_client0_moor33), std::end(g_client0_moor33)));
		QueuePacket(PacketData(std::begin(g_client0_moor34), std::end(g_client0_moor34)));
		QueuePacket(PacketData(std::begin(g_client0_moor35), std::end(g_client0_moor35)));
		QueuePacket(PacketData(std::begin(g_client0_moor36), std::end(g_client0_moor36)));
		QueuePacket(PacketData(std::begin(g_client0_moor37), std::end(g_client0_moor37)));
	*/
		//Enables chat?
	//	QueuePacket(PacketData(std::begin(g_client0_moor38), std::end(g_client0_moor38)));

		{
			CCompositePacket packet;
			packet.AddPacket(PacketData(std::begin(g_client0_moor38), std::end(g_client0_moor38)));
			QueuePacket(packet.ToPacketData());
		}

	//	QueuePacket(PacketData(std::begin(g_client0_moor39), std::end(g_client0_moor39)));

	//	QueuePacket(PacketData(std::begin(g_client0_moor40), std::end(g_client0_moor40)));

		m_zoneMasterCreated = true;
	}

}
