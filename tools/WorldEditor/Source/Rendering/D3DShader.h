#pragma once

#include "Stream.h"
#include "D3DShaderConstantTable.h"
#include <vector>

class CD3DShader
{
public:
	enum SHADER_TYPE
	{
		SHADER_TYPE_INVALID	= 0,
		SHADER_TYPE_VERTEX	= 0xFFFE,
		SHADER_TYPE_PIXEL	= 0xFFFF,
	};

	enum SHADER_REGISTER_TYPE
	{
		SHADER_REGISTER_TEMP,
		SHADER_REGISTER_INPUT,
		SHADER_REGISTER_CONST,
		SHADER_REGISTER_TEXTURE,	//also ADDR type for VS?
		SHADER_REGISTER_RASTOUT,
		SHADER_REGISTER_ATTROUT,
		SHADER_REGISTER_OUTPUT,
		SHADER_REGISTER_CONSTINT,
		SHADER_REGISTER_COLOROUT,
		SHADER_REGISTER_DEPTHOUT,
		SHADER_REGISTER_SAMPLER,
		SHADER_REGISTER_CONST2,
		SHADER_REGISTER_CONST3,
		SHADER_REGISTER_CONST4,
		SHADER_REGISTER_CONSTBOOL,
		SHADER_REGISTER_LOOP,
		SHADER_REGISTER_TEMPFLOAT16,
		SHADER_REGISTER_MISCTYPE,
		SHADER_REGISTER_LABEL,
		SHADER_REGISTER_PREDICATE
	};

	enum SHADER_DECLUSAGE_TYPE
	{
		SHADER_DECLUSAGE_POSITION,
		SHADER_DECLUSAGE_BLENDWEIGHT,
		SHADER_DECLUSAGE_BLENDINDICES,
		SHADER_DECLUSAGE_NORMAL,
		SHADER_DECLUSAGE_PSIZE,
		SHADER_DECLUSAGE_TEXCOORD,
		SHADER_DECLUSAGE_TANGENT,
		SHADER_DECLUSAGE_BINORMAL,
		SHADER_DECLUSAGE_TESSFACTOR,
		SHADER_DECLUSAGE_POSITIONT,
		SHADER_DECLUSAGE_COLOR,
		SHADER_DECLUSAGE_FOG,
		SHADER_DECLUSAGE_DEPTH,
		SHADER_DECLUSAGE_SAMPLE,
	};

	enum OPCODE
	{
		OPCODE_NOP		= 0x00,
		OPCODE_MOV		= 0x01,
		OPCODE_ADD		= 0x02,
		OPCODE_MAD		= 0x04,
		OPCODE_MUL		= 0x05,
		OPCODE_RCP		= 0x06,
		OPCODE_RSQ		= 0x07,
		OPCODE_DP3		= 0x08,
		OPCODE_DP4		= 0x09,
		OPCODE_MAX		= 0x0B,
		OPCODE_LRP		= 0x12,
		OPCODE_LOOP		= 0x1B,
		OPCODE_DCL		= 0x1F,
		OPCODE_NRM		= 0x24,
		OPCODE_IF		= 0x28,
		OPCODE_ENDIF	= 0x2B,
		OPCODE_DEFI		= 0x30,
		OPCODE_TEXLD	= 0x42,
		OPCODE_DEF		= 0x51,
		OPCODE_COMMENT	= 0xFFFE,
		OPCODE_END		= 0xFFFF
	};

	struct INSTRUCTION_TOKEN
	{
		OPCODE			opcode		: 16;
		unsigned int	control		: 8;
		unsigned int	size		: 4;
		unsigned int	predicated	: 1;
		unsigned int	reserved0	: 1;
		unsigned int	coIssue		: 1;
		unsigned int	reserved1	: 1;
	};
	static_assert(sizeof(INSTRUCTION_TOKEN) == 4, "Size of INSTRUCTION_TOKEN must be 4 bytes.");

	struct SOURCE_PARAMETER_TOKEN
	{
		unsigned int			registerNumber			: 11;
		unsigned int			registerTypeHi			: 2;
		unsigned int			useRelativeAddressing	: 1;
		unsigned int			reserved0				: 2;
		unsigned int			swizzleX				: 2;
		unsigned int			swizzleY				: 2;
		unsigned int			swizzleZ				: 2;
		unsigned int			swizzleW				: 2;
		unsigned int			sourceModifier			: 4;
		unsigned int			registerTypeLo			: 3;
		unsigned int			reserved2				: 1;

		SHADER_REGISTER_TYPE	GetRegisterType() const { return static_cast<SHADER_REGISTER_TYPE>(registerTypeLo | (registerTypeHi << 3)); }
	};
	static_assert(sizeof(SOURCE_PARAMETER_TOKEN) == 4, "Size of SOURCE_PARAMETER_TOKEN must be 4 bytes.");

	struct DESTINATION_PARAMETER_TOKEN
	{
		unsigned int			registerNumber			: 11;
		unsigned int			registerTypeHi			: 2;
		unsigned int			useRelativeAddressing	: 1;
		unsigned int			reserved0				: 2;
		unsigned int			writeMask				: 4;
		unsigned int			resultModifier			: 4;
		unsigned int			reserved1				: 4;
		unsigned int			registerTypeLo			: 3;
		unsigned int			reserved2				: 1;

		SHADER_REGISTER_TYPE	GetRegisterType() const { return static_cast<SHADER_REGISTER_TYPE>(registerTypeLo | (registerTypeHi << 3)); }
	};
	static_assert(sizeof(DESTINATION_PARAMETER_TOKEN) == 4, "Size of DESTINATION_PARAMETER_TOKEN must be 4 bytes.");

	struct INSTRUCTION
	{
		INSTRUCTION()
		{
			memset(&token, 0, sizeof(token));
		}

		INSTRUCTION_TOKEN		token;
		std::vector<uint32>		additionalTokens;
	};
	typedef std::vector<INSTRUCTION> InstructionArray;

	typedef std::vector<uint8> Comment;
	typedef std::vector<Comment> CommentArray;

								CD3DShader(Framework::CStream&);
	virtual						~CD3DShader();

	SHADER_TYPE					GetType() const;
	const InstructionArray&		GetInstructions() const;
	const CommentArray&			GetComments() const;
	CD3DShaderConstantTable		GetConstantTable() const;

private:
	void						Read(Framework::CStream&);

	SHADER_TYPE					m_type = SHADER_TYPE_INVALID;
	InstructionArray			m_instructions;
	CommentArray				m_comments;
};
