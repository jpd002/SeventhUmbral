#pragma once

#include "StdStreamUtils.h"
#include "math/Vector4.h"
#include "../../dataobjects/SheetData.h"
#include "../../dataobjects/FileManager.h"
#include "../../dataobjects/HalfFloat.h"

template <int MaterialCount, uint32 VAR_SHEET_ID>
class CItemVars
{
public:
	enum
	{
		MATERIAL_COUNT = MaterialCount,
		PGRP_COUNT = 32,
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

	struct ITEMVAR
	{
		MATERIALINFO	materials[MATERIAL_COUNT];
		uint32			polyGroupState = 0;
		uint32			textureId = 0;
	};

	CItemVars()
	{
		auto sheetFilePath = CFileManager::GetResourcePath(VAR_SHEET_ID);
		auto sheetStream = Framework::CreateInputStdStream(sheetFilePath.native());
		auto varEquipSheet = CSheet::Create(sheetStream);
		m_varSheetData = CSheetData::Create(varEquipSheet, 0,
			[](uint32 fileId)
			{
				auto filePath = CFileManager::GetResourcePath(fileId);
				return new Framework::CStdStream(Framework::CreateInputStdStream(filePath.native()));
			}
		);
	}

	virtual ~CItemVars()
	{
	
	}

	ITEMVAR GetVarForId(uint32 varId) const
	{
		static const unsigned int pgrpBase = MATERIAL_COUNT * 19;
		static const unsigned int textureIdCol = pgrpBase + PGRP_COUNT;

		ITEMVAR result;
		auto row = m_varSheetData.GetRow(varId);
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
		for(unsigned int i = 0; i < PGRP_COUNT; i++)
		{
			if(row[pgrpBase + i].GetValue8() != 0)
			{
				result.polyGroupState |= (1 << i);
			}
		}
		result.textureId = row[textureIdCol].GetValue8();
		return result;
	}

private:
	CSheetData m_varSheetData;
};
