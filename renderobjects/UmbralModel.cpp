#include "UmbralModel.h"
#include "UmbralMesh.h"

CUmbralModel::CUmbralModel(const ModelChunkPtr& modelChunk)
{
	auto resourceSection = std::dynamic_pointer_cast<CResourceSection>(modelChunk->GetParent()->GetParent()->GetParent()->GetParent());
	assert(resourceSection);
	auto shaderSections = resourceSection->SelectNodes<CShaderSection>();

	auto meshNodes = modelChunk->SelectNodes<CMeshChunk>();
	assert(!meshNodes.empty());
	for(const auto& meshNode : meshNodes)
	{
		auto name = meshNode->SelectNode<CStringChunk>();
		assert(name);

		auto shaderSection = FindShaderForName(shaderSections, name->GetString());

		auto mesh = std::make_shared<CUmbralMesh>(meshNode, shaderSection);
		mesh->SetName(name->GetString());
		AppendChild(mesh);

		m_boundingSphere = m_boundingSphere.Accumulate(mesh->GetBoundingSphere());
	}
}

CUmbralModel::~CUmbralModel()
{

}

const CSphere& CUmbralModel::GetBoundingSphere() const
{
	return m_boundingSphere;
}

void CUmbralModel::SetLocalTexture(const ResourceNodePtr& texture)
{
	for(const auto& child : m_children)
	{
		if(auto mesh = std::dynamic_pointer_cast<CUmbralMesh>(child))
		{
			mesh->SetLocalTexture(texture);
		}
	}
}

ShaderSectionPtr CUmbralModel::FindShaderForName(const std::vector<ShaderSectionPtr>& shaders, const std::string& name)
{
	for(const auto& shader : shaders)
	{
		auto resourcePath = shader->GetResourcePath();
		if(resourcePath.find(name) != std::string::npos)
		{
			return shader;
		}
	}
	return ShaderSectionPtr();
}
