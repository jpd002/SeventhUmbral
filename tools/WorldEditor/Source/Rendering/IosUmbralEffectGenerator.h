#pragma once

#include <string>
#include <map>
#include "D3DShader.h"
#include "D3DShaderConstantTable.h"

class CIosUmbralEffectGenerator
{
public:
	static std::string		GenerateVertexShader(const CD3DShader&);
	static std::string		GeneratePixelShader(const CD3DShader&);

private:
	typedef std::pair<CD3DShader::SHADER_REGISTER_TYPE, uint32> RegisterId;
	
	struct REGISTER_INFO
	{
		REGISTER_INFO()
		{
			memset(&constantValue, 0, sizeof(constantValue));
		}
		
		enum SCOPE
		{
			SCOPE_GLOBAL,
			SCOPE_LOCAL
		};
		
		std::string		name;
		bool			isArray = false;
		bool			isMatrix = false;
		uint32			arraySize = 0;
		uint32			subscript = 0;
		uint32			constantValue[4];
		uint8			usage = 0;
		uint8			usageIndex = 0;
		SCOPE			scope = SCOPE_LOCAL;
	};
	typedef std::map<RegisterId, REGISTER_INFO> RegisterInfoMap;
	typedef std::map<RegisterId, std::string> RegisterNameMap;
	
	struct IDENTATION_STATE
	{
	public:
								IDENTATION_STATE();
		
		void					SetLevel(unsigned int);
		void					Increase();
		void					Decrease();
		const std::string&		GetString() const;
		
	private:
		std::string				m_string;
		unsigned int			m_level = 0;
	};
	
	typedef std::function<std::string (CD3DShader::CTokenStream& tokenStream, IDENTATION_STATE&)> EmitInstructionFunction;
	
							CIosUmbralEffectGenerator();
		
	std::string				GenerateVertexShaderInternal(const CD3DShader&);
	std::string				GeneratePixelShaderInternal(const CD3DShader&);
	
	void					ParseTemporaries(const CD3DShader&);
	void					ParseAddressTemporaries(const CD3DShader&);
	void					ParseVertexInputs(const CD3DShader&);
	void					ParseOutputs(const CD3DShader&);	
	void					ParseGlobalConstants(const CD3DShaderConstantTable&);
	void					ParseGlobalBoolConstants(const CD3DShaderConstantTable&);
	void					ParseGlobalSamplerConstants(const CD3DShaderConstantTable&);
	void					ParseLocalConstants(const CD3DShader&);
	void					ParseLocalIntConstants(const CD3DShader&);
	
	std::string				GenerateInstructions(const CD3DShader&) const;
	std::string				GenerateAttributeDeclarations();
	std::string				GenerateVaryingDeclarations();
	std::string				GeneratePixelVaryingDeclarations();
	std::string				GenerateUniformDeclarations(const std::string&);
	std::string				GenerateTemporaryDeclarations();
	std::string				GenerateLocalConstantDeclarations();
	
	REGISTER_INFO			GetRegisterInfo(CD3DShader::SHADER_REGISTER_TYPE, uint32) const;
	std::string				GetRegisterName(CD3DShader::SHADER_REGISTER_TYPE, uint32) const;
	
	std::string				PrintSourceOperand(const CD3DShader::SOURCE_PARAMETER&, uint32) const;
	std::string				PrintDestinationOperand(const CD3DShader::DESTINATION_PARAMETER&) const;
	static std::string		MakeSaturationOperation(const std::string&, const std::string&);
	
	std::string				Emit_Nop(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_Mad(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_Dp3(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_Dp4(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_Lrp(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_Loop(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_Rep(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_If(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_Cmp(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_Texld(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	std::string				Emit_Texldl(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	
	std::string				Emit_Unary(CD3DShader::CTokenStream&, IDENTATION_STATE&, const char*) const;
	std::string				Emit_Binary(CD3DShader::CTokenStream&, IDENTATION_STATE&, const char*) const;
	std::string				Emit_EndScope(CD3DShader::CTokenStream&, IDENTATION_STATE&) const;
	
	RegisterInfoMap			m_registerInfos;
	RegisterNameMap			m_registerNames;
	
	EmitInstructionFunction	m_instructionEmitters[0x80];
};
