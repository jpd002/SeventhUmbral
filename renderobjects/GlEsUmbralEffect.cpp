#include "GlEsUmbralEffect.h"
#include "GlEsUmbralEffectGenerator.h"
#include "UmbralEffect.h"

CGlEsUmbralEffect::CGlEsUmbralEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader)
: m_vertexShader(vertexShader)
, m_pixelShader(pixelShader)
{
	auto vertexShaderText = CGlEsUmbralEffectGenerator::GenerateVertexShader(m_vertexShader);
	auto pixelShaderText = CGlEsUmbralEffectGenerator::GeneratePixelShader(m_pixelShader);
	
	AttributeBindingArray attributeBindings;
	attributeBindings.push_back(std::make_pair(Palleon::VERTEX_ITEM_ID_POSITION, "a_position0"));
	attributeBindings.push_back(std::make_pair(Palleon::VERTEX_ITEM_ID_UV0, "a_texcoord0"));
	attributeBindings.push_back(std::make_pair(Palleon::VERTEX_ITEM_ID_NORMAL, "a_normal0"));
	attributeBindings.push_back(std::make_pair(Palleon::VERTEX_ITEM_ID_COLOR, "a_color0"));
	attributeBindings.push_back(std::make_pair(CUmbralEffect::VERTEX_ITEM_ID_TANGENT, "a_texcoord5"));
	
	BuildProgram(vertexShaderText, pixelShaderText, attributeBindings);

	GetUniformLocations();
	CHECKGLERROR();
}

CGlEsUmbralEffect::~CGlEsUmbralEffect()
{

}
#if 0
class CMaterialParameterBase
{
public:
	virtual			~CMaterialParameterBase() {}
	virtual void	UpdateConstantValue(const Palleon::MaterialPtr&) = 0;
};

template<typename ParamType>
class CMaterialParameter : public CMaterialParameterBase
{
public:
	CMaterialParameter(const ParamType& defaultValue)
	: m_defaultValue(defaultValue)
	{
		
	}
	
	virtual void	UpdateConstantValue(const Palleon::MaterialPtr&) override;
	
private:
	
	ParamType		m_defaultValue;
};

template<>
void CMaterialParameter<CVector2>::UpdateConstantValue(const Palleon::MaterialPtr& material)
{
	auto effectParam = material->GetEffectParameter(paramName);
	assert((paramOffset == -1) || !effectParam.IsNull());
	if(effectParam.IsNull()) return m_defaultValue;
	return effectParam.GetVector2();
}

template<typename ParamType>
CMaterialParameterBase* MakeMaterialParameter(const ParamType& defaultValue)
{
	return new CMaterialParameter<ParamType>(defaultValue);
}

#endif

template <typename ParamType> 
void SetParamValue(uint32 paramHandle, const ParamType& value);

template <>
void SetParamValue<float>(uint32 paramHandle, const float& value)
{
	if(paramHandle != -1)
	{
		glUniform4f(paramHandle, value, 0, 0, 0);
	}
}

template <>
void SetParamValue<CVector2>(uint32 paramHandle, const CVector2& value)
{
	if(paramHandle != -1)
	{
		glUniform4f(paramHandle, value.x, value.y, 0, 0);
	}
}

template <>
void SetParamValue<CVector3>(uint32 paramHandle, const CVector3& value)
{
	if(paramHandle != -1)
	{
		glUniform4f(paramHandle, value.x, value.y, value.z, 0);
	}
}

template <>
void SetParamValue<CVector4>(uint32 paramHandle, const CVector4& value)
{
	if(paramHandle != -1)
	{
		glUniform4f(paramHandle, value.x, value.y, value.z, value.w);
	}
}

template<>
void SetParamValue<CColor>(uint32 paramHandle, const CColor& value)
{
	if(paramHandle != -1)
	{
		glUniform4f(paramHandle, value.r, value.g, value.b, value.a);
	}
}

