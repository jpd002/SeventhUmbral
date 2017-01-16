#pragma once

#include "Singleton.h"
#include "ItemVars.h"

#define VAREQUIP_SHEET_ID		0x03A70001

class CEquipVars :
	public CItemVars<4, VAREQUIP_SHEET_ID>,
	public CSingleton<CEquipVars>
{

};
