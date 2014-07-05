#pragma once

#include "PalleonEngine.h"
#include "Singleton.h"
#include "../dataobjects/SheetData.h"

class CWeaponVars : public CSingleton<CWeaponVars>
{
public:
	enum
	{
		MATERIAL_COUNT = 9,
		PGRP_COUNT = 8,
	};

	struct MATERIALINFO
	{
		CVector4	diffuseColor = CVector4(1000.f, 1000.f, 1000.f, 1000.f);
		CVector4	multiDiffuseColor = CVector4(1000.f, 1000.f, 1000.f, 1000.f);
		CVector4	specularColor = CVector4(1000.f, 1000.f, 1000.f, 1000.f);
		CVector4	multiSpecularColor = CVector4(1000.f, 1000.f, 1000.f, 1000.f);
		float		shininess = 1000.f;
		float		multiShininess = 1000.f;
	};

	struct WEAPONVAR
	{
		MATERIALINFO	materials[MATERIAL_COUNT];
		uint32			polyGroupState = 0;
		uint32			textureId = 0;
	};

					CWeaponVars();
	virtual			~CWeaponVars();

	WEAPONVAR		GetVarForId(uint32) const;

private:
	CSheetData		m_varWepSheetData;
};
