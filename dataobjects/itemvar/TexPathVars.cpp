#include "StdStreamUtils.h"
#include "TexPathVars.h"
#include "../../dataobjects/FileManager.h"

#define VAR_SHEET_ID 0x03A70305

CTexPathVars::CTexPathVars()
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

CTexPathVars::~CTexPathVars()
{

}

uint32 CTexPathVars::GetTexturePathId(uint32 textureId, uint32 race) const
{
	if(textureId == 0) return race;
	if(race == 0) return race;
	auto row = m_varSheetData.GetRow(textureId);
	return row[race - 1].GetValue32();
}
