#include <assert.h>
#include <set>
#include "string_format.h"
#include "GlEsUmbralEffectGenerator.h"
#include "D3DShaderConstantTable.h"

CGlEsUmbralEffectGenerator::CGlEsUmbralEffectGenerator()
{
	for(auto& instructionEmitter : m_instructionEmitters)
	{
		instructionEmitter = nullptr;
	}
	
	//0x00
	m_instructionEmitters[CD3DShader::OPCODE_MOV]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Unary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = %s;\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_ADD]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Binary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = %s + %s;\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_MAD]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Mad, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_MUL]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Binary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = %s * %s;\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_RCP]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Unary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = 1.0 / %s;\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_RSQ]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Unary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = inversesqrt(%s);\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_DP3]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Dp3, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_DP4]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Dp4, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_MIN]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Binary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = min(%s, %s);\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_MAX]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Binary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = max(%s, %s);\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_SLT]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Binary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = vec4(lessThan(%s, %s));\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_EXP]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Unary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = exp(%s);\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_LOG]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Unary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = log(%s);\r\n");
	
	//0x10
	m_instructionEmitters[CD3DShader::OPCODE_LRP]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Lrp, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_LOOP]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Loop, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_ENDLOOP]	= std::bind(&CGlEsUmbralEffectGenerator::Emit_EndScope, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_FRC]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Unary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = fract(%s);\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_DCL]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Nop, this, std::placeholders::_1, std::placeholders::_2);
	
	//0x20
	m_instructionEmitters[CD3DShader::OPCODE_POW]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Binary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = pow(%s, %s);\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_ABS]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Unary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = abs(%s);\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_NRM]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Unary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = normalize(%s);\r\n");
	m_instructionEmitters[CD3DShader::OPCODE_REP]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Rep, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_ENDREP]	= std::bind(&CGlEsUmbralEffectGenerator::Emit_EndScope, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_IF]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_If, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_ENDIF]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_EndScope, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_MOVA]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Unary, this, std::placeholders::_1, std::placeholders::_2, "%s%s = ivec4(%s);\r\n");
	
	//0x30
	m_instructionEmitters[CD3DShader::OPCODE_DEFI]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Nop, this, std::placeholders::_1, std::placeholders::_2);
	
	//0x40
	m_instructionEmitters[CD3DShader::OPCODE_TEXLD]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Texld, this, std::placeholders::_1, std::placeholders::_2);
	
	//0x51
	m_instructionEmitters[CD3DShader::OPCODE_DEF]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Nop, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_CMP]		= std::bind(&CGlEsUmbralEffectGenerator::Emit_Cmp, this, std::placeholders::_1, std::placeholders::_2);
	m_instructionEmitters[CD3DShader::OPCODE_TEXLDL]	= std::bind(&CGlEsUmbralEffectGenerator::Emit_Texldl, this, std::placeholders::_1, std::placeholders::_2);
}

std::string CGlEsUmbralEffectGenerator::GenerateVertexShader(const CD3DShader& inputVertexShader)
{
	CGlEsUmbralEffectGenerator generator;
	return generator.GenerateVertexShaderInternal(inputVertexShader);
}

std::string CGlEsUmbralEffectGenerator::GeneratePixelShader(const CD3DShader& inputPixelShader)
{
	CGlEsUmbralEffectGenerator generator;
	return generator.GeneratePixelShaderInternal(inputPixelShader);
}

