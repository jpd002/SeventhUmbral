#include "PramChunk.h"
#include <assert.h>

CPramChunk::CPramChunk()
: m_numParameters(0)
, m_parameterNameStart(0)
, m_numSamplers(0)
, m_samplerDataStart(0)
, m_samplerNameStart(0)
{

}

CPramChunk::~CPramChunk()
{

}

void CPramChunk::Read(Framework::CStream& inputStream)
{
	CBaseChunk::Read(inputStream);

	auto basePosition = inputStream.Tell();

	{
		int16 unknown1 = inputStream.Read16();
		int16 unknown2 = inputStream.Read16();
		uint32 unknown3 = inputStream.Read32();
		m_numParameters = inputStream.Read32();
		uint32 unknown4 = inputStream.Read32();
		m_parameterNameStart = inputStream.Read32();
		m_numSamplers = inputStream.Read32();
		m_samplerDataStart = inputStream.Read32();
		m_samplerNameStart = inputStream.Read32();
	}

	auto parameterDataOffsets = ReadOffsets(inputStream, m_numParameters);
	auto parameterNameOffsets = ReadOffsets(inputStream, m_numParameters);
	auto samplerDataOffsets = ReadOffsets(inputStream, m_numSamplers);
	auto samplerNameOffsets = ReadOffsets(inputStream, m_numSamplers);

	m_samplers.resize(m_numSamplers);

	for(uint32 i = 0; i < m_numSamplers; i++)
	{
		const auto& samplerDataOffset = samplerDataOffsets[i];
		const auto& samplerNameOffset = samplerNameOffsets[i];
		auto& sampler = m_samplers[i];

		auto absDataOffset = basePosition + samplerDataOffset;
		auto absNameOffset = basePosition + samplerNameOffset;

		inputStream.Seek(absDataOffset, Framework::STREAM_SEEK_SET);

		uint16 magic = inputStream.Read16();
		assert(magic == 0x102);
		uint8 stringTableLength = inputStream.Read8();
		uint8 unknown1 = inputStream.Read8();
		uint32 unknown2 = inputStream.Read32();
		uint32 unknown3 = inputStream.Read32();
		uint32 unknown4 = inputStream.Read32();
		uint32 unknown5 = inputStream.Read32();
		uint32 unknown6 = inputStream.Read32();
		uint32 unknown7 = inputStream.Read32();
		uint32 unknown8 = inputStream.Read32();

		while(1)
		{
			auto stringItem = inputStream.ReadString();
			if(stringItem.empty()) break;
			sampler.strings.push_back(stringItem);
		}

		inputStream.Seek(absNameOffset, Framework::STREAM_SEEK_SET);
		sampler.name = inputStream.ReadString();
	}
}

const CPramChunk::SamplerArray& CPramChunk::GetSamplers() const
{
	return m_samplers;
}

CPramChunk::OffsetArray CPramChunk::ReadOffsets(Framework::CStream& inputStream, uint32 itemCount)
{
	OffsetArray result;
	result.resize(itemCount);
	inputStream.Read(result.data(), itemCount * sizeof(uint32));
	return result;
}
