//#include <d3d9types.h>
//#include <d3dx9shader.h>
#include "D3DShaderDisassembler.h"
#include <assert.h>
#include "string_format.h"

static const char* g_declUsageString[] =
{
	"position",
	"blendweight",
	"blendindices",
	"normal",
	"psize",
	"texcoord",
	"tangent",
	"binormal",
	"tessfactor",
	"positiont",
	"color",
	"fog",
	"depth",
	"sample"
};

std::string CD3DShaderDisassembler::GetInstructionMnemonic(CD3DShader::SHADER_TYPE shaderType, const CD3DShader::INSTRUCTION& instruction)
{
	switch(instruction.token.opcode)
	{
	case CD3DShader::OPCODE_MOV:
		return "mov";
	case CD3DShader::OPCODE_ADD:
		return "add";
	case CD3DShader::OPCODE_MAD:
		return "mad";
	case CD3DShader::OPCODE_MUL:
		return "mul";
	case CD3DShader::OPCODE_RSQ:
		return "rsq";
	case CD3DShader::OPCODE_DP3:
		return "dp3";
	case CD3DShader::OPCODE_DP4:
		return "dp4";
	case CD3DShader::OPCODE_MAX:
		return "max";
	case CD3DShader::OPCODE_LRP:
		return "lrp";
	case CD3DShader::OPCODE_LOOP:
		return "loop";
	case CD3DShader::OPCODE_DCL:
		assert(instruction.token.size == 2);
		if(shaderType == CD3DShader::SHADER_TYPE_VERTEX)
		{
			auto usage = static_cast<CD3DShader::SHADER_DECLUSAGE_TYPE>(instruction.additionalTokens[0] & 0x1F);
			auto usageIndex = (instruction.additionalTokens[0] >> 16) & 0x0F;
			return PrintDeclUsage(usage, usageIndex);
		}
		else if(shaderType == CD3DShader::SHADER_TYPE_PIXEL)
		{
			auto usage = static_cast<CD3DShader::SHADER_DECLUSAGE_TYPE>(instruction.additionalTokens[0] & 0x1F);
			auto usageIndex = (instruction.additionalTokens[0] >> 16) & 0x0F;
			return PrintDeclUsage(usage, usageIndex);
		}
		else
		{
			assert(0);
			return string_format("unk_0x%0.2X", instruction.token.opcode);
		}
		break;
	case CD3DShader::OPCODE_NRM:
		return "nrm";
	case CD3DShader::OPCODE_IF:
		return "if";
	case CD3DShader::OPCODE_ENDIF:
		return "endif";
	case CD3DShader::OPCODE_DEFI:
		return "defi";
	case CD3DShader::OPCODE_TEXLD:
		return "texld";
	case CD3DShader::OPCODE_DEF:
		return "def";
	default:
		assert(0);
		return string_format("unk_0x%0.2X", instruction.token.opcode);
	}
}