std::string CGlEsUmbralEffectGenerator::GenerateVertexShaderInternal(const CD3DShader& inputVertexShader)
{
	std::string result;
	
	auto vertexConstantTable = inputVertexShader.GetConstantTable();
	
	ParseTemporaries(inputVertexShader);
	ParseAddressTemporaries(inputVertexShader);
	ParseVertexInputs(inputVertexShader);
	ParseOutputs(inputVertexShader);
	ParseLocalConstants(inputVertexShader);
	ParseLocalIntConstants(inputVertexShader);
	ParseGlobalConstants(vertexConstantTable);
	ParseGlobalBoolConstants(vertexConstantTable);
	
	//Special registers
	{
		auto registerId = std::make_pair(CD3DShader::SHADER_REGISTER_LOOP, 0);
		m_registerNames.insert(std::make_pair(registerId, "loopCounter"));
	}
		
	result += GenerateAttributeDeclarations();
	result += GenerateVaryingDeclarations();
	result += GenerateUniformDeclarations("vs");
		
	result += "void main()\r\n";
	result += "{\r\n";
	
	result += GenerateTemporaryDeclarations();
	result += GenerateLocalConstantDeclarations();
	result += GenerateInstructions(inputVertexShader);
		
	result += "}\r\n";
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::GeneratePixelShaderInternal(const CD3DShader& inputPixelShader)
{
	std::string result;
	
	auto pixelConstantTable = inputPixelShader.GetConstantTable();
	
	ParseTemporaries(inputPixelShader);
	ParseAddressTemporaries(inputPixelShader);
	ParseVertexInputs(inputPixelShader);
	ParseOutputs(inputPixelShader);
	ParseLocalConstants(inputPixelShader);
	ParseLocalIntConstants(inputPixelShader);
	ParseGlobalConstants(pixelConstantTable);
	ParseGlobalBoolConstants(pixelConstantTable);
	ParseGlobalSamplerConstants(pixelConstantTable);
	
	//Special registers
	{
		auto registerId = std::make_pair(CD3DShader::SHADER_REGISTER_COLOROUT, 0);
		m_registerNames.insert(std::make_pair(registerId, "gl_FragColor"));
	}

	result += "precision mediump float;\r\n";

	result += GeneratePixelVaryingDeclarations();
	result += GenerateUniformDeclarations("ps");
		
	result += "void main()\r\n";
	result += "{\r\n";
	
	result += GenerateTemporaryDeclarations();
	result += GenerateLocalConstantDeclarations();
	result += GenerateInstructions(inputPixelShader);
	
	result += "}\r\n";
	
	return result;
}

static std::string GetUsageName(uint32 usage)
{
	switch(usage)
	{
		case CD3DShader::SHADER_DECLUSAGE_POSITION:
			return "position";
			break;
		case CD3DShader::SHADER_DECLUSAGE_NORMAL:
			return "normal";
			break;
		case CD3DShader::SHADER_DECLUSAGE_TEXCOORD:
			return "texcoord";
			break;
		case CD3DShader::SHADER_DECLUSAGE_COLOR:
			return "color";
			break;
		default:
			assert(0);
			return string_format("_unknown%d_", usage);
			break;
	}
}

std::string CGlEsUmbralEffectGenerator::GenerateInstructions(const CD3DShader& inputShader) const
{
	IDENTATION_STATE identationState;	
	std::string result;

	for(const auto& instruction : inputShader.GetInstructions())
	{
		CD3DShader::CTokenStream tokenStream(instruction.additionalTokens);
		const auto& instructionEmitter = m_instructionEmitters[instruction.token.opcode];
		assert(instructionEmitter != nullptr);
		result += instructionEmitter(tokenStream, identationState);
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::GenerateAttributeDeclarations()
{
	std::string result;
		
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_INPUT) continue;
		
		std::string usageName = GetUsageName(registerInfo.usage);
		
		auto attributeName = string_format("a_%s%d", usageName.c_str(), registerInfo.usageIndex);
		m_registerNames.insert(std::make_pair(registerId, attributeName));
			
		result += string_format("attribute vec4 %s;\r\n", attributeName.c_str());
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::GenerateVaryingDeclarations()
{
	std::string result;
		
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_OUTPUT) continue;
		
		if(registerInfo.usage == CD3DShader::SHADER_DECLUSAGE_POSITION && registerInfo.usageIndex == 0)
		{
			m_registerNames.insert(std::make_pair(registerId, "gl_Position"));
		}
		else
		{
			std::string usageName = GetUsageName(registerInfo.usage);
			
			auto varyingName = string_format("v_%s%d", usageName.c_str(), registerInfo.usageIndex);
			m_registerNames.insert(std::make_pair(registerId, varyingName));
						
			result += string_format("varying vec4 %s;\r\n", varyingName.c_str());
		}
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::GeneratePixelVaryingDeclarations()
{
	std::string result;
	
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_INPUT) continue;
		
		std::string usageName = GetUsageName(registerInfo.usage);
		
		auto attributeName = string_format("v_%s%d", usageName.c_str(), registerInfo.usageIndex);
		m_registerNames.insert(std::make_pair(registerId, attributeName));
		
		result += string_format("varying vec4 %s;\r\n", attributeName.c_str());
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::GenerateUniformDeclarations(const std::string& prefix)
{
	std::string result;

	//Float uniforms
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_CONST) continue;
		if(registerInfo.scope != REGISTER_INFO::SCOPE_GLOBAL) continue;

		auto uniformName = string_format("%s_%s", prefix.c_str(), registerInfo.name.c_str());
		if(registerInfo.subscript == 0)
		{
			const char* varType = registerInfo.isMatrix ? "mat4" : "vec4";
			if(registerInfo.arraySize > 1)
			{
				result += string_format("uniform %s %s[%d];\r\n", varType, uniformName.c_str(), registerInfo.arraySize);
			}
			else
			{
				result += string_format("uniform %s %s;\r\n", varType, uniformName.c_str());
			}
		}
		if(registerInfo.isMatrix)
		{
			auto uniformRegisterName = string_format("%s[%d]", uniformName.c_str(), registerInfo.subscript);
			m_registerNames.insert(std::make_pair(registerId, uniformRegisterName));
		}
		else
		{
			m_registerNames.insert(std::make_pair(registerId, uniformName));
		}
	}

	//Bool uniforms
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_CONSTBOOL) continue;
		if(registerInfo.scope != REGISTER_INFO::SCOPE_GLOBAL) continue;
		
		auto uniformName = string_format("%s_%s", prefix.c_str(), registerInfo.name.c_str());
		m_registerNames.insert(std::make_pair(registerId, uniformName));
		
		result += string_format("uniform bool %s;\r\n", uniformName.c_str());
	}

	//Sampler uniforms
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_SAMPLER) continue;
		if(registerInfo.scope != REGISTER_INFO::SCOPE_GLOBAL) continue;
		
		auto uniformName = string_format("%s_textureUnit%d", prefix.c_str(), registerId.second);
		m_registerNames.insert(std::make_pair(registerId, uniformName));
		
		const char* samplerType = registerInfo.isCubeSampler ? "samplerCube" : "sampler2D";
		
		result += string_format("uniform %s %s;\r\n", samplerType, uniformName.c_str());
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::GenerateTemporaryDeclarations()
{
	std::string result;

	//Float temporaries
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_TEMP) continue;
		if(registerInfo.scope != REGISTER_INFO::SCOPE_LOCAL) continue;
		
		auto temporaryName = string_format("temp%d", registerId.second);
		m_registerNames.insert(std::make_pair(registerId, temporaryName));
		
		result += string_format("\tvec4 %s;\r\n", temporaryName.c_str());
	}
	
	//Address temporaries
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_TEXTURE) continue;
		if(registerInfo.scope != REGISTER_INFO::SCOPE_LOCAL) continue;
		
		auto temporaryName = string_format("tempAddr%d", registerId.second);
		m_registerNames.insert(std::make_pair(registerId, temporaryName));
		
		result += string_format("\tivec4 %s;\r\n", temporaryName.c_str());
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::GenerateLocalConstantDeclarations()
{
	std::string result;
	unsigned int constantIndex = 0;
	
	//Float constants
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_CONST) continue;
		if(registerInfo.scope != REGISTER_INFO::SCOPE_LOCAL) continue;
		
		auto constantName = string_format("tempConstant%d", constantIndex++);
		m_registerNames.insert(std::make_pair(registerId, constantName));
		
		float cst[4] =
		{
			*reinterpret_cast<const float*>(registerInfo.constantValue + 0),
			*reinterpret_cast<const float*>(registerInfo.constantValue + 1),
			*reinterpret_cast<const float*>(registerInfo.constantValue + 2),
			*reinterpret_cast<const float*>(registerInfo.constantValue + 3),
		};

		result += string_format("\tvec4 %s = vec4(%f, %f, %f, %f);\r\n",
								constantName.c_str(), cst[0], cst[1], cst[2], cst[3]);
	}
	
	//Int constants
	for(const auto& registerInfoPair : m_registerInfos)
	{
		const auto& registerId = registerInfoPair.first;
		const auto& registerInfo = registerInfoPair.second;
		if(registerId.first != CD3DShader::SHADER_REGISTER_CONSTINT) continue;
		if(registerInfo.scope != REGISTER_INFO::SCOPE_LOCAL) continue;
		
		auto constantName = string_format("tempConstant%d", constantIndex++);
		m_registerNames.insert(std::make_pair(registerId, constantName));
		
		result += string_format("\tivec4 %s = ivec4(%d, %d, %d, %d);\r\n", constantName.c_str(),
								registerInfo.constantValue[0], registerInfo.constantValue[1],
								registerInfo.constantValue[2], registerInfo.constantValue[3]);
	}
	
	return result;
}

