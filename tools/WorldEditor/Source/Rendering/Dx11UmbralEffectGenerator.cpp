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
	result += "cbuffer GlobalConstants\r\n";
	result += "{\r\n";

	{
		auto constantTable = inputVertexShader.GetConstantTable();
		auto constantInfo = GenerateConstants(constantTable);
		result += constantInfo.first;
		m_constantRegisterMap.insert(std::begin(constantInfo.second), std::end(constantInfo.second));
	}

	{
		auto constantTable = inputVertexShader.GetConstantTable();
		auto constantInfo = GenerateBoolConstants(constantTable);
		result += constantInfo.first;
		m_boolConstantRegisterMap.insert(std::begin(constantInfo.second), std::end(constantInfo.second));
	}

	result += "}\r\n";

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
		auto privateConstantInfo = GeneratePrivateConstants(inputVertexShader);
		result += privateConstantInfo.first;
		m_constantRegisterMap.insert(std::begin(privateConstantInfo.second), std::end(privateConstantInfo.second));
	}

	{
		auto privateIntConstantInfo = GeneratePrivateIntConstants(inputVertexShader);
		result += privateIntConstantInfo.first;
		m_intConstantRegisterMap.insert(std::begin(privateIntConstantInfo.second), std::end(privateIntConstantInfo.second));
	}

	{
		auto temporaryInfo = GenerateTemporaries(inputVertexShader);
		m_temporaryRegisterMap = std::move(temporaryInfo.second);
		result += temporaryInfo.first;
	}

	{
		auto temporaryInfo = GenerateAddressTemporaries(inputVertexShader);
		m_addressTemporaryRegisterMap = std::move(temporaryInfo.second);
		result += temporaryInfo.first;
	}

	result += GenerateInstructions(inputVertexShader);

	result += "\treturn output;\r\n";
	result += "}\r\n";

	return result;
}

