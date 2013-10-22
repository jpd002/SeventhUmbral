#pragma once

#include "BaseChunk.h"

class CFileChunk : public CBaseChunk
{
public:
					CFileChunk();
	virtual			~CFileChunk() override;

	virtual void	Read(Framework::CStream&) override;

private:
	uint32			m_blockSize;
	uint32			m_stringOffset;
	uint32			m_shaderOffset;
	std::string		m_name;

	uint32			m_compiledShaderLength;
	uint8*			m_compiledShader;
};
