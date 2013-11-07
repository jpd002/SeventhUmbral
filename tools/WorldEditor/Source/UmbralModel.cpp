#include "UmbralModel.h"
#include "UmbralMesh.h"
#include "MeshChunk.h"

CUmbralModel::CUmbralModel(const ModelChunkPtr& modelChunk)
{
	auto meshNodes = modelChunk->SelectNodes<CMeshChunk>();
	assert(!meshNodes.empty());
	for(const auto& meshNode : meshNodes)
	{
		auto mesh = std::make_shared<CUmbralMesh>(meshNode);
		AppendChild(mesh);
	}
}

CUmbralModel::~CUmbralModel()
{

}
