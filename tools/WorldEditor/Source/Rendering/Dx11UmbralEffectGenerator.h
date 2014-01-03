#pragma once

#include <string>
#include <map>
#include "D3DShader.h"
#include "D3DShaderConstantTable.h"

class CDx11UmbralEffectGenerator
{
public:
	static std::string		GenerateVertexShader(const CD3DShader&);
	static std::string		GeneratePixelShader(const CD3DShader&, const CD3DShader&);

private:
	struct VARIABLE_INFO
	{
		VARIABLE_INFO()
			: VARIABLE_INFO("")
		{
		}

		explicit VARIABLE_INFO(const std::string& name)
			: name(name)
		{
			memset(&constantValue, 0, sizeof(constantValue));
		}

		std::string		name;
		bool			isArray = false;
		uint32			subscript = 0;
		uint32			constantValue[4];
	};

	typedef std::map<uint32, VARIABLE_INFO> RegisterIndexMap;
	typedef std::pair<std::string, RegisterIndexMap> StructureDef;

	std::string				GenerateVertexShaderInternal(const CD3DShader&);
	std::string				GeneratePixelShaderInternal(const CD3DShader&, const CD3DShader&);

	std::string				GenerateInstructions(const CD3DShader&) const;

	static StructureDef		GenerateConstants(const CD3DShaderConstantTable&);
	static StructureDef		GenerateBoolConstants(const CD3DShaderConstantTable&);
	static StructureDef		GenerateSamplers(const CD3DShaderConstantTable&);
	static StructureDef		GeneratePrivateConstants(const CD3DShader&);
	static StructureDef		GeneratePrivateIntConstants(const CD3DShader&);
	static StructureDef		GenerateTemporaries(const CD3DShader&);
	static StructureDef		GenerateAddressTemporaries(const CD3DShader&);
	static StructureDef		GenerateVertexInputStructure(const CD3DShader&);
	static StructureDef		GeneratePixelInputStructure(const CD3DShader&, const CD3DShader&);
	static StructureDef		GenerateOutputStructure(const CD3DShader&);

	std::string				PrintSourceOperand(const CD3DShader::SOURCE_PARAMETER&, uint32) const;
	std::string				PrintDestinationOperand(const CD3DShader::DESTINATION_PARAMETER&) const;
	VARIABLE_INFO			GetVariableForRegister(CD3DShader::SHADER_REGISTER_TYPE, uint32) const;
	static std::string		PrintDeclUsage(CD3DShader::SHADER_DECLUSAGE_TYPE, unsigned int);
	static std::string		PrintVarTypeFromWriteMask(uint32);

	RegisterIndexMap		m_constantRegisterMap;
	RegisterIndexMap		m_intConstantRegisterMap;
	RegisterIndexMap		m_boolConstantRegisterMap;
	RegisterIndexMap		m_samplerRegisterMap;
	RegisterIndexMap		m_temporaryRegisterMap;
	RegisterIndexMap		m_addressTemporaryRegisterMap;
	RegisterIndexMap		m_inputRegisterMap;
	RegisterIndexMap		m_outputRegisterMap;
};
