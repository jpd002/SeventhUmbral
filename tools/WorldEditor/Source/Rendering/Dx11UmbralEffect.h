#pragma once

#include "athena/win32/Dx11Effect.h"
#include "D3DShader.h"

class CDx11UmbralEffect : public Athena::CDx11Effect
{
public:
					CDx11UmbralEffect(ID3D11Device*, ID3D11DeviceContext*, const CD3DShader&, const CD3DShader&);
	virtual			~CDx11UmbralEffect();

	virtual void	UpdateConstants(const Athena::MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;

private:
	CD3DShader		m_vertexShader;
	CD3DShader		m_pixelShader;

	void			ParseVertexShaderConstantTable(OffsetKeeper&, const CD3DShaderConstantTable&, CD3DShaderConstantTable::REGISTER_SET);

	uint32			m_modelBBoxOffsetOffset = -1;
	uint32			m_modelBBoxScaleOffset = -1;
	uint32			m_worldMatrixOffset = -1;
	uint32			m_worldViewProjMatrixOffset = -1;
};
