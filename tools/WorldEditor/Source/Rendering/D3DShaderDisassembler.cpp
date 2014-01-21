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
	case CD3DShader::OPCODE_RCP:
		return "rcp";
	case CD3DShader::OPCODE_RSQ:
		return "rsq";
	case CD3DShader::OPCODE_DP3:
		return "dp3";
	case CD3DShader::OPCODE_DP4:
		return "dp4";
	case CD3DShader::OPCODE_MIN:
		return "min";
	case CD3DShader::OPCODE_MAX:
		return "max";
	case CD3DShader::OPCODE_SLT:
		return "slt";
	case CD3DShader::OPCODE_EXP:
		return "exp";
	case CD3DShader::OPCODE_LOG:
		return "log";
	case CD3DShader::OPCODE_LRP:
		return "lrp";
	case CD3DShader::OPCODE_FRC:
		return "frc";
	case CD3DShader::OPCODE_LOOP:
		return "loop";
	case CD3DShader::OPCODE_ENDLOOP:
		return "endloop";
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
	case CD3DShader::OPCODE_POW:
		return "pow";
	case CD3DShader::OPCODE_ABS:
		return "abs";
	case CD3DShader::OPCODE_NRM:
		return "nrm";
	case CD3DShader::OPCODE_REP:
		return "rep";
	case CD3DShader::OPCODE_ENDREP:
		return "endrep";
	case CD3DShader::OPCODE_IF:
		return "if";
	case CD3DShader::OPCODE_ENDIF:
		return "endif";
	case CD3DShader::OPCODE_MOVA:
		return "mova";
	case CD3DShader::OPCODE_DEFI:
		return "defi";
	case CD3DShader::OPCODE_TEXKILL:
		return "texkill";
	case CD3DShader::OPCODE_TEXLD:
		return "texld";
	case CD3DShader::OPCODE_DEF:
		return "def";
	case CD3DShader::OPCODE_CMP:
		return "cmp";
	case CD3DShader::OPCODE_TEXLDL:
		return "texldl";
	default:
		assert(0);
		return string_format("unk_0x%0.2X", instruction.token.opcode);
	}
}

