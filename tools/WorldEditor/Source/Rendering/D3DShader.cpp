#include <assert.h>
#include <vector>
#include "D3DShader.h"
#include "PtrStream.h"

CD3DShader::CD3DShader(Framework::CStream& stream)
: m_type(SHADER_TYPE_INVALID)
{
	Read(stream);
}

CD3DShader::~CD3DShader()
{

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
