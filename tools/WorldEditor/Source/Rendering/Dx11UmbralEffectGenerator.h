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
	std::string				GenerateVertexShaderInternal(const CD3DShader&);
	std::string				GeneratePixelShaderInternal(const CD3DShader&, const CD3DShader&);

	typedef std::map<uint32, std::string> RegisterIndexMap;
	typedef std::pair<std::string, RegisterIndexMap> StructureDef;

	std::string				GenerateInstructions(const CD3DShader&) const;

	static StructureDef		GenerateConstants(const CD3DShaderConstantTable&);
	static StructureDef		GeneratePrivateConstants(const CD3DShader&);
	static StructureDef		GenerateTemporaries(const CD3DShader&);
	static StructureDef		GenerateVertexInputStructure(const CD3DShader&);
	static StructureDef		GeneratePixelInputStructure(const CD3DShader&, const CD3DShader&);
	static StructureDef		GenerateOutputStructure(const CD3DShader&);

	std::string				PrintSourceOperand(const CD3DShader::SOURCE_PARAMETER_TOKEN&, uint32) const;
	std::string				PrintDestinationOperand(const CD3DShader::DESTINATION_PARAMETER_TOKEN&) const;
	std::string				PrintParameterRegister(CD3DShader::SHADER_REGISTER_TYPE, uint32) const;
	static std::string		PrintDeclUsage(CD3DShader::SHADER_DECLUSAGE_TYPE, unsigned int);
	static std::string		PrintVarTypeFromWriteMask(uint32);

	RegisterIndexMap		m_constantRegisterMap;
	RegisterIndexMap		m_temporaryRegisterMap;
	RegisterIndexMap		m_inputRegisterMap;
	RegisterIndexMap		m_outputRegisterMap;
};
