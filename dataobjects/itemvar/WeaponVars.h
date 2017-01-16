#pragma once

#include "Singleton.h"
#include "ItemVars.h"

#define VARWEP_SHEET_ID		0x03A70309

class CWeaponVars :
	public CItemVars<9, VARWEP_SHEET_ID>,
	public CSingleton<CWeaponVars>
{

};
