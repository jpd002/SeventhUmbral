#pragma once

#include "Singleton.h"
#include "../../dataobjects/SheetData.h"

class CTexPathVars : public CSingleton<CTexPathVars>
{
public:
					CTexPathVars();
	virtual			~CTexPathVars();

	uint32			GetTexturePathId(uint32, uint32) const;

private:
	CSheetData		m_varSheetData;
};
