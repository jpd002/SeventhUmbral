#include "IosUmbralEffect.h"
#include "IosUmbralEffectGenerator.h"

CIosUmbralEffect::CIosUmbralEffect(const CD3DShader& vertexShader, const CD3DShader& pixelShader)
: m_vertexShader(vertexShader)
, m_pixelShader(pixelShader)
{
	auto vertexShaderText = CIosUmbralEffectGenerator::GenerateVertexShader(m_vertexShader);
	auto pixelShaderText = CIosUmbralEffectGenerator::GeneratePixelShader(m_vertexShader, m_pixelShader);
	
	AttributeBindingArray attributeBindings;
	attributeBindings.push_back(std::make_pair(Athena::VERTEX_ITEM_ID_POSITION, "a_position0"));
	attributeBindings.push_back(std::make_pair(Athena::VERTEX_ITEM_ID_UV0, "a_texcoord0"));
	
	BuildProgram(vertexShaderText, pixelShaderText, attributeBindings);

	m_worldMatrixHandle			= glGetUniformLocation(m_program, "worldMatrix");
	m_worldViewMatrixHandle		= glGetUniformLocation(m_program, "worldViewMatrix");
	m_worldViewProjMatrixHandle	= glGetUniformLocation(m_program, "worldViewProjMatrix");
	m_modelBBoxScaleHandle		= glGetUniformLocation(m_program, "ModelBBoxScale");
	m_modelBBoxOffsetHandle		= glGetUniformLocation(m_program, "ModelBBoxOffSet");
	m_isSkiningHandle			= glGetUniformLocation(m_program, "isSkining");
	
	CHECKGLERROR();	
}

CIosUmbralEffect::~CIosUmbralEffect()
{

}

template <typename ParamType> 
void SetParamValue(uint8* constantBufferPtr, uint32 paramOffset, const ParamType& value)
{
	if(paramOffset != -1)
	{
		*reinterpret_cast<ParamType*>(constantBufferPtr + paramOffset) = value;
	}
}

template <typename ParamType>
ParamType GetMaterialEffectParamOrDefault(const Athena::MaterialPtr& material, const std::string& paramName, uint32 paramOffset, const ParamType& defaultValue);

template <>
float GetMaterialEffectParamOrDefault<float>(const Athena::MaterialPtr& material, const std::string& paramName, uint32 paramOffset, const float& defaultValue)
{
	auto effectParam = material->GetEffectParameter(paramName);
	assert((paramOffset == -1) || !effectParam.IsNull());
	if(effectParam.IsNull()) return defaultValue;
	return effectParam.GetScalar();
}

template <>
CVector3 GetMaterialEffectParamOrDefault<CVector3>(const Athena::MaterialPtr& material, const std::string& paramName, uint32 paramOffset, const CVector3& defaultValue)
{
	auto effectParam = material->GetEffectParameter(paramName);
	assert((paramOffset == -1) || !effectParam.IsNull());
	if(effectParam.IsNull()) return defaultValue;
	return effectParam.GetVector3();
}

template <>
CVector4 GetMaterialEffectParamOrDefault<CVector4>(const Athena::MaterialPtr& material, const std::string& paramName, uint32 paramOffset, const CVector4& defaultValue)
{
	auto effectParam = material->GetEffectParameter(paramName);
	assert((paramOffset == -1) || !effectParam.IsNull());
	if(effectParam.IsNull()) return defaultValue;
	if(effectParam.IsVector3())
	{
		return CVector4(effectParam.GetVector3(), 0);
	}
	else
	{
		return effectParam.GetVector4();
	}
}

void CIosUmbralEffect::UpdateConstants(const Athena::MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewMatrix, const CMatrix4& projMatrix,
	const CMatrix4& shadowViewProjMatrix)
{
	//Update vertex shader params
	{
		auto worldITMatrix = worldMatrix.Inverse().Transpose();
		auto viewITMatrix = viewMatrix.Inverse().Transpose();
		auto worldViewMatrix = worldMatrix * viewMatrix;
		auto worldViewProjMatrix = worldViewMatrix * projMatrix;
		CVector3 modelBBoxOffset(0, 0, 0);
		CVector3 modelBBoxScale(1, 1, 1);
		
		glUniformMatrix4fv(m_worldViewProjMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&worldViewProjMatrix));
		glUniformMatrix4fv(m_worldMatrixHandle, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&worldMatrix));
		glUniform4f(m_modelBBoxOffsetHandle, modelBBoxOffset.x, modelBBoxOffset.y, modelBBoxOffset.z, 0);
		glUniform4f(m_modelBBoxScaleHandle, modelBBoxScale.x, modelBBoxScale.y, modelBBoxScale.z, 0);
		glUniform1i(m_isSkiningHandle, 0);
	}
	
	CHECKGLERROR();
}
