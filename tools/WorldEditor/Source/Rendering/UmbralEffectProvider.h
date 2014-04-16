#pragma once

#include "PalleonEngine.h"
#include "D3DShader.h"
#include <unordered_map>

struct EFFECTINFO
{
	CD3DShader		vertexShader;
	CD3DShader		pixelShader;

	bool operator ==(const EFFECTINFO& rhs) const
	{
		return 
			(vertexShader == rhs.vertexShader) && 
			(pixelShader == rhs.pixelShader);
	}
};

namespace std
{
	template<>
	struct hash<EFFECTINFO>
	{
		size_t operator ()(const EFFECTINFO& effectInfo) const
		{
			uLong crc = crc32(0L, Z_NULL, 0);
			crc = HashShader(crc, effectInfo.vertexShader);
			crc = HashShader(crc, effectInfo.pixelShader);
			return crc;
		}

	private:
		static uLong HashShader(uLong crc, const CD3DShader& shader)
		{
			for(const auto& instruction : shader.GetInstructions())
			{
				crc = crc32(crc, reinterpret_cast<const Bytef*>(&instruction.token), sizeof(uint32));
				crc = crc32(crc, reinterpret_cast<const Bytef*>(instruction.additionalTokens.data()), sizeof(uint32) * instruction.additionalTokens.size());
			}
			for(const auto& comment : shader.GetComments())
			{
				crc = crc32(crc, reinterpret_cast<const Bytef*>(comment.data()), comment.size());
			}
			return crc;
		}
	};
}

class CUmbralEffectProvider : public Palleon::CEffectProvider
{
public:
	Palleon::EffectPtr			GetEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader);

	virtual Palleon::EffectPtr	GetEffectForRenderable(Palleon::CMesh*, bool) override;

protected:
	virtual Palleon::EffectPtr	CreateEffect(const CD3DShader&, const CD3DShader&) = 0;
	
private:
	typedef std::unordered_map<EFFECTINFO, Palleon::EffectPtr> EffectMap;

	EffectMap					m_effects;
};
