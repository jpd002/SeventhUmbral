#pragma once

#include <vector>
#include "Stream.h"

class CD3DShaderConstantTable
{
public:
	enum CONSTANT_TYPE : uint16
	{
		CONSTANT_TYPE_VOID,
		CONSTANT_TYPE_BOOL,
		CONSTANT_TYPE_INT,
		CONSTANT_TYPE_FLOAT,
		CONSTANT_TYPE_STRING,
		CONSTANT_TYPE_TEXTURE1D,
		CONSTANT_TYPE_TEXTURE2D,
		CONSTANT_TYPE_TEXTURE3D,
		CONSTANT_TYPE_TEXTURECUBE,
		CONSTANT_TYPE_SAMPLER,
		CONSTANT_TYPE_SAMPLER1D,
		CONSTANT_TYPE_SAMPLER2D,
		CONSTANT_TYPE_SAMPLER3D,
		CONSTANT_TYPE_SAMPLERCUBE,
		CONSTANT_TYPE_PIXELSHADER,
		CONSTANT_TYPE_VERTEXSHADER,
		CONSTANT_TYPE_PIXELFRAGMENT,
		CONSTANT_TYPE_VERTEXFRAGMENT,
		CONSTANT_TYPE_UNSUPPORTED
	};

	enum CONSTANT_CLASS : uint16
	{
		CONSTANT_CLASS_SCALAR,
		CONSTANT_CLASS_VECTOR,
		CONSTANT_CLASS_MATRIX_ROWS,
		CONSTANT_CLASS_MATRIX_COLS,
		CONSTANT_CLASS_OBJECT,
		CONSTANT_CLASS_STRUCT
	};

	struct D3DCONSTANTTABLE
	{
		uint32 size;
		uint32 creator;
		uint32 version;
		uint32 constants;
		uint32 constantInfos;
		uint32 flags;
		uint32 target;
	};
	static_assert(sizeof(D3DCONSTANTTABLE) == 0x1C, "Size of D3DCONSTANTTABLE struct must be 28 bytes.");

	struct D3DCONSTANTINFO
	{
		uint32	name;
		uint16	registerSet;
		uint16	registerIndex;
		uint16	registerCount;
		uint16	reserved;
		uint32	typeInfo;
		uint32	defaultValue;
	};
	static_assert(sizeof(D3DCONSTANTINFO) == 0x14, "Size of D3DCONSTANTINFO struct must be 20 bytes.");

	struct D3DTYPEINFO
	{
		CONSTANT_CLASS		typeClass;
		CONSTANT_TYPE		type;
		uint16				rows;
		uint16				columns;
		uint16				elements;
		uint16				structMembers;
		uint32				structMemberInfos;
	};
	static_assert(sizeof(D3DTYPEINFO) == 0x10, "Size of D3DTYPEINFO struct must be 16 bytes.");

	struct CONSTANT
	{
		D3DCONSTANTINFO		info;
		D3DTYPEINFO			typeInfo;
		std::string			name;
	};
	typedef std::vector<CONSTANT> ConstantArray;

							CD3DShaderConstantTable();
							CD3DShaderConstantTable(Framework::CStream&);
	virtual					~CD3DShaderConstantTable();

	const ConstantArray&	GetConstants() const;

private:
	void					Read(Framework::CStream&);

	D3DCONSTANTTABLE		m_header;
	ConstantArray			m_constants;
};