void CGlEsUmbralEffectGenerator::ParseTemporaries(const CD3DShader& inputShader)
{
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
			auto registerNumber = param.registerNumber;
			auto registerId = std::make_pair(param.GetRegisterType(), registerNumber);
			REGISTER_INFO registerInfo;
			registerInfo.scope = REGISTER_INFO::SCOPE_LOCAL;
			m_registerInfos.insert(std::make_pair(registerId, registerInfo));
		}
	}
}

void CGlEsUmbralEffectGenerator::ParseAddressTemporaries(const CD3DShader& inputShader)
{
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
			auto registerNumber = param.registerNumber;
			auto registerId = std::make_pair(param.GetRegisterType(), registerNumber);
			REGISTER_INFO registerInfo;
			registerInfo.scope = REGISTER_INFO::SCOPE_LOCAL;
			m_registerInfos.insert(std::make_pair(registerId, registerInfo));
		}
	}
}

void CGlEsUmbralEffectGenerator::ParseGlobalConstants(const CD3DShaderConstantTable& constantTable)
{
	for(const auto& constant : constantTable.GetConstants())
	{
		if(constant.info.registerSet != CD3DShaderConstantTable::REGISTER_SET_FLOAT4) continue;
		assert(constant.typeInfo.type == CD3DShaderConstantTable::CONSTANT_TYPE_FLOAT);
		switch(constant.typeInfo.typeClass)
		{
		case CD3DShaderConstantTable::CONSTANT_CLASS_SCALAR:
		case CD3DShaderConstantTable::CONSTANT_CLASS_VECTOR:
			{
				for(unsigned int i = 0; i < constant.typeInfo.elements; i++)
				{
					REGISTER_INFO registerInfo;
					registerInfo.name = constant.name;
					registerInfo.isArray = constant.typeInfo.elements > 1;
					registerInfo.arraySize = constant.typeInfo.elements;
					registerInfo.subscript = i;
					registerInfo.scope = REGISTER_INFO::SCOPE_GLOBAL;
					auto registerIndex = constant.info.registerIndex + i;
					auto registerId = std::make_pair(CD3DShader::SHADER_REGISTER_CONST, registerIndex);
					m_registerInfos.insert(std::make_pair(registerId, registerInfo));
				}
			}
			break;
		case CD3DShaderConstantTable::CONSTANT_CLASS_MATRIX_COLS:
			{
				//We probably need to differentiate this somehow
				assert(constant.typeInfo.elements == 1);
				for(unsigned int i = 0; i < constant.info.registerCount; i++)
				{
					REGISTER_INFO registerInfo;
					registerInfo.name = constant.name;
					registerInfo.isArray = constant.typeInfo.elements > 1;
					registerInfo.isMatrix = true;
					registerInfo.subscript = i;
					registerInfo.scope = REGISTER_INFO::SCOPE_GLOBAL;
					auto registerIndex = constant.info.registerIndex + i;
					auto registerId = std::make_pair(CD3DShader::SHADER_REGISTER_CONST, registerIndex);
					m_registerInfos.insert(std::make_pair(registerId, registerInfo));
				}
			}
			break;
		default:
			assert(0);
			break;
		}
	}
}

