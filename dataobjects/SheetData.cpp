#include <memory>
#include <cassert>
#include <stdexcept>
#include "SheetData.h"

CSheetData::~CSheetData()
{

}

CSheetData::CSheetData(CSheetData&& rhs)
{
	MoveFrom(std::move(rhs));
}

CSheetData CSheetData::Create(const CSheet& sheet, unsigned int subSheetIndex, const FileProvider& fileProvider)
{
	CSheetData sheetData;
	sheetData.Read(sheet, subSheetIndex, fileProvider);
	return std::move(sheetData);
}

CSheetData& CSheetData::operator =(CSheetData&& rhs)
{
	MoveFrom(std::move(rhs));
	return (*this);
}

const CSheetData::RowMap& CSheetData::GetRows() const
{
	return m_rows;
}

const CSheetData::Row& CSheetData::GetRow(uint32 rowId) const
{
	auto rowIterator = m_rows.find(rowId);
	if(rowIterator == std::end(m_rows)) throw std::exception();
	return rowIterator->second;
}

void CSheetData::MoveFrom(CSheetData&& rhs)
{
	m_rows = std::move(rhs.m_rows);
}

void CSheetData::Read(const CSheet& sheet, unsigned int subSheetIndex, const FileProvider& fileProvider)
{
	auto subSheet = sheet.GetSubSheets()[subSheetIndex];
	const auto& typeParams = subSheet.typeParams;
	for(const auto& block : subSheet.blocks)
	{
		auto enableFileStream = std::unique_ptr<Framework::CStream>(fileProvider(block.enableFileId));
		auto dataFileStream = std::unique_ptr<Framework::CStream>(fileProvider(block.dataFileId));
		while(1)
		{
			int startId = enableFileStream->Read32();
			if(enableFileStream->IsEOF()) break;
			int numIds = enableFileStream->Read32();
			for(unsigned int i = 0; i < numIds; i++)
			{
				auto row = ReadRow(typeParams, *dataFileStream.get());
				m_rows.insert(std::make_pair(startId + i, std::move(row)));
			}
		}
	}
}

CSheetData::Row CSheetData::ReadRow(const CSheet::TypeParamArray& typeParams, Framework::CStream& inputStream)
{
	Row result;
	result.reserve(typeParams.size());
	for(const auto& typeParam : typeParams)
	{
		CELL cell;
		switch(typeParam)
		{
		case CSheet::TYPE_PARAM_S8:
		case CSheet::TYPE_PARAM_U8:
		case CSheet::TYPE_PARAM_BOOL:
			cell.SetValue8(inputStream.Read8());
			break;
		case CSheet::TYPE_PARAM_U16:
		case CSheet::TYPE_PARAM_S16:
		case CSheet::TYPE_PARAM_F16:
			cell.SetValue16(inputStream.Read16());
			break;
		case CSheet::TYPE_PARAM_S32:
		case CSheet::TYPE_PARAM_U32:
		case CSheet::TYPE_PARAM_F32:
			cell.SetValue32(inputStream.Read32());
			break;
		case CSheet::TYPE_PARAM_STRING:
			{
				uint16 stringLength = inputStream.Read16();
				uint8 unknown = inputStream.Read8();
				char* stringData = new char[stringLength - 1];
				inputStream.Read(stringData, stringLength - 1);
				for(unsigned int i = 0; i < stringLength - 1; i++)
				{
					stringData[i] ^= 0x73;
				}
				assert(stringData[stringLength - 2] == 0);
				cell.AcquireStringValue(stringData);
			}
			break;
		default:
			assert(0);
			break;
		}
		result.push_back(std::move(cell));
	}
	return std::move(result);
}
