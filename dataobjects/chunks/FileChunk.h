#pragma once

#include "BaseChunk.h"

class CFileChunk : public CBaseChunk
{
public:
					CFileChunk();
	virtual			~CFileChunk() override;

	virtual void	Read(Framework::CStream&) override;

	std::string		GetName() const;

	const uint8*	GetCompiledShader() const;
	uint32			GetCompiledShaderLength() const;

private:
	uint32			m_blockSize;
	uint32			m_stringOffset;
	uint32			m_shaderOffset;
	std::string		m_name;

	uint32			m_compiledShaderLength;
	uint8*			m_compiledShader;
};

typedef std::shared_ptr<CFileChunk> FileChunkPtr;
