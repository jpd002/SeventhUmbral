#include <assert.h>
#include <vector>
#include "D3DShader.h"
#include "PtrStream.h"

CD3DShader::CD3DShader()
{

}

CD3DShader::CD3DShader(Framework::CStream& stream)
{
	Read(stream);
}

CD3DShader::~CD3DShader()
{

}

bool CD3DShader::operator ==(const CD3DShader& rhs) const
{
	if(m_type != rhs.m_type) return false;
	if(m_comments != rhs.m_comments) return false;
	if(m_instructions != rhs.m_instructions) return false;
	return true;
}

CD3DShader::SHADER_TYPE CD3DShader::GetType() const
{
	return m_type;
}

const CD3DShader::InstructionArray& CD3DShader::GetInstructions() const
{
	return m_instructions;
}

const CD3DShader::CommentArray& CD3DShader::GetComments() const
{
	return m_comments;
}

CD3DShaderConstantTable CD3DShader::GetConstantTable() const
{
	for(const auto& comment : m_comments)
	{
		if(comment.size() >= 4 && 
			comment[0] == 'C' && comment[1] == 'T' &&
			comment[2] == 'A' && comment[3] == 'B')
		{
			Framework::CPtrStream tableStream(comment.data() + 4, comment.size() - 4);
			return CD3DShaderConstantTable(tableStream);
		}
	}
	
	return CD3DShaderConstantTable();
}

CD3DShader::DESTINATION_PARAMETER CD3DShader::ReadDestinationParameter(CTokenStream& stream)
{
	DESTINATION_PARAMETER result;
	memset(&result, 0, sizeof(result));
	{
		uint32 dstToken = stream.ReadToken();
		result.parameter = *reinterpret_cast<const CD3DShader::DESTINATION_PARAMETER_TOKEN*>(&dstToken);
	}
	if(result.parameter.useRelativeAddressing)
	{
		uint32 addrToken = stream.ReadToken();
		result.relativeAddress = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&addrToken);
		auto addressRegisterType = result.relativeAddress.GetRegisterType();
		assert(addressRegisterType == SHADER_REGISTER_TEXTURE || addressRegisterType == SHADER_REGISTER_LOOP);
		assert(result.relativeAddress.reserved2 == 1);
	}
	return result;
}

CD3DShader::SOURCE_PARAMETER CD3DShader::ReadSourceParameter(CTokenStream& stream)
{
	SOURCE_PARAMETER result;
	memset(&result, 0, sizeof(result));
	{
		uint32 srcToken = stream.ReadToken();
		result.parameter = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&srcToken);
	}
	if(result.parameter.useRelativeAddressing)
	{
		uint32 addrToken = stream.ReadToken();
		result.relativeAddress = *reinterpret_cast<const CD3DShader::SOURCE_PARAMETER_TOKEN*>(&addrToken);
		auto addressRegisterType = result.relativeAddress.GetRegisterType();
		assert(addressRegisterType == SHADER_REGISTER_TEXTURE || addressRegisterType == SHADER_REGISTER_LOOP);
		assert(result.relativeAddress.reserved2 == 1);
	}
	return result;
}

void CD3DShader::Read(Framework::CStream& stream)
{
	uint32 shaderInfo = stream.Read32();
	m_type = static_cast<SHADER_TYPE>(shaderInfo >> 16);
	uint16 shaderVersion = static_cast<uint16>(shaderInfo & 0xFFFF);
	assert(m_type == SHADER_TYPE_VERTEX || m_type == SHADER_TYPE_PIXEL);
	assert(shaderVersion == 0x0300);	//Shader Model 3.0

	while(1)
	{
		uint32 token = stream.Read32();
		if(token == OPCODE_END) break;
		if(stream.IsEOF()) break;
		switch(token & 0xFFFF)
		{
		case OPCODE_COMMENT:
			{
				Comment comment;
				uint32 commentSize = (token >> 16) & 0x7FFF;
				comment.resize(commentSize * 4);
				stream.Read(comment.data(), commentSize * 4);
				m_comments.push_back(std::move(comment));
			}
			break;
		default:
			{
				INSTRUCTION instruction;
				instruction.token = *reinterpret_cast<INSTRUCTION_TOKEN*>(&token);
				instruction.additionalTokens.resize(instruction.token.size);
				stream.Read(instruction.additionalTokens.data(), instruction.token.size * 4);
				m_instructions.push_back(std::move(instruction));
			}
			break;
		}
	}
}

CD3DShader::CTokenStream::CTokenStream(const TokenArray& tokens)
: m_tokens(tokens)
{

}

CD3DShader::CTokenStream::~CTokenStream()
{

}

uint32 CD3DShader::CTokenStream::ReadToken()
{
	if(m_position >= m_tokens.size())
	{
		throw std::runtime_error("No more tokens to read.");
	}
	return m_tokens[m_position++];
}
