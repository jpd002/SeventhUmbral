#include <memory>
#include <cassert>
#include "Sheet.h"
#include "XmlFileDecoder.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include "PtrStream.h"

CSheet::CSheet()
{

}

CSheet::~CSheet()
{

}

CSheet CSheet::Create(Framework::CStream& inputStream)
{
	CSheet sheet;
	sheet.Read(inputStream);
	return std::move(sheet);
}

const CSheet::SheetInfoArray& CSheet::GetSubSheets() const
{
	return m_subSheets;
}

void CSheet::Read(Framework::CStream& inputStream)
{
	auto decodedFile = CXmlFileDecoder::Decode(inputStream);
	auto decodedFileStream = Framework::CPtrStream(decodedFile.data(), decodedFile.size());
	auto sheetDocument = std::unique_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(decodedFileStream));
	auto sheetNodes = sheetDocument->SelectNodes("ssd/sheet");
	m_subSheets.reserve(sheetNodes.size());
	for(const auto& sheetNode : sheetNodes)
	{
		SHEET_INFO info;
		info.name = Framework::Xml::GetAttributeStringValue(sheetNode, "name");
		info.mode = Framework::Xml::GetAttributeStringValue(sheetNode, "mode");
		info.columnMax = Framework::Xml::GetAttributeIntValue(sheetNode, "column_max");
		info.columnCount = Framework::Xml::GetAttributeIntValue(sheetNode, "column_count");
		info.cache = Framework::Xml::GetAttributeIntValue(sheetNode, "cache");
		info.type = Framework::Xml::GetAttributeStringValue(sheetNode, "type");
		Framework::Xml::GetAttributeStringValue(sheetNode, "lang", &info.lang);
		info.typeParams = ReadTypes(sheetNode);
		info.indexParams = ReadIndices(sheetNode);
		info.blocks = ReadBlocks(sheetNode);
		m_subSheets.push_back(info);
	}
}

CSheet::TypeParamArray CSheet::ReadTypes(Framework::Xml::CNode* sheetNode)
{
	TypeParamArray result;
	auto typeParamNodes = sheetNode->SelectNodes("type/param");
	for(const auto& typeParamNode : typeParamNodes)
	{
		auto nodeValue = typeParamNode->GetInnerText();
		TYPE_PARAM typeParam = TYPE_PARAM_UNKNOWN;
		if(!strcmp(nodeValue, "s8"))			typeParam = TYPE_PARAM_S8;
		else if(!strcmp(nodeValue, "u8"))		typeParam = TYPE_PARAM_U8;
		else if(!strcmp(nodeValue, "s16"))		typeParam = TYPE_PARAM_S16;
		else if(!strcmp(nodeValue, "u16"))		typeParam = TYPE_PARAM_U16;
		else if(!strcmp(nodeValue, "s32"))		typeParam = TYPE_PARAM_S32;
		else if(!strcmp(nodeValue, "u32"))		typeParam = TYPE_PARAM_U32;
		else if(!strcmp(nodeValue, "bool"))		typeParam = TYPE_PARAM_BOOL;
		else if(!strcmp(nodeValue, "f16"))		typeParam = TYPE_PARAM_F16;
		else if(!strcmp(nodeValue, "float"))	typeParam = TYPE_PARAM_F32;
		else if(!strcmp(nodeValue, "str"))		typeParam = TYPE_PARAM_STRING;
		else									assert(0);
		result.push_back(typeParam);
	}
	return result;
}

CSheet::IndexParamArray CSheet::ReadIndices(Framework::Xml::CNode* sheetNode)
{
	IndexParamArray result;
	auto indexParamNodes = sheetNode->SelectNodes("index/param");
	for(const auto& indexParamNode : indexParamNodes)
	{
		auto nodeValue = indexParamNode->GetInnerText();
		result.push_back(atoi(nodeValue));
	}
	return result;
}

CSheet::BlockInfoArray CSheet::ReadBlocks(Framework::Xml::CNode* sheetNode)
{
	BlockInfoArray result;
	auto fileNodes = sheetNode->SelectNodes("block/file");
	for(const auto& fileNode : fileNodes)
	{
		BLOCK_INFO blockInfo;
		blockInfo.begin = Framework::Xml::GetAttributeIntValue(fileNode, "begin");
		blockInfo.count = Framework::Xml::GetAttributeIntValue(fileNode, "count");
		blockInfo.offsetFileId = Framework::Xml::GetAttributeIntValue(fileNode, "offset");
		blockInfo.enableFileId = Framework::Xml::GetAttributeIntValue(fileNode, "enable");
		blockInfo.dataFileId = atoi(fileNode->GetInnerText());
		result.push_back(blockInfo);
	}
	return result;
}