std::string CD3DShaderDisassembler::GetInstructionOperands(CD3DShader::SHADER_TYPE, const CD3DShader::INSTRUCTION& instruction)
{
	CD3DShader::CTokenStream tokenStream(instruction.additionalTokens);
	switch(instruction.token.opcode)
	{
	case CD3DShader::OPCODE_MOV:
	case CD3DShader::OPCODE_MOVA:
	case CD3DShader::OPCODE_RCP:
	case CD3DShader::OPCODE_RSQ:
	case CD3DShader::OPCODE_EXP:
	case CD3DShader::OPCODE_LOG:
	case CD3DShader::OPCODE_FRC:
	case CD3DShader::OPCODE_ABS:
	case CD3DShader::OPCODE_NRM:
		{
			auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
			auto srcParam = CD3DShader::ReadSourceParameter(tokenStream);

			auto dstParamString = PrintDestinationParameterToken(dstParam);
			auto srcParamString = PrintSourceParameterToken(srcParam, dstParam.parameter.writeMask);
			return string_format("%s, %s", dstParamString.c_str(), srcParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_ADD:
	case CD3DShader::OPCODE_MUL:
	case CD3DShader::OPCODE_DP3:
	case CD3DShader::OPCODE_DP4:
	case CD3DShader::OPCODE_MIN:
	case CD3DShader::OPCODE_MAX:
	case CD3DShader::OPCODE_SLT:
	case CD3DShader::OPCODE_POW:
	case CD3DShader::OPCODE_TEXLD:
	case CD3DShader::OPCODE_TEXLDL:
		{
			auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
			auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
			auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);

			auto dstParamString = PrintDestinationParameterToken(dstParam);
			auto src1ParamString = PrintSourceParameterToken(src1Param, dstParam.parameter.writeMask);
			auto src2ParamString = PrintSourceParameterToken(src2Param, dstParam.parameter.writeMask);
			return string_format("%s, %s, %s", dstParamString.c_str(), src1ParamString.c_str(), src2ParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_TEXKILL:
		{
			auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
			auto dstParamString = PrintDestinationParameterToken(dstParam);
			return string_format("%s", dstParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_MAD:
	case CD3DShader::OPCODE_LRP:
	case CD3DShader::OPCODE_CMP:
		{
			auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
			auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
			auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
			auto src3Param = CD3DShader::ReadSourceParameter(tokenStream);

			auto dstParamString = PrintDestinationParameterToken(dstParam);
			auto src1ParamString = PrintSourceParameterToken(src1Param, dstParam.parameter.writeMask);
			auto src2ParamString = PrintSourceParameterToken(src2Param, dstParam.parameter.writeMask);
			auto src3ParamString = PrintSourceParameterToken(src3Param, dstParam.parameter.writeMask);
			return string_format("%s, %s, %s, %s", dstParamString.c_str(), 
				src1ParamString.c_str(), src2ParamString.c_str(), src3ParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_LOOP:
		{
			auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
			auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
			auto src1ParamString = PrintSourceParameterToken(src1Param, 0xF);
			auto src2ParamString = PrintSourceParameterToken(src2Param, 0xF);
			return string_format("%s, %s", src1ParamString.c_str(), src2ParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_DCL:
		{
			auto customParamToken = tokenStream.ReadToken();
			auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
			auto dstParamString = PrintDestinationParameterToken(dstParam);
			return string_format("%s", dstParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_IF:
	case CD3DShader::OPCODE_REP:
		{
			auto srcParam = CD3DShader::ReadSourceParameter(tokenStream);
			auto srcParamString = PrintSourceParameterToken(srcParam, 0x0F);
			return string_format("%s", srcParamString.c_str());
		}
		break;
	case CD3DShader::OPCODE_DEFI:
		{
			auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
			auto dstParamString = PrintDestinationParameterToken(dstParam);
			return string_format("%s, %d, %d, %d, %d", dstParamString.c_str(), 
				instruction.additionalTokens[1], instruction.additionalTokens[2],
				instruction.additionalTokens[3], instruction.additionalTokens[4]);
			return string_format("");
		}
		break;
	case CD3DShader::OPCODE_DEF:
		{
			auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
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
	case CD3DShader::OPCODE_ENDLOOP:
	case CD3DShader::OPCODE_ENDREP:
	case CD3DShader::OPCODE_ENDIF:
		return std::string();
		break;
	default:
		assert(0);
		return std::string();
		break;
	}
}

std::string CD3DShaderDisassembler::PrintSourceParameterToken(const CD3DShader::SOURCE_PARAMETER& srcParam, uint32 writeMask)
{
	static const char* g_element[4] =
	{
		"x",
		"y",
		"z",
		"w"
	};
	auto regString = PrintParameterRegister(srcParam.parameter.GetRegisterType(), srcParam.parameter.registerNumber);
	if(srcParam.parameter.useRelativeAddressing)
	{
		auto addrString = PrintParameterRegister(srcParam.relativeAddress.GetRegisterType(), srcParam.relativeAddress.registerNumber);
		addrString += ".";
		addrString += g_element[srcParam.relativeAddress.swizzleX];
		regString = string_format("%s[%s]", regString.c_str(), addrString.c_str());
	}
	switch(srcParam.parameter.sourceModifier)
	{
	case CD3DShader::SOURCE_MODIFIER_NONE:
		break;
	case CD3DShader::SOURCE_MODIFIER_NEGATE:
		regString = string_format("-%s", regString.c_str());
		break;
	case CD3DShader::SOURCE_MODIFIER_ABSOLUTE_NEGATE:
		regString = string_format("-abs(%s)", regString.c_str());
		break;
	default:
		assert(0);
		break;
	}
	regString += ".";
	if(writeMask & 0x1) regString += g_element[srcParam.parameter.swizzleX];
	if(writeMask & 0x2) regString += g_element[srcParam.parameter.swizzleY];
	if(writeMask & 0x4) regString += g_element[srcParam.parameter.swizzleZ];
	if(writeMask & 0x8) regString += g_element[srcParam.parameter.swizzleW];
	return regString;
}

std::string CD3DShaderDisassembler::PrintDestinationParameterToken(const CD3DShader::DESTINATION_PARAMETER& dstParam)
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
	assert(dstParam.parameter.useRelativeAddressing == 0);
	auto regString = PrintParameterRegister(dstParam.parameter.GetRegisterType(), dstParam.parameter.registerNumber);
	regString += ".";
	regString += g_writeMask[dstParam.parameter.writeMask];
	if(dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_SATURATE)
	{
		regString += " [sat]";
	}
	if(dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_PARTIALPRECISION)
	{
		regString += " [pp]";
	}
	if(dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_CENTROID)
	{
		assert(0);
	}
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
	case CD3DShader::SHADER_REGISTER_TEXTURE:
		return string_format("a%d", registerNumber);
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
	case CD3DShader::SHADER_REGISTER_LOOP:
		return "aL";
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
