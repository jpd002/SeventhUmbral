#include "SheetColumnsDefinition.h"
#include "xml/Parser.h"
#include "xml/Utils.h"

CSheetColumnsDefinition::CSheetColumnsDefinition()
{

}

CSheetColumnsDefinition::~CSheetColumnsDefinition()
{

}

std::string CSheetColumnsDefinition::GetColumnName(const std::string& sheetName, uint32 columnIndex) const
{
	auto sheetIterator = std::find_if(std::begin(m_sheets), std::end(m_sheets), 
		[sheetName] (const SHEET& sheet) { return sheet.name == sheetName; });
	if(sheetIterator == std::end(m_sheets)) return std::string();
	
	const auto& sheet = *sheetIterator;
	auto columnIterator = std::find_if(std::begin(sheet.columns), std::end(sheet.columns),
		[columnIndex] (const COLUMN& column) { return column.index == columnIndex; });
	if(columnIterator == std::end(sheet.columns)) return std::string();

	return columnIterator->name;
}

CSheetColumnsDefinition CSheetColumnsDefinition::CreateFromXml(Framework::CStream& stream)
{
	CSheetColumnsDefinition result;
	auto documentNode = std::shared_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(stream));
	auto sheetNodes = documentNode->SelectNodes("Sheets/Sheet");
	for(const auto& sheetNode : sheetNodes)
	{
		SHEET sheet;
		sheet.name = Framework::Xml::GetAttributeStringValue(sheetNode, "Name");

		auto columnNodes = sheetNode->SelectNodes("Column");
		for(const auto& columnNode : columnNodes)
		{
			COLUMN column;
			column.index = Framework::Xml::GetAttributeIntValue(columnNode, "Index");
			column.name = Framework::Xml::GetAttributeStringValue(columnNode, "Name");
			sheet.columns.push_back(column);
		}

		result.m_sheets.push_back(sheet);
	}
	return result;
}
