#include <assert.h>
#include <set>
#include "string_format.h"
#include "Dx11UmbralEffectGenerator.h"
#include "D3DShaderConstantTable.h"

std::string CDx11UmbralEffectGenerator::GenerateVertexShader(const CD3DShader& inputShader)
{
	CDx11UmbralEffectGenerator generator;
	return generator.GenerateVertexShaderInternal(inputShader);
}

std::string CDx11UmbralEffectGenerator::GeneratePixelShader(const CD3DShader& inputVertexShader, const CD3DShader& inputPixelShader)
{
	CDx11UmbralEffectGenerator generator;
	return generator.GeneratePixelShaderInternal(inputVertexShader, inputPixelShader);
}

std::string CDx11UmbralEffectGenerator::GenerateVertexShaderInternal(const CD3DShader& inputVertexShader)
{
	assert(inputVertexShader.GetType() == CD3DShader::SHADER_TYPE_VERTEX);

	std::string result;

	//Generate constants
	{
		auto constantTable = inputVertexShader.GetConstantTable();
		auto constantInfo = GenerateConstants(constantTable);
		result += constantInfo.first;
		m_constantRegisterMap = constantInfo.second;
	}

	//Generate input/output structures using dcl instructions
	{
		auto structure = GenerateVertexInputStructure(inputVertexShader);
		m_inputRegisterMap = std::move(structure.second);
		result += structure.first;
	}

	{
		auto structure = GenerateOutputStructure(inputVertexShader);
		m_outputRegisterMap = std::move(structure.second);
		result += structure.first;
	}

	result += "SHADER_OUT VertexProgram(SHADER_IN input)\r\n";
	result += "{\r\n";
	result += "\tSHADER_OUT output;\r\n";

	{
		auto temporaryInfo = GenerateTemporaries(inputVertexShader);
		m_temporaryRegisterMap = std::move(temporaryInfo.second);
		result += temporaryInfo.first;
	}

//	result += "float4 pos = mul(c_worldMatrix, float4(input.var0.xyz, 1));\r\n";
	result += GenerateInstructions(inputVertexShader);
//	result += "output.var0 = mul(c_viewProjMatrix, temp0);\r\n";

	result += "\treturn output;\r\n";
	result += "}\r\n";

	return result;
}

std::string CDx11UmbralEffectGenerator::GeneratePixelShaderInternal(const CD3DShader& inputVertexShader, const CD3DShader& inputPixelShader)
{
	assert(inputVertexShader.GetType() == CD3DShader::SHADER_TYPE_VERTEX);
	assert(inputPixelShader.GetType() == CD3DShader::SHADER_TYPE_PIXEL);

	std::string result;

	//Generate constants
	{
		auto constantTable = inputPixelShader.GetConstantTable();
		auto constantInfo = GenerateConstants(constantTable);
		result += constantInfo.first;
		m_constantRegisterMap.insert(std::begin(constantInfo.second), std::end(constantInfo.second));
	}

	{
		auto structure = GeneratePixelInputStructure(inputVertexShader, inputPixelShader);
		m_inputRegisterMap = std::move(structure.second);
		result += structure.first;
	}

	result += "float4 PixelProgram(SHADER_IN input) : SV_TARGET\r\n";
	result += "{\r\n";
	result += "\tfloat4 oC0;\r\n";

	{
		auto privateConstantInfo = GeneratePrivateConstants(inputPixelShader);
		result += privateConstantInfo.first;
		m_constantRegisterMap.insert(std::begin(privateConstantInfo.second), std::end(privateConstantInfo.second));
	}

	{
		auto temporaryInfo = GenerateTemporaries(inputPixelShader);
		m_temporaryRegisterMap = std::move(temporaryInfo.second);
		result += temporaryInfo.first;
	}

	result += GenerateInstructions(inputPixelShader);

	result += "\treturn oC0;\r\n";
	result += "}\r\n";

	return result;
}

