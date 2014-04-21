#pragma once

#include <string>
#include <vector>
#include "Stream.h"
#include "xml/Node.h"

class CSheet
{
public:
	enum TYPE_PARAM
	{
		TYPE_PARAM_UNKNOWN,
		TYPE_PARAM_S8,
		TYPE_PARAM_U8,
		TYPE_PARAM_S16,
		TYPE_PARAM_U16,
		TYPE_PARAM_S32,
		TYPE_PARAM_U32,
		TYPE_PARAM_BOOL,
		TYPE_PARAM_FLOAT,
		TYPE_PARAM_STRING,
	};

	typedef std::vector<TYPE_PARAM> TypeParamArray;
	typedef std::vector<int> IndexParamArray;

	struct BLOCK_INFO
	{
		unsigned int	begin;
		unsigned int	count;
		unsigned int	offsetFileId;
		unsigned int	enableFileId;
		unsigned int	dataFileId;
	};
	typedef std::vector<BLOCK_INFO> BlockInfoArray;

	struct SHEET_INFO
	{
		std::string		name;
		std::string		mode;
		uint32			columnMax = 0;
		uint32			columnCount = 0;
		int32			cache = 0;
		std::string		type;
		std::string		lang;
		TypeParamArray	typeParams;
		IndexParamArray	indexParams;
		BlockInfoArray	blocks;
	};
	typedef std::vector<SHEET_INFO> SheetInfoArray;

							CSheet();
	virtual					~CSheet();

	static CSheet			Create(Framework::CStream&);

	const SheetInfoArray&	GetSubSheets() const;

private:
	void					Read(Framework::CStream&);
	TypeParamArray			ReadTypes(Framework::Xml::CNode*);
	IndexParamArray			ReadIndices(Framework::Xml::CNode*);
	BlockInfoArray			ReadBlocks(Framework::Xml::CNode*);

	SheetInfoArray			m_subSheets;
};
