#pragma once

#include "BaseChunk.h"

class CPramChunk : public CBaseChunk
{
public:
	typedef std::vector<std::string> StringTable;

	struct SAMPLER
	{
		std::string		name;
		StringTable		strings;
	};
	typedef std::vector<SAMPLER> SamplerArray;

							CPramChunk();
	virtual					~CPramChunk();

	virtual void			Read(Framework::CStream&) override;

	const SamplerArray&		GetSamplers() const;

private:
	typedef std::vector<uint32> OffsetArray;

	OffsetArray				ReadOffsets(Framework::CStream&, uint32);

	uint32					m_numParameters;
	uint32					m_parameterNameStart;
	uint32					m_numSamplers;
	uint32					m_samplerDataStart;
	uint32					m_samplerNameStart;

	SamplerArray			m_samplers;
};

typedef std::shared_ptr<CPramChunk> PramChunkPtr;
