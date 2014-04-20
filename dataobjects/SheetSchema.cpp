#include <memory>
#include "SheetSchema.h"
#include "XmlFileDecoder.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include "PtrStream.h"

CSheetSchema::CSheetSchema(Framework::CStream& inputStream)
{
	Read(inputStream);
}

CSheetSchema::~CSheetSchema()
{

}

const CSheetSchema::SheetInfoArray& CSheetSchema::GetSheetInfos() const
{
	return m_sheetInfos;
}

void CSheetSchema::Read(Framework::CStream& inputStream)
{
	auto decodedFile = CXmlFileDecoder::Decode(inputStream);
	auto decodedFileStream = Framework::CPtrStream(decodedFile.data(), decodedFile.size());
	auto schemaDocument = std::unique_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(decodedFileStream));
	auto sheetNodes = schemaDocument->SelectNodes("ssd/sheet");
	m_sheetInfos.reserve(sheetNodes.size());
	for(const auto& sheetNode : sheetNodes)
	{
		SHEET_INFO info;
		info.name = Framework::Xml::GetAttributeStringValue(sheetNode, "name");
		info.fileId = Framework::Xml::GetAttributeIntValue(sheetNode, "infofile");
		m_sheetInfos.push_back(info);
	}
}
