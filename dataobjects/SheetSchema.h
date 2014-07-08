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
		uint32			fileId = 0;
	};
	typedef std::vector<SHEET_INFO> SheetInfoArray;

	enum
	{
		GAME_SCHEMA_FILE_ID	= 0x01030000,
		VAR_SCHEMA_FILE_ID	= 0x03A70000
	};

							CSheetSchema() = default;
							CSheetSchema(Framework::CStream&);
	virtual					~CSheetSchema();

	const SheetInfoArray&	GetSheets() const;

private:
	void					Read(Framework::CStream&);

	SheetInfoArray			m_sheets;
};