std::string CDx11UmbralEffectGenerator::GenerateInstructions(const CD3DShader& inputShader) const
{
	std::string result;

	for(const auto& instruction : inputShader.GetInstructions())
	{
		switch(instruction.token.opcode)
		{
		case CD3DShader::OPCODE_DCL:
		case CD3DShader::OPCODE_DEF:
			break;
		case CD3DShader::OPCODE_MOV:
			{
				assert(instruction.token.size == 2);
				auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
				auto src1Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);

				auto dstString = PrintDestinationOperand(dstParam);
				auto src1String = PrintSourceOperand(src1Param, dstParam.writeMask);

				result += string_format("\t%s = %s;\r\n", dstString.c_str(), src1String.c_str());
			}
			break;
		case CD3DShader::OPCODE_ADD:
			{
				assert(instruction.token.size == 3);
				auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
				auto src1Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
				auto src2Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[2]);

				auto dstString = PrintDestinationOperand(dstParam);
				auto src1String = PrintSourceOperand(src1Param, dstParam.writeMask);
				auto src2String = PrintSourceOperand(src2Param, dstParam.writeMask);

				result += string_format("\t%s = %s + %s;\r\n", dstString.c_str(), src1String.c_str(), src2String.c_str());
			}
			break;
		case CD3DShader::OPCODE_MUL:
			{
				assert(instruction.token.size == 3);
				auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
				auto src1Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
				auto src2Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[2]);

				auto dstString = PrintDestinationOperand(dstParam);
				auto src1String = PrintSourceOperand(src1Param, dstParam.writeMask);
				auto src2String = PrintSourceOperand(src2Param, dstParam.writeMask);

				result += string_format("\t%s = %s * %s;\r\n", dstString.c_str(), src1String.c_str(), src2String.c_str());
			}
			break;
		case CD3DShader::OPCODE_MAD:
			{
				assert(instruction.token.size == 4);
				auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
				auto src1Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
				auto src2Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[2]);
				auto src3Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[3]);

				auto dstString = PrintDestinationOperand(dstParam);
				auto src1String = PrintSourceOperand(src1Param, dstParam.writeMask);
				auto src2String = PrintSourceOperand(src2Param, dstParam.writeMask);
				auto src3String = PrintSourceOperand(src3Param, dstParam.writeMask);

				result += string_format("\t%s = %s * %s + %s;\r\n", dstString.c_str(), src1String.c_str(), src2String.c_str(), src3String.c_str());
			}
			break;
		case CD3DShader::OPCODE_DP3:
			{
				assert(instruction.token.size == 3);
				auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
				auto src1Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
				auto src2Param = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&instruction.additionalTokens[2]);

				auto dstString = PrintDestinationOperand(dstParam);
				auto src1String = PrintSourceOperand(src1Param, 0x07);
				auto src2String = PrintSourceOperand(src2Param, 0x07);

				result += string_format("\t%s = dot(%s, %s);\r\n", dstString.c_str(), src1String.c_str(), src2String.c_str());
			}
			break;
		default:
			assert(0);
			break;
		}
	}

	return result;
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GenerateConstants(const CD3DShaderConstantTable& constantTable)
{
	std::string constantsText;
	RegisterIndexMap constantIndices;

	constantsText += "cbuffer GlobalConstants\r\n";
	constantsText += "{\r\n";

	for(const auto& constant : constantTable.GetConstants())
	{
		if(constant.info.registerSet != CD3DShader::SHADER_REGISTER_CONST) continue;
		assert(constant.typeInfo.type == CD3DShaderConstantTable::CONSTANT_TYPE_FLOAT);
		switch(constant.typeInfo.typeClass)
		{
		case CD3DShaderConstantTable::CONSTANT_CLASS_VECTOR:
			constantsText += string_format("\tfloat4 %s;\r\n", constant.name.c_str());
			constantIndices.insert(std::make_pair(constant.info.registerIndex, constant.name));
			break;
		case CD3DShaderConstantTable::CONSTANT_CLASS_MATRIX_COLS:
			{
				constantsText += string_format("\tmatrix %s;\r\n", constant.name.c_str());
				for(unsigned int i = 0; i < 4; i++)
				{
					unsigned int registerIndex = constant.info.registerIndex + i;
					auto varName = string_format("%s[%d]", constant.name.c_str(), i);
					constantIndices.insert(std::make_pair(registerIndex, varName));
				}
			}
			break;
		default:
			assert(0);
			break;
		}
	}

	constantsText += "};\r\n";

	return std::make_pair(constantsText, constantIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GeneratePrivateConstants(const CD3DShader& inputShader)
{
	std::string constantsText;
	RegisterIndexMap constantsIndices;
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DEF) continue;
		assert(instruction.token.size == 5);
		auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
		assert(dstParam.GetRegisterType() == CD3DShader::SHADER_REGISTER_CONST);
		float cst[4] =
		{
			*reinterpret_cast<const float*>(instruction.additionalTokens.data() + 1),
			*reinterpret_cast<const float*>(instruction.additionalTokens.data() + 2),
			*reinterpret_cast<const float*>(instruction.additionalTokens.data() + 3),
			*reinterpret_cast<const float*>(instruction.additionalTokens.data() + 4),
		};
		auto constantName = string_format("privateConst%d", dstParam.registerNumber);
		constantsText += string_format("\tfloat4 %s = float4(%f, %f, %f, %f);\r\n", constantName.c_str(), cst[0], cst[1], cst[2], cst[3]);
		auto registerNumber = dstParam.registerNumber;
		constantsIndices.insert(std::make_pair(registerNumber, constantName));
	}
	return std::make_pair(constantsText, constantsIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GenerateTemporaries(const CD3DShader& inputShader)
{
	std::string temporariesText;
	RegisterIndexMap temporaryIndices;
	std::set<uint32> temporaries;
	//Not 100% accurate, might give false temporaries, but we don't really care
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode == CD3DShader::OPCODE_DCL) continue;
		if(instruction.token.opcode == CD3DShader::OPCODE_DEF) continue;
		for(const auto& token : instruction.additionalTokens)
		{
			auto param = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&token);
			if(param.GetRegisterType() != CD3DShader::SHADER_REGISTER_TEMP) continue;
			temporaries.insert(param.registerNumber);
		}
	}
	for(const auto& temporary : temporaries)
	{
		auto temporaryName = string_format("temp%d", temporary);
		temporariesText += string_format("\tfloat4 %s;\r\n", temporaryName.c_str());
		temporaryIndices.insert(std::make_pair(temporary, temporaryName));
	}
	return std::make_pair(temporariesText, temporaryIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GenerateVertexInputStructure(const CD3DShader& inputShader)
{
	std::string structureText;
	RegisterIndexMap registerIndices;
	unsigned int inputIndex = 0;

	structureText += "struct SHADER_IN\r\n";
	structureText += "{\r\n";

	//Check all DCL
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DCL) continue;
		assert(instruction.token.size == 2);
		auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
		if(dstParam.GetRegisterType() != CD3DShader::SHADER_REGISTER_INPUT) continue;
		auto usage = static_cast<CD3DShader::SHADER_DECLUSAGE_TYPE>(instruction.additionalTokens[0] & 0x1F);
		auto usageIndex = (instruction.additionalTokens[0] >> 16) & 0x0F;

		auto usageText = PrintDeclUsage(usage, usageIndex);
		auto varName = string_format("var%d", inputIndex++);
		auto globalName = "input." + varName;
		structureText += string_format("\tfloat4 %s : %s;\r\n", varName.c_str(), usageText.c_str());

		auto registerNumber = dstParam.registerNumber;
		registerIndices.insert(std::make_pair(registerNumber, globalName));
	}
	
	structureText += "};\r\n";
	
	return std::make_pair(structureText, registerIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GeneratePixelInputStructure(const CD3DShader& inputVertexShader, const CD3DShader& inputPixelShader)
{
	auto findMatchingPixelInstruction = 
		[](const CD3DShader& inputPixelShader, const CD3DShader::INSTRUCTION& refInstruction) -> CD3DShader::INSTRUCTION
		{
			auto refDstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&refInstruction.additionalTokens[1]);
			auto refUsage = static_cast<CD3DShader::SHADER_DECLUSAGE_TYPE>(refInstruction.additionalTokens[0] & 0x1F);
			auto refUsageIndex = (refInstruction.additionalTokens[0] >> 16) & 0x0F;
			for(const auto& instruction : inputPixelShader.GetInstructions())
			{
				if(instruction.token.opcode != CD3DShader::OPCODE_DCL) continue;
				assert(instruction.token.size == 2);
				auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
				if(dstParam.GetRegisterType() != CD3DShader::SHADER_REGISTER_INPUT) continue;
				auto usage = static_cast<CD3DShader::SHADER_DECLUSAGE_TYPE>(instruction.additionalTokens[0] & 0x1F);
				auto usageIndex = (instruction.additionalTokens[0] >> 16) & 0x0F;
				if((usage == refUsage) && (usageIndex == refUsageIndex))
				{
					return instruction;
				}
			}
			CD3DShader::INSTRUCTION instruction;
			instruction.token.opcode = CD3DShader::OPCODE_NOP;
			return instruction;
		};

	std::string structureText;
	RegisterIndexMap registerIndices;
	unsigned int inputIndex = 0;

	structureText += "struct SHADER_IN\r\n";
	structureText += "{\r\n";

	//Output structure from vertex shader must match the pixel shader's input structure per DX11's requirements
	//So, we generate the input structure for the pixel shader based on the vertex shader's output and bind variables according 
	//to the input DCLs from the pixel shader

	for(const auto& instruction : inputVertexShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DCL) continue;
		assert(instruction.token.size == 2);
		auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
		if(dstParam.GetRegisterType() != CD3DShader::SHADER_REGISTER_OUTPUT) continue;
		auto usage = static_cast<CD3DShader::SHADER_DECLUSAGE_TYPE>(instruction.additionalTokens[0] & 0x1F);
		auto usageIndex = (instruction.additionalTokens[0] >> 16) & 0x0F;

		auto varType = PrintVarTypeFromWriteMask(dstParam.writeMask);
		auto usageText = ((usage == 0) && (usageIndex == 0)) ? "SV_POSITION" : PrintDeclUsage(usage, usageIndex);
		auto varName = string_format("var%d", inputIndex++);
		std::string globalName = "input." + varName;
		structureText += string_format("\t%s %s : %s;\r\n", varType.c_str(), varName.c_str(), usageText.c_str());

		auto pixelDclInstruction = findMatchingPixelInstruction(inputPixelShader, instruction);
		if(pixelDclInstruction.token.opcode == CD3DShader::OPCODE_DCL)
		{
			auto pixelDclDstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&pixelDclInstruction.additionalTokens[1]);
			assert(pixelDclDstParam.GetRegisterType() == CD3DShader::SHADER_REGISTER_INPUT);
			auto registerNumber = pixelDclDstParam.registerNumber;
			registerIndices.insert(std::make_pair(registerNumber, globalName));
		}
	}
	
	structureText += "};\r\n";
	
	return std::make_pair(structureText, registerIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GenerateOutputStructure(const CD3DShader& inputShader)
{
	std::string structureText;
	RegisterIndexMap registerIndices;
	unsigned int inputIndex = 0;

	structureText += "struct SHADER_OUT\r\n";
	structureText += "{\r\n";

	//Check all DCL
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DCL) continue;
		assert(instruction.token.size == 2);
		auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
		if(dstParam.GetRegisterType() != CD3DShader::SHADER_REGISTER_OUTPUT) continue;
		auto usage = static_cast<CD3DShader::SHADER_DECLUSAGE_TYPE>(instruction.additionalTokens[0] & 0x1F);
		auto usageIndex = (instruction.additionalTokens[0] >> 16) & 0x0F;

		auto varType = PrintVarTypeFromWriteMask(dstParam.writeMask);
		auto usageText = ((usage == 0) && (usageIndex == 0)) ? "SV_POSITION" : PrintDeclUsage(usage, usageIndex);
		auto varName = string_format("var%d", inputIndex++);
		auto globalName = "output." + varName;
		structureText += string_format("\t%s %s : %s;\r\n", varType.c_str(), varName.c_str(), usageText.c_str());

		auto registerNumber = dstParam.registerNumber;
		registerIndices.insert(std::make_pair(registerNumber, globalName));
	}
	
	structureText += "};\r\n";
	
	return std::make_pair(structureText, registerIndices);
}