void CGlEsUmbralEffectGenerator::ParseGlobalBoolConstants(const CD3DShaderConstantTable& constantTable)
{
	for(const auto& constant : constantTable.GetConstants())
	{
		if(constant.info.registerSet != CD3DShaderConstantTable::REGISTER_SET_BOOL) continue;
		assert(constant.typeInfo.type == CD3DShaderConstantTable::CONSTANT_TYPE_BOOL);
		assert(constant.typeInfo.typeClass == CD3DShaderConstantTable::CONSTANT_CLASS_SCALAR);
		auto registerIndex = constant.info.registerIndex;
		auto registerId = std::make_pair(CD3DShader::SHADER_REGISTER_CONSTBOOL, registerIndex);
		REGISTER_INFO registerInfo;
		registerInfo.scope = REGISTER_INFO::SCOPE_GLOBAL;
		registerInfo.name = constant.name;
		m_registerInfos.insert(std::make_pair(registerId, registerInfo));
	}
}

void CGlEsUmbralEffectGenerator::ParseGlobalSamplerConstants(const CD3DShaderConstantTable& constantTable)
{
	for(const auto& constant : constantTable.GetConstants())
	{
		if(constant.info.registerSet != CD3DShaderConstantTable::REGISTER_SET_SAMPLER) continue;
		auto registerIndex = constant.info.registerIndex;
		auto registerId = std::make_pair(CD3DShader::SHADER_REGISTER_SAMPLER, registerIndex);
		REGISTER_INFO registerInfo;
		registerInfo.scope = REGISTER_INFO::SCOPE_GLOBAL;
		registerInfo.name = constant.name;
		registerInfo.isCubeSampler = (constant.typeInfo.type == CD3DShaderConstantTable::CONSTANT_TYPE_SAMPLERCUBE);
		m_registerInfos.insert(std::make_pair(registerId, registerInfo));
	}
}

