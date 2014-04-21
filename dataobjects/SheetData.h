#pragma once

#include <functional>
#include <map>

#include "Stream.h"
#include "Sheet.h"

class CSheetData
{
public:
	struct CELL
	{
		std::string strVal;

		union
		{
			uint32 val32 = 0;
			uint16 val16;
			uint8 val8;
		};
	};

	typedef std::function<Framework::CStream* (uint32)> FileProvider;
	typedef std::vector<CELL> Row;
	typedef std::map<uint32, Row> RowMap;

							CSheetData();
	virtual					~CSheetData();

	static CSheetData		Create(const CSheet&, const FileProvider&);

	const Row&				GetRow(uint32) const;

private:
	void					Read(const CSheet&, const FileProvider&);
	Row						ReadRow(const CSheet::TypeParamArray&, Framework::CStream&);

	RowMap					m_rows;
};
