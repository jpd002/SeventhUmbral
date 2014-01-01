#pragma once

#include <string>
#include "D3DShader.h"

class CD3DShaderDisassembler
{
public:
	static std::string			GetInstructionMnemonic(CD3DShader::SHADER_TYPE, const CD3DShader::INSTRUCTION&);
	static std::string			GetInstructionOperands(CD3DShader::SHADER_TYPE, const CD3DShader::INSTRUCTION&);

private:
	static std::string			PrintSourceParameterToken(const CD3DShader::SOURCE_PARAMETER&, uint32);
	static std::string			PrintDestinationParameterToken(const CD3DShader::DESTINATION_PARAMETER&);
	static std::string			PrintParameterRegister(CD3DShader::SHADER_REGISTER_TYPE, uint32);
	static std::string			PrintDeclUsage(CD3DShader::SHADER_DECLUSAGE_TYPE, uint32);
};