void CGlEsUmbralEffectGenerator::ParseLocalConstants(const CD3DShader& inputShader)
{
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DEF) continue;
		auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
		assert(dstParam.GetRegisterType() == CD3DShader::SHADER_REGISTER_CONST);
		auto registerNumber = dstParam.registerNumber;
		auto registerId = std::make_pair(dstParam.GetRegisterType(), registerNumber);
		REGISTER_INFO registerInfo;
		registerInfo.constantValue[0] = *(instruction.additionalTokens.data() + 1);
		registerInfo.constantValue[1] = *(instruction.additionalTokens.data() + 2);
		registerInfo.constantValue[2] = *(instruction.additionalTokens.data() + 3);
		registerInfo.constantValue[3] = *(instruction.additionalTokens.data() + 4);
		registerInfo.scope = REGISTER_INFO::SCOPE_LOCAL;
		m_registerInfos.insert(std::make_pair(registerId, registerInfo));
	}
}

void CGlEsUmbralEffectGenerator::ParseLocalIntConstants(const CD3DShader& inputShader)
{
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DEFI) continue;
		auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[0]);
		assert(dstParam.GetRegisterType() == CD3DShader::SHADER_REGISTER_CONSTINT);
		auto registerNumber = dstParam.registerNumber;
		auto registerId = std::make_pair(dstParam.GetRegisterType(), registerNumber);
		REGISTER_INFO registerInfo;
		registerInfo.constantValue[0] = *(instruction.additionalTokens.data() + 1);
		registerInfo.constantValue[1] = *(instruction.additionalTokens.data() + 2);
		registerInfo.constantValue[2] = *(instruction.additionalTokens.data() + 3);
		registerInfo.constantValue[3] = *(instruction.additionalTokens.data() + 4);
		registerInfo.scope = REGISTER_INFO::SCOPE_LOCAL;
		m_registerInfos.insert(std::make_pair(registerId, registerInfo));
	}
}

void CGlEsUmbralEffectGenerator::ParseVertexInputs(const CD3DShader& inputShader)
{
	//Check all DCL
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DCL) continue;
		assert(instruction.token.size == 2);
		auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
		if(dstParam.GetRegisterType() != CD3DShader::SHADER_REGISTER_INPUT) continue;
		auto usage = static_cast<CD3DShader::SHADER_DECLUSAGE_TYPE>(instruction.additionalTokens[0] & 0x1F);
		auto usageIndex = (instruction.additionalTokens[0] >> 16) & 0x0F;
		
		REGISTER_INFO info;
		info.usage = usage;
		info.usageIndex = usageIndex;
		
		auto registerNumber = dstParam.registerNumber;
		auto registerId = std::make_pair(CD3DShader::SHADER_REGISTER_INPUT, registerNumber);
		m_registerInfos.insert(std::make_pair(registerId, info));
	}
}

void CGlEsUmbralEffectGenerator::ParseOutputs(const CD3DShader& inputShader)
{
	for(const auto& instruction : inputShader.GetInstructions())
	{
		if(instruction.token.opcode != CD3DShader::OPCODE_DCL) continue;
		assert(instruction.token.size == 2);
		auto dstParam = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&instruction.additionalTokens[1]);
		if(dstParam.GetRegisterType() != CD3DShader::SHADER_REGISTER_OUTPUT) continue;
		auto usage = static_cast<CD3DShader::SHADER_DECLUSAGE_TYPE>(instruction.additionalTokens[0] & 0x1F);
		auto usageIndex = (instruction.additionalTokens[0] >> 16) & 0x0F;
		REGISTER_INFO registerInfo;
		registerInfo.usage = usage;
		registerInfo.usageIndex = usageIndex;
		auto registerNumber = dstParam.registerNumber;
		auto registerId = std::make_pair(dstParam.GetRegisterType(), registerNumber);
		m_registerInfos.insert(std::make_pair(registerId, registerInfo));
	}
}

CGlEsUmbralEffectGenerator::REGISTER_INFO CGlEsUmbralEffectGenerator::GetRegisterInfo(CD3DShader::SHADER_REGISTER_TYPE registerType, uint32 registerNumber) const
{
	auto registerId = std::make_pair(registerType, registerNumber);
	auto registerInfoIterator = m_registerInfos.find(registerId);
	
	if(registerInfoIterator != std::end(m_registerInfos))
	{
		return registerInfoIterator->second;
	}
	else
	{
		assert(0);
		return REGISTER_INFO();
	}
}