std::string CDx11UmbralEffectGenerator::PrintSourceOperand(const CD3DShader::SOURCE_PARAMETER_TOKEN& srcToken, uint32 writeMask) const
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

std::string CDx11UmbralEffectGenerator::PrintDestinationOperand(const CD3DShader::DESTINATION_PARAMETER_TOKEN& dstToken) const
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

std::string CDx11UmbralEffectGenerator::PrintParameterRegister(CD3DShader::SHADER_REGISTER_TYPE registerType, uint32 registerNumber) const
{
	switch(registerType)
	{
	case CD3DShader::SHADER_REGISTER_TEMP:
		{
			auto registerIterator = m_temporaryRegisterMap.find(registerNumber);
			if(registerIterator != std::end(m_temporaryRegisterMap))
			{
				return registerIterator->second;
			}
			else
			{
				assert(0);
				return string_format("t%d", registerNumber);
			}
		}
		break;
	case CD3DShader::SHADER_REGISTER_INPUT:
		{
			auto registerIterator = m_inputRegisterMap.find(registerNumber);
			if(registerIterator != std::end(m_inputRegisterMap))
			{
				return registerIterator->second;
			}
			else
			{
				assert(0);
				return string_format("v%d", registerNumber);
			}
		}
		break;
	case CD3DShader::SHADER_REGISTER_CONST:
		{
			auto registerIterator = m_constantRegisterMap.find(registerNumber);
			if(registerIterator != std::end(m_constantRegisterMap))
			{
				return registerIterator->second;
			}
			else
			{
				assert(0);
				return string_format("c%d", registerNumber);
			}
		}
		break;
	case CD3DShader::SHADER_REGISTER_OUTPUT:
		{
			auto registerIterator = m_outputRegisterMap.find(registerNumber);
			if(registerIterator != std::end(m_outputRegisterMap))
			{
				return registerIterator->second;
			}
			else
			{
				assert(0);
				return string_format("o%d", registerNumber);
			}
		}
		break;
	case CD3DShader::SHADER_REGISTER_CONSTINT:
		return string_format("i%d", registerNumber);
	case CD3DShader::SHADER_REGISTER_COLOROUT:
		return string_format("oC%d", registerNumber);
	case CD3DShader::SHADER_REGISTER_CONSTBOOL:
		return string_format("b%d", registerNumber);
	default:
		assert(0);
		return string_format("?%d", registerNumber);
	}
}

std::string CDx11UmbralEffectGenerator::PrintDeclUsage(CD3DShader::SHADER_DECLUSAGE_TYPE usage, unsigned int usageIndex)
{
	std::string usageText;
	switch(usage)
	{
	case CD3DShader::SHADER_DECLUSAGE_POSITION:
		usageText = string_format("POSITION%d", usageIndex);
		break;
	case CD3DShader::SHADER_DECLUSAGE_NORMAL:
		usageText = string_format("NORMAL%d", usageIndex);
		break;
	case CD3DShader::SHADER_DECLUSAGE_COLOR:
		usageText = string_format("COLOR%d", usageIndex);
		break;
	default:
		assert(0);
		break;
	}
	return usageText;
}

std::string CDx11UmbralEffectGenerator::PrintVarTypeFromWriteMask(uint32 writeMask)
{
	std::string varType;
	switch(writeMask)
	{
	case 0x07:
		varType = "float3";
		break;
	case 0x0F:
		varType = "float4";
		break;
	default:
		assert(0);
		varType = "float4";
		break;
	}
	return varType;
}
