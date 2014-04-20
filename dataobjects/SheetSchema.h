#pragma once

#include <string>
#include <vector>
#include "Stream.h"

class CSheetSchema
{
public:
	struct SHEET_INFO
	{
		std::string		name;
		uint32			fileId;
	};
	typedef std::vector<SHEET_INFO> SheetInfoArray;

	enum
	{
		FILE_ID	= 0x01030000
	};

							CSheetSchema(Framework::CStream&);
	virtual					~CSheetSchema();

	const SheetInfoArray&	GetSheetInfos() const;

private:
	void					Read(Framework::CStream&);

	SheetInfoArray			m_sheetInfos;
};