std::string CDx11UmbralEffectGenerator::GeneratePixelShaderInternal(const CD3DShader& inputVertexShader, const CD3DShader& inputPixelShader)
{
	assert(inputVertexShader.GetType() == CD3DShader::SHADER_TYPE_VERTEX);
	assert(inputPixelShader.GetType() == CD3DShader::SHADER_TYPE_PIXEL);

	std::string result;

	auto constantTable = inputPixelShader.GetConstantTable();

	//Generate constants
	result += "cbuffer GlobalConstants\r\n";
	result += "{\r\n";

	{
		auto constantInfo = GenerateConstants(constantTable);
		result += constantInfo.first;
		m_constantRegisterMap.insert(std::begin(constantInfo.second), std::end(constantInfo.second));
	}

	result += "}\r\n";

	//Generate samplers
	{
		auto samplerInfo = GenerateSamplers(constantTable);
		result += samplerInfo.first;
		m_samplerRegisterMap = samplerInfo.second;
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
	static const auto makeIdentationString = 
		[](unsigned int identationLevel)
		{
			std::string result;
			for(unsigned int i = 0; i <= identationLevel; i++)
			{
				result += "\t";
			}
			return result;
		};

	static const auto emitGenericUnaryOperation =
		[&](CD3DShader::CTokenStream& tokenStream, const std::string& identationString, const char* format)
		{
			auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
			auto srcParam = CD3DShader::ReadSourceParameter(tokenStream);

			auto dstString = PrintDestinationOperand(dstParam);
			auto srcString = PrintSourceOperand(srcParam, dstParam.parameter.writeMask);

			return string_format(format, identationString.c_str(), dstString.c_str(), srcString.c_str());
		};

	static const auto emitGenericBinaryOperation =
		[&](CD3DShader::CTokenStream& tokenStream, const std::string& identationString, const char* format)
		{
			auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
			auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
			auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);

			auto dstString = PrintDestinationOperand(dstParam);
			auto src1String = PrintSourceOperand(src1Param, dstParam.parameter.writeMask);
			auto src2String = PrintSourceOperand(src2Param, dstParam.parameter.writeMask);

			return string_format(format, identationString.c_str(), dstString.c_str(), src1String.c_str(), src2String.c_str());
		};

	std::string result;
	unsigned int identationLevel = 0;
	std::string identationString = makeIdentationString(identationLevel);

	for(const auto& instruction : inputShader.GetInstructions())
	{
		CD3DShader::CTokenStream tokenStream(instruction.additionalTokens);
		switch(instruction.token.opcode)
		{
		case CD3DShader::OPCODE_DCL:
		case CD3DShader::OPCODE_DEF:
		case CD3DShader::OPCODE_DEFI:
			break;
		case CD3DShader::OPCODE_MOV:
		case CD3DShader::OPCODE_MOVA:
			result += emitGenericUnaryOperation(tokenStream, identationString, "%s%s = %s;\r\n");
			break;
		case CD3DShader::OPCODE_ADD:
			result += emitGenericBinaryOperation(tokenStream, identationString, "%s%s = %s + %s;\r\n");
			break;
		case CD3DShader::OPCODE_MAD:
			{
				auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
				auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
				auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
				auto src3Param = CD3DShader::ReadSourceParameter(tokenStream);

				auto dstString = PrintDestinationOperand(dstParam);
				auto src1String = PrintSourceOperand(src1Param, dstParam.parameter.writeMask);
				auto src2String = PrintSourceOperand(src2Param, dstParam.parameter.writeMask);
				auto src3String = PrintSourceOperand(src3Param, dstParam.parameter.writeMask);

				result += string_format("%s%s = %s * %s + %s;\r\n", identationString.c_str(), 
					dstString.c_str(), src1String.c_str(), src2String.c_str(), src3String.c_str());
			}
			break;
		case CD3DShader::OPCODE_MUL:
			result += emitGenericBinaryOperation(tokenStream, identationString, "%s%s = %s * %s;\r\n");
			break;
		case CD3DShader::OPCODE_RCP:
			result += emitGenericUnaryOperation(tokenStream, identationString, "%s%s = rcp(%s);\r\n");
			break;
		case CD3DShader::OPCODE_RSQ:
			result += emitGenericUnaryOperation(tokenStream, identationString, "%s%s = rsqrt(%s);\r\n");
			break;
		case CD3DShader::OPCODE_DP3:
			{
				auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
				auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
				auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);

				auto dstString = PrintDestinationOperand(dstParam);
				auto src1String = PrintSourceOperand(src1Param, 0x07);
				auto src2String = PrintSourceOperand(src2Param, 0x07);

				result += string_format("%s%s = dot(%s, %s);\r\n", identationString.c_str(), 
					dstString.c_str(), src1String.c_str(), src2String.c_str());
			}
			break;
		case CD3DShader::OPCODE_DP4:
			{
				auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
				auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
				auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);

				auto dstString = PrintDestinationOperand(dstParam);
				auto src1String = PrintSourceOperand(src1Param, 0x0F);
				auto src2String = PrintSourceOperand(src2Param, 0x0F);

				result += string_format("%s%s = dot(%s, %s);\r\n", identationString.c_str(), 
					dstString.c_str(), src1String.c_str(), src2String.c_str());
			}
			break;
		case CD3DShader::OPCODE_MAX:
			result += emitGenericBinaryOperation(tokenStream, identationString, "%s%s = max(%s, %s);\r\n");
			break;
		case CD3DShader::OPCODE_SLT:
			result += emitGenericBinaryOperation(tokenStream, identationString, "%s%s = (%s < %s) ? 1 : 0;\r\n");
			break;
		case CD3DShader::OPCODE_EXP:
			result += emitGenericUnaryOperation(tokenStream, identationString, "%s%s = exp2(%s);\r\n");
			break;
		case CD3DShader::OPCODE_LOG:
			result += emitGenericUnaryOperation(tokenStream, identationString, "%s%s = log2(%s);\r\n");
			break;
		case CD3DShader::OPCODE_FRC:
			result += emitGenericUnaryOperation(tokenStream, identationString, "%s%s = frac(%s);\r\n");
			break;
		case CD3DShader::OPCODE_LOOP:
			{
				auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
				auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
				assert(src1Param.parameter.GetRegisterType() == CD3DShader::SHADER_REGISTER_LOOP);
				assert(src2Param.parameter.GetRegisterType() == CD3DShader::SHADER_REGISTER_CONSTINT);

				auto counterInfoVariable = GetVariableForRegister(src2Param.parameter.GetRegisterType(), src2Param.parameter.registerNumber);

				result += string_format("%sfor(int loopCounter = %d; loopCounter < %d; loopCounter += %d)\r\n", identationString.c_str(),
					counterInfoVariable.constantValue[1], counterInfoVariable.constantValue[0] + counterInfoVariable.constantValue[1], counterInfoVariable.constantValue[2]);
				result += string_format("%s{\r\n", identationString.c_str());

				identationLevel++;
				identationString = makeIdentationString(identationLevel);
			}
			break;
		case CD3DShader::OPCODE_NRM:
			result += emitGenericUnaryOperation(tokenStream, identationString, "%s%s = normalize(%s);\r\n");
			break;
		case CD3DShader::OPCODE_IF:
			{
				auto srcParam = CD3DShader::ReadSourceParameter(tokenStream);

				auto srcParamString = PrintSourceOperand(srcParam, 0);

				result += string_format("%sif(%s)\r\n", identationString.c_str(), srcParamString.c_str());
				result += string_format("%s{\r\n", identationString.c_str());

				identationLevel++;
				identationString = makeIdentationString(identationLevel);
			}
			break;
		case CD3DShader::OPCODE_ENDLOOP:
		case CD3DShader::OPCODE_ENDIF:
			{
				assert(identationLevel != 0);

				identationLevel--;
				identationString = makeIdentationString(identationLevel);

				result += string_format("%s}\r\n", identationString.c_str());
			}
			break;
		case CD3DShader::OPCODE_TEXLD:
			{
				auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
				auto locationParam = CD3DShader::ReadSourceParameter(tokenStream);
				auto samplerParam = CD3DShader::ReadSourceParameter(tokenStream);

				assert(samplerParam.parameter.GetRegisterType() == CD3DShader::SHADER_REGISTER_SAMPLER);

				auto dstString = PrintDestinationOperand(dstParam);
				auto samplerVariable = GetVariableForRegister(CD3DShader::SHADER_REGISTER_SAMPLER, samplerParam.parameter.registerNumber);
				auto textureString = string_format("%s_texture", samplerVariable.name.c_str());
				auto locationVariable = GetVariableForRegister(locationParam.parameter.GetRegisterType(), locationParam.parameter.registerNumber);

				result += string_format("%s%s = %s.Sample(%s, %s);\r\n", identationString.c_str(), 
					dstString.c_str(), textureString.c_str(), samplerVariable.name.c_str(), locationVariable.name.c_str());
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

	for(const auto& constant : constantTable.GetConstants())
	{
		if(constant.info.registerSet != CD3DShaderConstantTable::REGISTER_SET_FLOAT4) continue;
		assert(constant.typeInfo.type == CD3DShaderConstantTable::CONSTANT_TYPE_FLOAT);
		switch(constant.typeInfo.typeClass)
		{
		case CD3DShaderConstantTable::CONSTANT_CLASS_VECTOR:
			{
				if(constant.typeInfo.elements > 1)
				{
					constantsText += string_format("\tfloat4 %s[%d];\r\n", constant.name.c_str(), constant.typeInfo.elements);
				}
				else
				{
					constantsText += string_format("\tfloat4 %s;\r\n", constant.name.c_str());
				}
				for(unsigned int i = 0; i < constant.typeInfo.elements; i++)
				{
					VARIABLE_INFO variable(constant.name);
					variable.isArray = constant.typeInfo.elements > 1;
					variable.subscript = i;
					constantIndices.insert(std::make_pair(constant.info.registerIndex + i, variable));
				}
			}
			break;
		case CD3DShaderConstantTable::CONSTANT_CLASS_MATRIX_COLS:
			{
				constantsText += string_format("\tmatrix %s;\r\n", constant.name.c_str());
				for(unsigned int i = 0; i < constant.info.registerCount; i++)
				{
					unsigned int registerIndex = constant.info.registerIndex + i;
					auto varName = string_format("%s[%d]", constant.name.c_str(), i);
					constantIndices.insert(std::make_pair(registerIndex, VARIABLE_INFO(varName)));
				}
			}
			break;
		default:
			assert(0);
			break;
		}
	}

	return std::make_pair(constantsText, constantIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GenerateBoolConstants(const CD3DShaderConstantTable& constantTable)
{
	std::string constantsText;
	RegisterIndexMap constantIndices;

	for(const auto& constant : constantTable.GetConstants())
	{
		if(constant.info.registerSet != CD3DShaderConstantTable::REGISTER_SET_BOOL) continue;
		assert(constant.typeInfo.type == CD3DShaderConstantTable::CONSTANT_TYPE_BOOL);
		assert(constant.typeInfo.typeClass == CD3DShaderConstantTable::CONSTANT_CLASS_SCALAR);
		constantsText += string_format("\tbool %s;\r\n", constant.name.c_str());
		constantIndices.insert(std::make_pair(constant.info.registerIndex, VARIABLE_INFO(constant.name)));
	}

	return std::make_pair(constantsText, constantIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GenerateSamplers(const CD3DShaderConstantTable& constantTable)
{
	std::string samplersText;
	RegisterIndexMap samplerIndices;

	for(const auto& constant : constantTable.GetConstants())
	{
		if(constant.info.registerSet != CD3DShaderConstantTable::REGISTER_SET_SAMPLER) continue;
		if(constant.typeInfo.type != CD3DShaderConstantTable::CONSTANT_TYPE_SAMPLER2D) continue;
		//In SM3.0, texture object binding was a part of the sampler state
		//in SM4.0+, texture object binding is separated from the sampler state, this is why we declare a texture
		auto samplerName = string_format("sampler_%s", constant.name.c_str());
		auto samplerTextureName = string_format("%s_texture", samplerName.c_str());
		samplersText += string_format("SamplerState %s;\r\n", samplerName.c_str());
		samplersText += string_format("Texture2D %s;\r\n", samplerTextureName.c_str());
		samplerIndices.insert(std::make_pair(constant.info.registerIndex, VARIABLE_INFO(samplerName)));
	}

	return std::make_pair(samplersText, samplerIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GeneratePrivateConstants(const CD3DShader& inputShader)
{
	std::string constantsText;
	RegisterIndexMap constantsIndices;
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DEF) continue;
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
		constantsIndices.insert(std::make_pair(registerNumber, VARIABLE_INFO(constantName)));
	}
	return std::make_pair(constantsText, constantsIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GeneratePrivateIntConstants(const CD3DShader& inputShader)
{
	std::string constantsText;
	RegisterIndexMap constantsIndices;
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DEFI) continue;
		auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
		assert(dstParam.GetRegisterType() == CD3DShader::SHADER_REGISTER_CONSTINT);
		int32 cst[4] =
		{
			static_cast<int32>(instruction.additionalTokens[1]),
			static_cast<int32>(instruction.additionalTokens[2]),
			static_cast<int32>(instruction.additionalTokens[3]),
			static_cast<int32>(instruction.additionalTokens[4]),
		};
		auto constantName = string_format("privateIntConst%d", dstParam.registerNumber);
		constantsText += string_format("\tint4 %s = int4(%d, %d, %d, %d);\r\n", constantName.c_str(), cst[0], cst[1], cst[2], cst[3]);
		auto registerNumber = dstParam.registerNumber;
		VARIABLE_INFO variable;
		variable.name = constantName;
		memcpy(variable.constantValue, cst, sizeof(variable.constantValue));
		constantsIndices.insert(std::make_pair(registerNumber, variable));
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
		if(instruction.token.opcode == CD3DShader::OPCODE_DEFI) continue;
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
		temporaryIndices.insert(std::make_pair(temporary, VARIABLE_INFO(temporaryName)));
	}
	return std::make_pair(temporariesText, temporaryIndices);
}

CDx11UmbralEffectGenerator::StructureDef CDx11UmbralEffectGenerator::GenerateAddressTemporaries(const CD3DShader& inputShader)
{
	std::string temporariesText;
	RegisterIndexMap temporaryIndices;
	std::set<uint32> temporaries;
	//Not 100% accurate, might give false temporaries, but we don't really care
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode == CD3DShader::OPCODE_DCL) continue;
		if(instruction.token.opcode == CD3DShader::OPCODE_DEF) continue;
		if(instruction.token.opcode == CD3DShader::OPCODE_DEFI) continue;
		for(const auto& token : instruction.additionalTokens)
		{
			auto param = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&token);
			if(param.GetRegisterType() != CD3DShader::SHADER_REGISTER_TEXTURE) continue;
			temporaries.insert(param.registerNumber);
		}
	}
	for(const auto& temporary : temporaries)
	{
		auto temporaryName = string_format("tempAddr%d", temporary);
		temporariesText += string_format("\tint4 %s;\r\n", temporaryName.c_str());
		temporaryIndices.insert(std::make_pair(temporary, VARIABLE_INFO(temporaryName)));
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
		registerIndices.insert(std::make_pair(registerNumber, VARIABLE_INFO(globalName)));
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
			registerIndices.insert(std::make_pair(registerNumber, VARIABLE_INFO(globalName)));
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
		registerIndices.insert(std::make_pair(registerNumber, VARIABLE_INFO(globalName)));
	}
	
	structureText += "};\r\n";
	
	return std::make_pair(structureText, registerIndices);
}

std::string CDx11UmbralEffectGenerator::PrintSourceOperand(const CD3DShader::SOURCE_PARAMETER& srcParam, uint32 writeMask) const
{
	static const char* g_element[4] =
	{
		"x",
		"y",
		"z",
		"w"
	};
	auto regVariable = GetVariableForRegister(srcParam.parameter.GetRegisterType(), srcParam.parameter.registerNumber);
	auto regString = regVariable.name;
	auto regSubscript = regVariable.isArray ? string_format("[%d]", regVariable.subscript) : "";
	if(srcParam.parameter.useRelativeAddressing)
	{
		assert(regVariable.isArray);
		auto addrVariable = GetVariableForRegister(srcParam.relativeAddress.GetRegisterType(), srcParam.relativeAddress.registerNumber);
		auto addrString = addrVariable.name;
		addrString += ".";
		addrString += g_element[srcParam.relativeAddress.swizzleX];
		regSubscript = string_format("[%s + %d]", addrString.c_str(), regVariable.subscript);
	}
	regString += regSubscript;
	switch(srcParam.parameter.sourceModifier)
	{
	case CD3DShader::SOURCE_MODIFIER_NONE:
		break;
	case CD3DShader::SOURCE_MODIFIER_NEGATE:
		regString = string_format("-%s", regString.c_str());
		break;
	default:
		assert(0);
		break;
	}
	if(writeMask != 0)
	{
		regString += ".";
		if(writeMask & 0x1) regString += g_element[srcParam.parameter.swizzleX];
		if(writeMask & 0x2) regString += g_element[srcParam.parameter.swizzleY];
		if(writeMask & 0x4) regString += g_element[srcParam.parameter.swizzleZ];
		if(writeMask & 0x8) regString += g_element[srcParam.parameter.swizzleW];
	}
	return regString;
}

std::string CDx11UmbralEffectGenerator::PrintDestinationOperand(const CD3DShader::DESTINATION_PARAMETER& dstParam) const
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
	auto regVariable = GetVariableForRegister(dstParam.parameter.GetRegisterType(), dstParam.parameter.registerNumber);
	auto regString = regVariable.name;
	regString += ".";
	regString += g_writeMask[dstParam.parameter.writeMask];
	return regString;
}

CDx11UmbralEffectGenerator::VARIABLE_INFO CDx11UmbralEffectGenerator::GetVariableForRegister(CD3DShader::SHADER_REGISTER_TYPE registerType, uint32 registerNumber) const
{
	static const auto getRegisterFromMap =
		[] (const RegisterIndexMap& registerMap, uint32 registerNumber, const char* defaultFormat) -> VARIABLE_INFO
		{
			auto registerIterator = registerMap.find(registerNumber);
			if(registerIterator != std::end(registerMap))
			{
				return registerIterator->second;
			}
			else
			{
				assert(0);
				return VARIABLE_INFO(string_format(defaultFormat, registerNumber));
			}
		};

	switch(registerType)
	{
	case CD3DShader::SHADER_REGISTER_TEMP:
		return getRegisterFromMap(m_temporaryRegisterMap, registerNumber, "t%d");
	case CD3DShader::SHADER_REGISTER_INPUT:
		return getRegisterFromMap(m_inputRegisterMap, registerNumber, "v%d");
	case CD3DShader::SHADER_REGISTER_TEXTURE:
		return getRegisterFromMap(m_addressTemporaryRegisterMap, registerNumber, "a%d");
	case CD3DShader::SHADER_REGISTER_CONST:
		return getRegisterFromMap(m_constantRegisterMap, registerNumber, "c%d");
	case CD3DShader::SHADER_REGISTER_OUTPUT:
		return getRegisterFromMap(m_outputRegisterMap, registerNumber, "o%d");
	case CD3DShader::SHADER_REGISTER_CONSTINT:
		return getRegisterFromMap(m_intConstantRegisterMap, registerNumber, "i%d");
	case CD3DShader::SHADER_REGISTER_COLOROUT:
		return VARIABLE_INFO(string_format("oC%d", registerNumber));
	case CD3DShader::SHADER_REGISTER_SAMPLER:
		return getRegisterFromMap(m_samplerRegisterMap, registerNumber, "s%d");
	case CD3DShader::SHADER_REGISTER_CONSTBOOL:
		return getRegisterFromMap(m_boolConstantRegisterMap, registerNumber, "b%d");
	case CD3DShader::SHADER_REGISTER_LOOP:
		return VARIABLE_INFO("loopCounter");
		break;
	default:
		assert(0);
		return VARIABLE_INFO(string_format("?%d", registerNumber));
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
	case CD3DShader::SHADER_DECLUSAGE_TEXCOORD:
		usageText = string_format("TEXCOORD%d", usageIndex);
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
	case 0x03:
		varType = "float2";
		break;
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