void CGlEsUmbralEffect::UpdateConstants(const Palleon::GLESVIEWPORT_PARAMS& viewportParams, Palleon::CMaterial* material, const CMatrix4& worldMatrix)
{
	//Update vertex shader params
	{
		auto worldITMatrix = worldMatrix.Inverse().Transpose();
		auto viewITMatrix = viewportParams.viewMatrix.Inverse().Transpose();
		auto worldViewMatrix = worldMatrix * viewportParams.viewMatrix;
		auto worldViewProjMatrix = worldViewMatrix * viewportParams.projMatrix;
		CVector3 modelBBoxOffset(0, 0, 0);
		CVector3 modelBBoxScale(1, 1, 1);
		CVector3 fogParam(0, 10000, 1);
		
		glUniformMatrix4fv(m_worldMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&worldMatrix));
		glUniformMatrix4fv(m_worldITMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&worldITMatrix));
		glUniformMatrix4fv(m_viewITMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&viewITMatrix));
		glUniformMatrix4fv(m_worldViewProjMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&worldViewProjMatrix));
		SetParamValue(m_modelBBoxOffsetHandle, modelBBoxOffset);
		SetParamValue(m_modelBBoxScaleHandle, modelBBoxScale);
		SetParamValue(m_fogParamHandle, fogParam);
		glUniform1i(m_isSkinningHandle, 0);
	}
	
	//Update pixel shader params
	{
		auto modulateColor = material->GetEffectParamOrDefault("ps_modulateColor", CVector4(1, 1, 1, 1), m_modulateColorHandle != -1);
		auto ambientColor = material->GetEffectParamOrDefault("ps_ambientColor", CVector4(0, 0, 0, 0), m_ambientColorHandle != -1);
		auto diffuseColor = material->GetEffectParamOrDefault("ps_diffuseColor", CVector4(0, 0, 0, 0), m_diffuseColorHandle != -1);
		auto specularColor = material->GetEffectParamOrDefault("ps_specularColor", CVector4(0, 0, 0, 0), m_specularColorHandle != -1);
		auto shininess = material->GetEffectParamOrDefault("ps_shininess", 128.f, m_shininessHandle != -1);
		auto reflectivity = material->GetEffectParamOrDefault("ps_reflectivity", CVector3(0, 0, 0), m_reflectivityHandle != -1);
		auto normalPower = material->GetEffectParamOrDefault("ps_normalPower", 1.f, m_normalPowerHandle != -1);

		auto fresnelExp = material->GetEffectParamOrDefault("ps_fresnelExp", 1.f, m_fresnelExpHandle != -1);
		auto fresnelLightDiffBias = material->GetEffectParamOrDefault("ps_fresnelLightDiffBias", 1.f, m_fresnelLightDiffBiasHandle != -1);
		auto lightDiffusePower = material->GetEffectParamOrDefault("ps_lightDiffusePower", 0.67f, m_lightDiffusePowerHandle != -1);
		auto specularInfluence = material->GetEffectParamOrDefault("ps_specularInfluence", 0.0f, m_specularInfluenceHandle != -1);
		auto lightDiffuseInfluence = material->GetEffectParamOrDefault("ps_lightDiffuseInfluence", 0.56f, m_lightDiffuseInfluenceHandle != -1);
		auto reflectMapInfluence = material->GetEffectParamOrDefault("ps_reflectMapInfluence", 0.8f, m_reflectMapInfluenceHandle != -1);

		auto glareLdrScale = material->GetEffectParamOrDefault("ps_glareLdrScale", 1.0f, m_glareLdrScaleHandle != -1);
		auto refAlphaRestrain = material->GetEffectParamOrDefault("ps_refAlphaRestrain", 0.0f, m_refAlphaRestrainHandle != -1);
		auto velvetParam = material->GetEffectParamOrDefault("ps_velvetParam", CVector2(0, 0), m_velvetParamHandle != -1);
		
		glUniform1i(m_textureUnit0Handle, 0);
		glUniform1i(m_textureUnit1Handle, 1);
		glUniform1i(m_textureUnit2Handle, 2);
		glUniform1i(m_textureUnit3Handle, 3);
		glUniform1i(m_textureUnit4Handle, 4);
		glUniform1i(m_textureUnit5Handle, 5);
		glUniform1i(m_textureUnit10Handle, 6);
		
		SetParamValue(m_modulateColorHandle, modulateColor);
		SetParamValue(m_ambientColorHandle, ambientColor);
		SetParamValue(m_diffuseColorHandle, diffuseColor);
		SetParamValue(m_specularColorHandle, specularColor);
		SetParamValue(m_shininessHandle, shininess);
		SetParamValue(m_reflectivityHandle, reflectivity);
		SetParamValue(m_normalPowerHandle, normalPower);
		
		SetParamValue(m_fresnelExpHandle, fresnelExp);
		SetParamValue(m_fresnelLightDiffBiasHandle, fresnelLightDiffBias);
		SetParamValue(m_lightDiffusePowerHandle, lightDiffusePower);
		SetParamValue(m_specularInfluenceHandle, specularInfluence);
		SetParamValue(m_lightDiffuseInfluenceHandle, lightDiffuseInfluence);
		SetParamValue(m_reflectMapInfluenceHandle, reflectMapInfluence);
		
		SetParamValue(m_glareLdrScaleHandle, glareLdrScale);
		SetParamValue(m_refAlphaRestrainHandle, refAlphaRestrain);
		SetParamValue(m_velvetParamHandle, velvetParam);
				
		CVector4 latitudeParam(1, 0, 1, 0);
		auto ambientLightColor = viewportParams.viewport->GetEffectParamOrDefault("ps_ambientLightColor", CVector4(0, 0, 0, 0));
		CVector4 enableShadowFlag(1, 0, 1, 0);
		CVector4 dirLightDirections[2] =
		{
			viewportParams.viewport->GetEffectParamOrDefault("ps_dirLightDirection0", CVector4(0, 0, 0, 0)),
			viewportParams.viewport->GetEffectParamOrDefault("ps_dirLightDirection1", CVector4(0, 0, 0, 0))
		};
		
		CVector4 dirLightColors[2] =
		{
			viewportParams.viewport->GetEffectParamOrDefault("ps_dirLightColor0", CVector4(0, 0, 0, 0)),
			viewportParams.viewport->GetEffectParamOrDefault("ps_dirLightColor1", CVector4(0, 0, 0, 0))
		};
				
		SetParamValue(m_ambientLightColorHandle, ambientLightColor);
		SetParamValue(m_latitudeParamHandle, latitudeParam);
		SetParamValue(m_enableShadowFlagHandle, enableShadowFlag);
		glUniform4fv(m_dirLightDirectionsHandle, 2, reinterpret_cast<const float*>(&dirLightDirections));
		glUniform4fv(m_dirLightColorsHandle, 2, reinterpret_cast<const float*>(&dirLightColors));
	}
	
	CHECKGLERROR();
}

