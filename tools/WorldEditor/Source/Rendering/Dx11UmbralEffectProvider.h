#pragma once

#include "AthenaEngine.h"
#include "D3DShader.h"
#include "Dx11UmbralEffect.h"
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

class CDx11UmbralEffectProvider : public Athena::CEffectProvider
{
public:
								CDx11UmbralEffectProvider(ID3D11Device*, ID3D11DeviceContext*);
	virtual						~CDx11UmbralEffectProvider();

	Athena::EffectPtr			GetEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader);

	virtual Athena::EffectPtr	GetEffectForRenderable(Athena::CMesh*, bool) override;

private:
	typedef std::unordered_map<EFFECTINFO, Athena::EffectPtr> EffectMap;

	ID3D11Device*				m_device;
	ID3D11DeviceContext*		m_deviceContext;
	EffectMap					m_effects;
};
