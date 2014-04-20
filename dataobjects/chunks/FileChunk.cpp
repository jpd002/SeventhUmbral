#include "FileChunk.h"

CFileChunk::CFileChunk()
: m_blockSize(0)
, m_stringOffset(0)
, m_shaderOffset(0)
, m_compiledShaderLength(0)
, m_compiledShader(nullptr)
{

}

CFileChunk::~CFileChunk()
{
	delete [] m_compiledShader;
}

void CFileChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);

	uint64 baseOffset = inputStream.Tell();

	m_blockSize = inputStream.Read32_MSBF();
	m_stringOffset = inputStream.Read32_MSBF();
	m_shaderOffset = inputStream.Read32_MSBF();
	m_name = inputStream.ReadString();

	inputStream.Seek(baseOffset + m_shaderOffset, Framework::STREAM_SEEK_SET);

	uint8 unknownData[0x10];
	inputStream.Read(unknownData, 0x10);

	m_compiledShaderLength = inputStream.Read32_MSBF();
	uint32 unknownWord = inputStream.Read32_MSBF();

	inputStream.Seek(baseOffset + m_shaderOffset + 0x20, Framework::STREAM_SEEK_SET);

	m_compiledShader = new uint8[m_compiledShaderLength];
	inputStream.Read(m_compiledShader, m_compiledShaderLength);
}

std::string CFileChunk::GetName() const
{
	return m_name;
}

const uint8* CFileChunk::GetCompiledShader() const
{
	return m_compiledShader;
}

uint32 CFileChunk::GetCompiledShaderLength() const
{
	return m_compiledShaderLength;
}
