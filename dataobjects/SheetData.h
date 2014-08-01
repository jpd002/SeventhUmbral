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
	public:
		CELL() = default;

		~CELL()
		{
			Reset();
		}

		CELL(const CELL& rhs)
		{
			CopyFrom(rhs);
		}

		CELL& operator =(const CELL& rhs)
		{
			Reset();
			CopyFrom(rhs);
			return (*this);
		}

		uint8 GetValue8() const
		{
			return val8;
		}

		void SetValue8(uint8 value)
		{
			val8 = value;
		}

		uint16 GetValue16() const
		{
			return val16;
		}

		void SetValue16(uint16 value)
		{
			val16 = value;
		}

		uint32 GetValue32() const
		{
			return val32;
		}

		void SetValue32(uint32 value)
		{
			val32 = value;
		}

		float GetValueFloat() const
		{
			return valFloat;
		}

		const char* GetStringValue() const
		{
			return strVal;
		}

		void AcquireStringValue(char* value)
		{
			Reset();
			strVal = value;
		}

	private:
		void Reset()
		{
			if(strVal)
			{
				delete [] strVal;
				strVal = nullptr;
			}
		}

		void CopyFrom(const CELL& rhs)
		{
			val32 = rhs.val32;
			if(rhs.strVal)
			{
				strVal = new char[strlen(rhs.strVal) + 1];
				strcpy(strVal, rhs.strVal);
			}
		}

		union
		{
			uint32		val32 = 0;
			uint16		val16;
			uint8		val8;
			float		valFloat;
		};

		char*			strVal = nullptr;
	};

	typedef std::function<Framework::CStream* (uint32)> FileProvider;
	typedef std::vector<CELL> Row;
	typedef std::map<uint32, Row> RowMap;

							CSheetData() = default;
							CSheetData(CSheetData&&);
							CSheetData(const CSheetData&) = delete;
	virtual					~CSheetData();

	static CSheetData		Create(const CSheet&, unsigned int, const FileProvider&);

	CSheetData&				operator =(CSheetData&&);
	CSheetData&				operator =(const CSheetData&) = delete;

	const RowMap&			GetRows() const;
	const Row&				GetRow(uint32) const;

private:
	void					MoveFrom(CSheetData&&);

	void					Read(const CSheet&, unsigned int, const FileProvider&);
	Row						ReadRow(const CSheet::TypeParamArray&, Framework::CStream&);

	RowMap					m_rows;
};
