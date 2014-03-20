#pragma once

#include "athena/ios/IosEffect.h"
#include "D3DShader.h"

class CIosUmbralEffect : public Athena::CIosEffect
{
public:
									CIosUmbralEffect(const CD3DShader&, const CD3DShader&);
	virtual							~CIosUmbralEffect();

	virtual void					UpdateConstants(const Athena::MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;

private:
	CD3DShader						m_vertexShader;
	CD3DShader						m_pixelShader;
	
	GLuint							m_worldMatrixHandle = -1;
	GLuint							m_worldViewProjMatrixHandle = -1;
	GLuint							m_worldViewMatrixHandle = -1;
	GLuint							m_modelBBoxScaleHandle = -1;
	GLuint							m_modelBBoxOffsetHandle = -1;
	GLuint							m_isSkiningHandle = -1;
};