std::string CGlEsUmbralEffectGenerator::GetRegisterName(CD3DShader::SHADER_REGISTER_TYPE registerType, uint32 registerNumber) const
{
	auto registerId = std::make_pair(registerType, registerNumber);
	auto registerNameIterator = m_registerNames.find(registerId);
	
	if(registerNameIterator != std::end(m_registerNames))
	{
		return registerNameIterator->second;
	}
	else
	{
		assert(0);
		return "";
	}
}

////////////////////////////////////////////////////////////////////////
//IDENTATION_STATE
////////////////////////////////////////////////////////////////////////

CGlEsUmbralEffectGenerator::IDENTATION_STATE::IDENTATION_STATE()
{
	SetLevel(m_level);
}

void CGlEsUmbralEffectGenerator::IDENTATION_STATE::SetLevel(unsigned int level)
{
	std::string result;
	for(unsigned int i = 0; i <= level; i++)
	{
		result += "\t";
	}
	m_string = result;
	m_level = level;
}

void CGlEsUmbralEffectGenerator::IDENTATION_STATE::Increase()
{
	SetLevel(m_level + 1);
}

void CGlEsUmbralEffectGenerator::IDENTATION_STATE::Decrease()
{
	assert(m_level != 0);
	SetLevel(m_level - 1);
}

const std::string& CGlEsUmbralEffectGenerator::IDENTATION_STATE::GetString() const
{
	return m_string;
}

////////////////////////////////////////////////////////////////////////
//Emitters
////////////////////////////////////////////////////////////////////////

