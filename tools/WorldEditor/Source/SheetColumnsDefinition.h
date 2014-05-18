#pragma once

#include <vector>

class CSheetColumnsDefinition
{
public:
	struct COLUMN
	{
		uint32			index = 0;
		std::string		name;
	};
	typedef std::vector<COLUMN> ColumnArray;

	struct SHEET
	{
		std::string		name;
		ColumnArray		columns;
	};

									CSheetColumnsDefinition();
	virtual							~CSheetColumnsDefinition();

	
	static CSheetColumnsDefinition	CreateFromXml(Framework::CStream&);

	std::string						GetColumnName(const std::string&, uint32) const;

private:
	typedef std::vector<SHEET> SheetArray;

	SheetArray						m_sheets;
};
