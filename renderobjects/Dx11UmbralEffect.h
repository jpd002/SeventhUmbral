#pragma once

#include "palleon/win32/Dx11Effect.h"
#include "D3DShader.h"

class CDx11UmbralEffect : public Palleon::CDx11Effect
{
public:
									CDx11UmbralEffect(ID3D11Device*, ID3D11DeviceContext*, const CD3DShader&, const CD3DShader&);
	virtual							~CDx11UmbralEffect();

	virtual void					UpdateConstants(const Palleon::MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;

private:
	virtual D3D11InputLayoutPtr		CreateInputLayout(const Palleon::VERTEX_BUFFER_DESCRIPTOR&) override;

	void							ParseVertexShaderConstantTable(OffsetKeeper&, const CD3DShaderConstantTable&, CD3DShaderConstantTable::REGISTER_SET);
	void							ParsePixelShaderConstantTable(OffsetKeeper&, const CD3DShaderConstantTable&, CD3DShaderConstantTable::REGISTER_SET);

	CD3DShader						m_vertexShader;
	CD3DShader						m_pixelShader;

	//Vertex Shader Constants
	uint32							m_modelBBoxOffsetOffset = -1;
	uint32							m_modelBBoxScaleOffset = -1;

	uint32							m_vertexOcclusionScaleOffset = -1;
	uint32							m_vertexColorBiasOffset = -1;

	uint32							m_pointLightColorsOffset = -1;
	uint32							m_pointLightParamsOffset = -1;
	uint32							m_pointLightPositionsOffset = -1;

	uint32							m_viewITMatrixOffset = -1;
	uint32							m_worldITMatrixOffset = -1;
	uint32							m_worldMatrixOffset = -1;
	uint32							m_worldViewMatrixOffset = -1;
	uint32							m_worldViewProjMatrixOffset = -1;
	uint32							m_isUseInstancingOffset = -1;

	//Pixel Shader Constants
	uint32							m_pixelClippingDistanceOffset = -1;

	uint32							m_modulateColorOffset = -1;
	uint32							m_ambientColorOffset = -1;
	uint32							m_diffuseColorOffset = -1;
	uint32							m_specularColorOffset = -1;
	uint32							m_shininessOffset = -1;
	uint32							m_reflectivityOffset = -1;
	uint32							m_normalPowerOffset = -1;

	uint32							m_multiDiffuseColorOffset = -1;
	uint32							m_multiSpecularColorOffset = -1;
	uint32							m_multiShininessOffset = -1;
	uint32							m_multiReflectivityOffset = -1;
	uint32							m_multiNormalPowerOffset = -1;

	uint32							m_fresnelExpOffset = -1;
	uint32							m_fresnelLightDiffBiasOffset = -1;
	uint32							m_specularInfluenceOffset = -1;
	uint32							m_lightDiffusePowerOffset = -1;
	uint32							m_lightDiffuseInfluenceOffset = -1;
	uint32							m_lightDiffuseMapLodOffset = -1;
	uint32							m_reflectMapInfluenceOffset = -1;
	uint32							m_reflectMapLodOffset = -1;

	uint32							m_glareLdrScaleOffset = -1;
	uint32							m_refAlphaRestrainOffset = -1;
	uint32							m_normalVectorOffset = -1;
	uint32							m_depthBiasOffset = -1;
	uint32							m_velvetParamOffset = -1;

	uint32							m_ambientOcclusionColorOffset = -1;
	uint32							m_specularOcclusionColorOffset = -1;
	uint32							m_pointLightOcclusionColorOffset = -1;
	uint32							m_mainLightOcclusionColorOffset = -1;
	uint32							m_subLightOcclusionColorOffset = -1;
	uint32							m_lightMapOcclusionColorOffset = -1;
	uint32							m_reflectMapOcclusionColorOffset = -1;

	uint32							m_enableShadowFlagOffset = -1;

	uint32							m_latitudeParamOffset = -1;
	uint32							m_ambientLightColorOffset = -1;
	uint32							m_dirLightDirectionsOffset = -1;
	uint32							m_dirLightColorsOffset = -1;
};