std::string CD3DShaderDisassembler::GetInstructionOperands(CD3DShader::SHADER_TYPE, const CD3DShader::INSTRUCTION& instruction)
{
	switch(instruction.token.opcode)
	{
	case CD3DShader::OPCODE_MOV:
	case CD3DShader::OPCODE_RSQ:
	case CD3DShader::OPCODE_NRM:
		{
			assert(instruction.token.size == 2);
			auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
			auto srcParam = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);

			auto dstParamString = PrintDestinationParameterToken(dstParam);
			auto srcParamString = PrintSourceParameterToken(srcParam, dstParam.writeMask);
			return string_format("%s, %s", dstParamString.c_str(), srcParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_MAD:
	case CD3DShader::OPCODE_LRP:
		{
			assert(instruction.token.size == 4);
			auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
			auto src1Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
			auto src2Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[2]);
			auto src3Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[3]);

			auto dstParamString = PrintDestinationParameterToken(dstParam);
			auto src1ParamString = PrintSourceParameterToken(src1Param, dstParam.writeMask);
			auto src2ParamString = PrintSourceParameterToken(src2Param, dstParam.writeMask);
			auto src3ParamString = PrintSourceParameterToken(src3Param, dstParam.writeMask);
			return string_format("%s, %s, %s, %s", dstParamString.c_str(), 
				src1ParamString.c_str(), src2ParamString.c_str(), src3ParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_ADD:
	case CD3DShader::OPCODE_MUL:
	case CD3DShader::OPCODE_DP3:
	case CD3DShader::OPCODE_DP4:
	case CD3DShader::OPCODE_MAX:
	case CD3DShader::OPCODE_TEXLD:
		{
			assert(instruction.token.size == 3);
			auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
			auto src1Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
			auto src2Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[2]);

			auto dstParamString = PrintDestinationParameterToken(dstParam);
			auto src1ParamString = PrintSourceParameterToken(src1Param, dstParam.writeMask);
			auto src2ParamString = PrintSourceParameterToken(src2Param, dstParam.writeMask);
			return string_format("%s, %s, %s", dstParamString.c_str(), src1ParamString.c_str(), src2ParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_DCL:
		{
			assert(instruction.token.size == 2);
			auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
			auto dstParamString = PrintDestinationParameterToken(dstParam);
			return string_format("%s", dstParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_IF:
		{
			assert(instruction.token.size == 1);
			auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
			auto dstParamString = PrintDestinationParameterToken(dstParam);
			return string_format("%s", dstParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_DEFI:
		{
			assert(instruction.token.size == 5);
			auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
			auto dstParamString = PrintDestinationParameterToken(dstParam);
			return string_format("%s, %d, %d, %d, %d", dstParamString.c_str(), 
				instruction.additionalTokens[1], instruction.additionalTokens[2],
				instruction.additionalTokens[3], instruction.additionalTokens[4]);
			return string_format("");
		}
		break;
	case CD3DShader::OPCODE_DEF:
		{
			assert(instruction.token.size == 5);
			auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
			auto dstParamString = PrintDestinationParameterToken(dstParam);
			float cst[4] =
			{
				*reinterpret_cast<const float*>(instruction.additionalTokens.data() + 1),
				*reinterpret_cast<const float*>(instruction.additionalTokens.data() + 2),
				*reinterpret_cast<const float*>(instruction.additionalTokens.data() + 3),
				*reinterpret_cast<const float*>(instruction.additionalTokens.data() + 4),
			};
			return string_format("%s, %f, %f, %f, %f", dstParamString.c_str(), cst[0], cst[1], cst[2], cst[3]);
		}
		break;
	case CD3DShader::OPCODE_ENDIF:
		return std::string();
		break;
	default:
		assert(0);
		return std::string();
		break;
	}
}

std::string CD3DShaderDisassembler::PrintSourceParameterToken(const CD3DShader::SOURCE_PARAMETER_TOKEN& srcToken, uint32 writeMask)
{
	static const char* g_element[4] =
	{
		"x",
		"y",
		"z",
		"w"
	};
	assert(srcToken.useRelativeAddressing == 0);
	auto regString = PrintParameterRegister(srcToken.GetRegisterType(), srcToken.registerNumber);
	regString += ".";
	if(writeMask & 0x1) regString += g_element[srcToken.swizzleX];
	if(writeMask & 0x2) regString += g_element[srcToken.swizzleY];
	if(writeMask & 0x4) regString += g_element[srcToken.swizzleZ];
	if(writeMask & 0x8) regString += g_element[srcToken.swizzleW];
	return regString;
}

std::string CD3DShaderDisassembler::PrintDestinationParameterToken(const CD3DShader::DESTINATION_PARAMETER_TOKEN& dstToken)
{
	static const char* g_writeMask[0x10] =
	{
		"(Invalid)",
		"x",		//0x01
		"y",		//0x02
		"xy",		//0x03
		"z",		//0x04
		"xz",		//0x05
		"yz",		//0x06
		"xyz",		//0x07
		"w",
		"xw",
		"yw",
		"xyw",
		"zw",
		"xzw",
		"yzw",
		"xyzw"
	};
	assert(dstToken.useRelativeAddressing == 0);
	auto regString = PrintParameterRegister(dstToken.GetRegisterType(), dstToken.registerNumber);
	regString += ".";
	regString += g_writeMask[dstToken.writeMask];
	return regString;
}

std::string CD3DShaderDisassembler::PrintParameterRegister(CD3DShader::SHADER_REGISTER_TYPE registerType, uint32 registerNumber)
{
	switch(registerType)
	{
	case CD3DShader::SHADER_REGISTER_TEMP:
		return string_format("r%d", registerNumber);
	case CD3DShader::SHADER_REGISTER_INPUT:
		return string_format("v%d", registerNumber);
	case CD3DShader::SHADER_REGISTER_CONST:
		return string_format("c%d", registerNumber);
	case CD3DShader::SHADER_REGISTER_OUTPUT:
		return string_format("o%d", registerNumber);
	case CD3DShader::SHADER_REGISTER_CONSTINT:
		return string_format("i%d", registerNumber);
	case CD3DShader::SHADER_REGISTER_COLOROUT:
		return string_format("oC%d", registerNumber);
	case CD3DShader::SHADER_REGISTER_SAMPLER:
		return string_format("s%d", registerNumber);
	case CD3DShader::SHADER_REGISTER_CONSTBOOL:
		return string_format("b%d", registerNumber);
	default:
		assert(0);
		return string_format("?%d", registerNumber);
	}
}

std::string CD3DShaderDisassembler::PrintDeclUsage(CD3DShader::SHADER_DECLUSAGE_TYPE usage, uint32 usageIndex)
{
	switch(usage)
	{
	case CD3DShader::SHADER_DECLUSAGE_POSITION:
		return "dcl_position";
	case CD3DShader::SHADER_DECLUSAGE_BLENDINDICES:
		return "dcl_blendindices";
	case CD3DShader::SHADER_DECLUSAGE_NORMAL:
		return "dcl_normal";
	case CD3DShader::SHADER_DECLUSAGE_TEXCOORD:
		return string_format("dcl_texcoord%d", usageIndex);
	case CD3DShader::SHADER_DECLUSAGE_COLOR:
		return "dcl_color";
	default:
		assert(0);
		return string_format("dcl?_%d", usage);
	}
}
