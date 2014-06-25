#include "WeaponVars.h"
#include "StdStreamUtils.h"
#include "../dataobjects/FileManager.h"
#include "../dataobjects/HalfFloat.h"

#define VARWEP_SHEET_ID		0x03A70309

CWeaponVars::CWeaponVars()
{
	auto sheetFilePath = CFileManager::GetResourcePath(VARWEP_SHEET_ID);
	auto sheetStream = Framework::CreateInputStdStream(sheetFilePath.native());
	auto varEquipSheet = CSheet::Create(sheetStream);
	m_varWepSheetData = CSheetData::Create(varEquipSheet, 0,
		[](uint32 fileId)
		{
			auto filePath = CFileManager::GetResourcePath(fileId);
			return new Framework::CStdStream(Framework::CreateInputStdStream(filePath.native()));
		}
	);
}

CWeaponVars::~CWeaponVars()
{

}

CWeaponVars::WEAPONVAR CWeaponVars::GetVarForId(uint32 varId) const
{
	WEAPONVAR result;
	auto row = m_varWepSheetData.GetRow(varId);
	for(unsigned int i = 0; i < MATERIAL_COUNT; i++)
	{
		auto& matInfo = result.materials[i];
		matInfo.diffuseColor = CVector4(
			CHalfFloat::ToFloat(row[(19 * i) + 0].GetValue16()),
			CHalfFloat::ToFloat(row[(19 * i) + 1].GetValue16()),
			CHalfFloat::ToFloat(row[(19 * i) + 2].GetValue16()),
			1000.f
		);
		matInfo.multiDiffuseColor = CVector4(
			CHalfFloat::ToFloat(row[(19 * i) + 3].GetValue16()),
			CHalfFloat::ToFloat(row[(19 * i) + 4].GetValue16()),
			CHalfFloat::ToFloat(row[(19 * i) + 5].GetValue16()),
			CHalfFloat::ToFloat(row[(19 * i) + 6].GetValue16())
		);
		matInfo.specularColor = CVector4(
			CHalfFloat::ToFloat(row[(19 * i) + 7].GetValue16()),
			CHalfFloat::ToFloat(row[(19 * i) + 8].GetValue16()),
			CHalfFloat::ToFloat(row[(19 * i) + 9].GetValue16()),
			1000.f
		);
		matInfo.multiSpecularColor = CVector4(
			CHalfFloat::ToFloat(row[(19 * i) + 10].GetValue16()),
			CHalfFloat::ToFloat(row[(19 * i) + 11].GetValue16()),
			CHalfFloat::ToFloat(row[(19 * i) + 12].GetValue16()),
			1000.f
		);
		matInfo.shininess = CHalfFloat::ToFloat(row[(19 * i) + 13].GetValue16());
		matInfo.multiShininess = CHalfFloat::ToFloat(row[(19 * i) + 14].GetValue16());
	}
	result.textureId = row[203].GetValue8();
	return result;
}