std::string CGlEsUmbralEffectGenerator::PrintSourceOperand(const CD3DShader::SOURCE_PARAMETER& srcParam, uint32 writeMask) const
{
	static const char* g_element[4] =
	{
		"x",
		"y",
		"z",
		"w"
	};
	auto regInfo = GetRegisterInfo(srcParam.parameter.GetRegisterType(), srcParam.parameter.registerNumber);
	auto regString = GetRegisterName(srcParam.parameter.GetRegisterType(), srcParam.parameter.registerNumber);
	auto regSubscript = regInfo.isArray ? string_format("[%d]", regInfo.subscript) : "";
	if(srcParam.parameter.useRelativeAddressing)
	{
		assert(regInfo.isArray);
		auto addrString = GetRegisterName(srcParam.relativeAddress.GetRegisterType(), srcParam.relativeAddress.registerNumber);
		if(srcParam.relativeAddress.GetRegisterType() != CD3DShader::SHADER_REGISTER_LOOP)
		{
			addrString += ".";
			addrString += g_element[srcParam.relativeAddress.swizzleX];			
		}
		regSubscript = string_format("[%s + %d]", addrString.c_str(), regInfo.subscript);
	}
	regString += regSubscript;
	switch(srcParam.parameter.sourceModifier)
	{
		case CD3DShader::SOURCE_MODIFIER_NONE:
			break;
		case CD3DShader::SOURCE_MODIFIER_NEGATE:
			regString = string_format("-%s", regString.c_str());
			break;
		case CD3DShader::SOURCE_MODIFIER_ABSOLUTE:
			regString = string_format("abs(%s)", regString.c_str());
			break;
		case CD3DShader::SOURCE_MODIFIER_ABSOLUTE_NEGATE:
			regString = string_format("-abs(%s)", regString.c_str());
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

std::string CGlEsUmbralEffectGenerator::PrintDestinationOperand(const CD3DShader::DESTINATION_PARAMETER& dstParam) const
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
	auto regString = GetRegisterName(dstParam.parameter.GetRegisterType(), dstParam.parameter.registerNumber);
	if(dstParam.parameter.GetRegisterType() != CD3DShader::SHADER_REGISTER_DEPTHOUT)
	{
		regString += ".";
		regString += g_writeMask[dstParam.parameter.writeMask];
	}
	return regString;
}

std::string CGlEsUmbralEffectGenerator::MakeSaturationOperation(const std::string& identationString, const std::string& dstString)
{
	return string_format("%s%s = clamp(%s, 0.0, 1.0);\r\n", identationString.c_str(), dstString.c_str(), dstString.c_str());
}

std::string CGlEsUmbralEffectGenerator::Emit_Nop(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	return std::string();
}

std::string CGlEsUmbralEffectGenerator::Emit_Mad(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	std::string result;
	
	auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
	auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src3Param = CD3DShader::ReadSourceParameter(tokenStream);
	
	auto dstString = PrintDestinationOperand(dstParam);
	auto src1String = PrintSourceOperand(src1Param, dstParam.parameter.writeMask);
	auto src2String = PrintSourceOperand(src2Param, dstParam.parameter.writeMask);
	auto src3String = PrintSourceOperand(src3Param, dstParam.parameter.writeMask);
	
	result += string_format("%s%s = %s * %s + %s;\r\n", identationState.GetString().c_str(),
							dstString.c_str(), src1String.c_str(), src2String.c_str(), src3String.c_str());
	if(dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_SATURATE)
	{
		result += MakeSaturationOperation(identationState.GetString(), dstString);
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Dp3(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	std::string result;
	
	auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
	auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
	
	auto dstString = PrintDestinationOperand(dstParam);
	auto src1String = PrintSourceOperand(src1Param, 0x07);
	auto src2String = PrintSourceOperand(src2Param, 0x07);
	
	result += string_format("%s%s = dot(%s, %s);\r\n", identationState.GetString().c_str(),
							dstString.c_str(), src1String.c_str(), src2String.c_str());
	if(dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_SATURATE)
	{
		result += MakeSaturationOperation(identationState.GetString(), dstString);
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Dp4(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	std::string result;

	auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
	auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
	
	auto dstString = PrintDestinationOperand(dstParam);
	auto src1String = PrintSourceOperand(src1Param, 0x0F);
	auto src2String = PrintSourceOperand(src2Param, 0x0F);
	
	assert((dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_SATURATE) == 0);
	
	result += string_format("%s%s = dot(%s, %s);\r\n", identationState.GetString().c_str(),
							dstString.c_str(), src1String.c_str(), src2String.c_str());
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Lrp(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	std::string result;
	
	auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
	auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src3Param = CD3DShader::ReadSourceParameter(tokenStream);
	
	auto dstString = PrintDestinationOperand(dstParam);
	auto src1String = PrintSourceOperand(src1Param, dstParam.parameter.writeMask);
	auto src2String = PrintSourceOperand(src2Param, dstParam.parameter.writeMask);
	auto src3String = PrintSourceOperand(src3Param, dstParam.parameter.writeMask);
	
	result += string_format("%s%s = mix(%s, %s, %s);\r\n", identationState.GetString().c_str(),
							dstString.c_str(), src3String.c_str(), src2String.c_str(), src1String.c_str());
	if(dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_SATURATE)
	{
		result += MakeSaturationOperation(identationState.GetString(), dstString);
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Loop(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	std::string result;
	
	auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
	assert(src1Param.parameter.GetRegisterType() == CD3DShader::SHADER_REGISTER_LOOP);
	assert(src2Param.parameter.GetRegisterType() == CD3DShader::SHADER_REGISTER_CONSTINT);
	
	auto counterRegisterInfo = GetRegisterInfo(src2Param.parameter.GetRegisterType(), src2Param.parameter.registerNumber);
	
	result += string_format("%sfor(int loopCounter = %d; loopCounter < %d; loopCounter += %d)\r\n", identationState.GetString().c_str(),
							counterRegisterInfo.constantValue[1], counterRegisterInfo.constantValue[0] + counterRegisterInfo.constantValue[1], counterRegisterInfo.constantValue[2]);
	result += string_format("%s{\r\n", identationState.GetString().c_str());
	
	identationState.Increase();
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Rep(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	std::string result;
		
	auto srcParam = CD3DShader::ReadSourceParameter(tokenStream);
	assert(srcParam.parameter.GetRegisterType() == CD3DShader::SHADER_REGISTER_CONSTINT);
	
	auto counterRegisterInfo = GetRegisterInfo(srcParam.parameter.GetRegisterType(), srcParam.parameter.registerNumber);
	
	result += string_format("%sfor(int repCounter = 0; repCounter < %d; repCounter++)\r\n", identationState.GetString().c_str(), counterRegisterInfo.constantValue[0]);
	result += string_format("%s{\r\n", identationState.GetString().c_str());
	
	identationState.Increase();
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_If(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	std::string result;
	
	auto srcParam = CD3DShader::ReadSourceParameter(tokenStream);
	
	auto srcParamString = PrintSourceOperand(srcParam, 0);

	result += string_format("%sif(%s)\r\n", identationState.GetString().c_str(), srcParamString.c_str());
	result += string_format("%s{\r\n", identationState.GetString().c_str());
	
	identationState.Increase();
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Cmp(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	std::string result;
	
	auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
	auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src3Param = CD3DShader::ReadSourceParameter(tokenStream);
	
	auto dstString = PrintDestinationOperand(dstParam);
	auto src1String = PrintSourceOperand(src1Param, dstParam.parameter.writeMask);
	auto src2String = PrintSourceOperand(src2Param, dstParam.parameter.writeMask);
	auto src3String = PrintSourceOperand(src3Param, dstParam.parameter.writeMask);
	
	assert((dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_SATURATE) == 0);
	
	unsigned int compCount = 0;
	for(unsigned int i = 0; i < 4; i++)
	{
		if(dstParam.parameter.writeMask & (1 << i))
		{
			compCount++;
		}
	}
	
	//TODO: Make this work properly. On GLES 3.0 there's a nice mix(vec, vec, bvec) overload which should
	//work better than this without propagating NaNs everywhere
	std::string comparer, converter;
	switch(compCount)
	{
		case 1:
			comparer = string_format("(%s >= 0.0)", src1String.c_str());
			converter = "float";
			break;
		case 2:
			comparer = string_format("greaterThanEqual(%s, vec2(0.0))", src1String.c_str());
			converter = "vec2";
			break;
		case 3:
			comparer = string_format("greaterThanEqual(%s, vec3(0.0))", src1String.c_str());
			converter = "vec3";
			break;
		case 4:
			comparer = string_format("greaterThanEqual(%s, vec4(0.0))", src1String.c_str());
			converter = "vec4";
			break;
		default:
			assert(0);
			break;
	}
		
	result += string_format("%s%s = mix(%s, %s, %s(%s));\r\n", identationState.GetString().c_str(),
							dstString.c_str(), src3String.c_str(), src2String.c_str(), converter.c_str(), comparer.c_str());
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Texld(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	std::string result;
	
	auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
	auto locationParam = CD3DShader::ReadSourceParameter(tokenStream);
	auto samplerParam = CD3DShader::ReadSourceParameter(tokenStream);
	
	assert(samplerParam.parameter.GetRegisterType() == CD3DShader::SHADER_REGISTER_SAMPLER);
	
	auto dstString = PrintDestinationOperand(dstParam);
	auto samplerRegisterName = GetRegisterName(CD3DShader::SHADER_REGISTER_SAMPLER, samplerParam.parameter.registerNumber);
	auto locationVariable = PrintSourceOperand(locationParam, 0x03);
	
	result += string_format("%s%s = texture2D(%s, %s);\r\n", identationState.GetString().c_str(),
							dstString.c_str(), samplerRegisterName.c_str(), locationVariable.c_str());
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Texldl(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{	
	std::string result;
	
	auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
	auto locationParam = CD3DShader::ReadSourceParameter(tokenStream);
	auto samplerParam = CD3DShader::ReadSourceParameter(tokenStream);
	
	assert(samplerParam.parameter.GetRegisterType() == CD3DShader::SHADER_REGISTER_SAMPLER);
	
	auto dstString = PrintDestinationOperand(dstParam);
	auto samplerRegisterName = GetRegisterName(CD3DShader::SHADER_REGISTER_SAMPLER, samplerParam.parameter.registerNumber);
	auto locationVariable = PrintSourceOperand(locationParam, 0x07);
	
	result += string_format("%s%s = textureCube(%s, %s);\r\n", identationState.GetString().c_str(),
							dstString.c_str(), samplerRegisterName.c_str(), locationVariable.c_str());
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Unary(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState, const char* format) const
{
	auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
	auto srcParam = CD3DShader::ReadSourceParameter(tokenStream);
	
	auto dstString = PrintDestinationOperand(dstParam);
	auto srcString = PrintSourceOperand(srcParam, dstParam.parameter.writeMask);
	
	auto result = string_format(format, identationState.GetString().c_str(), dstString.c_str(), srcString.c_str());
	if(dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_SATURATE)
	{
		result += MakeSaturationOperation(identationState.GetString(), dstString);
	}
	
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_Binary(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState, const char* format) const
{
	auto dstParam = CD3DShader::ReadDestinationParameter(tokenStream);
	auto src1Param = CD3DShader::ReadSourceParameter(tokenStream);
	auto src2Param = CD3DShader::ReadSourceParameter(tokenStream);
	
	auto dstString = PrintDestinationOperand(dstParam);
	auto src1String = PrintSourceOperand(src1Param, dstParam.parameter.writeMask);
	auto src2String = PrintSourceOperand(src2Param, dstParam.parameter.writeMask);
	
	auto result = string_format(format, identationState.GetString().c_str(), dstString.c_str(), src1String.c_str(), src2String.c_str());
	if(dstParam.parameter.resultModifier & CD3DShader::RESULT_MODIFIER_SATURATE)
	{
		result += MakeSaturationOperation(identationState.GetString(), dstString);
	}
	return result;
}

std::string CGlEsUmbralEffectGenerator::Emit_EndScope(CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE& identationState) const
{
	identationState.Decrease();
	return string_format("%s}\r\n", identationState.GetString().c_str());
}