void CGlEsUmbralEffect::GetUniformLocations()
{
	std::map<std::string, uint32&> uniformLocations =
	{
		{ "vs_worldMatrix",				m_worldMatrixHandle					},
		{ "vs_worldITMatrix",			m_worldITMatrixHandle				},
		{ "vs_viewITMatrix",			m_viewITMatrixHandle				},
		{ "vs_worldViewMatrix",			m_worldViewMatrixHandle				},
		{ "vs_worldViewProjMatrix",		m_worldViewProjMatrixHandle			},
		{ "vs_ModelBBoxScale",			m_modelBBoxScaleHandle				},
		{ "vs_ModelBBoxOffSet",			m_modelBBoxOffsetHandle				},
		{ "vs_isSkining",				m_isSkinningHandle					},
		{ "vs_fogParam",				m_fogParamHandle					},
		
		{ "ps_textureUnit0",			m_textureUnit0Handle				},
		{ "ps_textureUnit1",			m_textureUnit1Handle				},
		{ "ps_textureUnit2",			m_textureUnit2Handle				},
		{ "ps_textureUnit3",			m_textureUnit3Handle				},
		{ "ps_textureUnit4",			m_textureUnit4Handle				},
		{ "ps_textureUnit5",			m_textureUnit5Handle				},
		{ "ps_textureUnit10",			m_textureUnit10Handle				},
		
		{ "ps_modulateColor",			m_modulateColorHandle				},
		{ "ps_ambientColor",			m_ambientColorHandle				},
		{ "ps_diffuseColor",			m_diffuseColorHandle				},
		{ "ps_specularColor",			m_specularColorHandle				},
		{ "ps_shininess",				m_shininessHandle					},
		{ "ps_reflectivity",			m_reflectivityHandle				},
		{ "ps_normalPower",				m_normalPowerHandle					},
		
		{ "ps_fresnelExp",				m_fresnelExpHandle					},
		{ "ps_fresnelLightDiffBias",	m_fresnelLightDiffBiasHandle		},
		{ "ps_lightDiffusePower",		m_lightDiffusePowerHandle			},
		{ "ps_specularInfluence",		m_specularInfluenceHandle			},
		{ "ps_lightDiffuseInfluence",	m_lightDiffuseInfluenceHandle		},
		{ "ps_reflectMapInfluence",		m_reflectMapInfluenceHandle			},
		
		{ "ps_glareLdrScale",			m_glareLdrScaleHandle				},
		{ "ps_refAlphaRestrain",		m_refAlphaRestrainHandle			},
		{ "ps_velvetParam",				m_velvetParamHandle					},
		
		{ "ps_ambientLightColor",		m_ambientLightColorHandle			},
		{ "ps_DirLightDirections",		m_dirLightDirectionsHandle			},
		{ "ps_DirLightColors",			m_dirLightColorsHandle				},
		{ "ps_latitudeParam",			m_latitudeParamHandle				},
		{ "ps_EnableShadowFlag",		m_enableShadowFlagHandle			},
	};
	
	for(const auto& uniformLocation : uniformLocations)
	{
		uniformLocation.second = glGetUniformLocation(m_program, uniformLocation.first.c_str());
	}
}

