#pragma once

#include "BaseChunk.h"

class CPramChunk : public CBaseChunk
{
public:
	typedef std::vector<std::string> StringTable;

	struct PARAMETER
	{
		std::string		name;
		bool			isPixelShaderParam = false;
		uint8			unknown2 = 0;
		uint8			numValues = 0;
		uint8			unknown3 = 0;
		uint32			unknown4 = 0;
		uint32			unknown5 = 0;
		uint32			unknown6 = 0;
		float			valueX = 0;
		float			valueY = 0;
		float			valueZ = 0;
		float			valueW = 0;
	};
	typedef std::vector<PARAMETER> ParameterArray;

	struct SAMPLER
	{
		std::string		name;
		StringTable		strings;
	};
	typedef std::vector<SAMPLER> SamplerArray;

							CPramChunk();
	virtual					~CPramChunk();

	virtual void			Read(Framework::CStream&) override;

	int16					GetRenderMode() const;
	const ParameterArray&	GetParameters() const;
	const SamplerArray&		GetSamplers() const;

private:
	typedef std::vector<uint32> OffsetArray;

	OffsetArray				ReadOffsets(Framework::CStream&, uint32);
	void					ReadParameters(Framework::CStream&, uint32, const OffsetArray&, const OffsetArray&);
	void					ReadSamplers(Framework::CStream&, uint32, const OffsetArray&, const OffsetArray&);

	uint32					m_numParameters;
	uint32					m_parameterNameStart;
	uint32					m_numSamplers;
	uint32					m_samplerDataStart;
	uint32					m_samplerNameStart;
	int16					m_renderMode;

	ParameterArray			m_parameters;
	SamplerArray			m_samplers;
};

typedef std::shared_ptr<CPramChunk> PramChunkPtr;
