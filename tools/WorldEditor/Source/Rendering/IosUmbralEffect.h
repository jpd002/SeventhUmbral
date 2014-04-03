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
	void							GetUniformLocations();
	
	CD3DShader						m_vertexShader;
	CD3DShader						m_pixelShader;
	
	GLuint							m_worldMatrixHandle = -1;
	GLuint							m_worldITMatrixHandle = -1;
	GLuint							m_viewITMatrixHandle = -1;
	GLuint							m_worldViewProjMatrixHandle = -1;
	GLuint							m_worldViewMatrixHandle = -1;
	GLuint							m_modelBBoxScaleHandle = -1;
	GLuint							m_modelBBoxOffsetHandle = -1;
	GLuint							m_isSkinningHandle = -1;
	GLuint							m_fogParamHandle = -1;
	
	GLuint							m_textureUnit0Handle = -1;
	GLuint							m_textureUnit1Handle = -1;
	GLuint							m_textureUnit2Handle = -1;
	GLuint							m_textureUnit3Handle = -1;
	GLuint							m_textureUnit4Handle = -1;
	GLuint							m_textureUnit5Handle = -1;
	
	//Per mesh pixel shader constants
	GLuint							m_modulateColorHandle = -1;
	GLuint							m_ambientColorHandle = -1;
	GLuint							m_diffuseColorHandle = -1;
	GLuint							m_specularColorHandle = -1;
	GLuint							m_shininessHandle = -1;
	GLuint							m_reflectivityHandle = -1;
	GLuint							m_normalPowerHandle = -1;

	GLuint							m_fresnelExpHandle = -1;
	GLuint							m_fresnelLightDiffBiasHandle = -1;
	GLuint							m_lightDiffusePowerHandle = -1;
	GLuint							m_specularInfluenceHandle = -1;
	GLuint							m_lightDiffuseInfluenceHandle = -1;
	GLuint							m_reflectMapInfluenceHandle = -1;
	
	GLuint							m_glareLdrScaleHandle = -1;
	GLuint							m_refAlphaRestrainHandle = -1;
	GLuint							m_velvetParamHandle = -1;
	
	//Per scene pixel shader constants
	GLuint							m_ambientLightColorHandle = -1;
	GLuint							m_dirLightDirectionsHandle = -1;
	GLuint							m_dirLightColorsHandle = -1;
	GLuint							m_latitudeParamHandle = -1;
	GLuint							m_enableShadowFlagHandle = -1;
};
