#include "ChunkLoader.h"
#include "ShaderChunk.h"
#include "FileChunk.h"
#include "ModelChunk.h"
#include "ModelContainerChunk.h"
#include "HeaderChunk.h"
#include "LtcdChunk.h"
#include "WrbChunk.h"
#include "NameChunk.h"
#include "MeshChunk.h"
#include "StringChunk.h"
#include "StreamChunk.h"

ChunkPtr CChunkLoader::Load(Framework::CStream& inputStream)
{
	uint32 chunkType = inputStream.Read32();
	inputStream.Seek(-4, Framework::STREAM_SEEK_CUR);

	ChunkPtr result;
	switch(chunkType)
	{
	case '\0DHS':		//Shader Section
		result = std::make_shared<CShaderChunk>();
		break;
	case 'ELIF':		//Shader Section
		result = std::make_shared<CFileChunk>();
		break;
	case '\0BRW':		//Model Section
		result = std::make_shared<CWrbChunk>();
		break;
	case '\0LDM':
		result = std::make_shared<CModelChunk>();
		break;
	case 'CLDM':		//Model Section
		result = std::make_shared<CModelContainerChunk>();
		break;
	case 'DAEH':		//Model Section (?)
		result = std::make_shared<CHeaderChunk>();
		break;
	case 'DCTL':		//Model Section
		result = std::make_shared<CLtcdChunk>();
		break;
	case 'EMAN':		//Model Section
		result = std::make_shared<CNameChunk>();
		break;
	case 'HSEM':		//Model Section
		result = std::make_shared<CMeshChunk>();
		break;
	case '\0RTS':
		result = std::make_shared<CStringChunk>();
		break;
	case 'SMTS':
		result = std::make_shared<CStreamChunk>();
		break;
	default:
		//Unknown chunk type
		result = std::make_shared<CBaseChunk>();
		break;
	}

	result->Read(inputStream);

	return result;
}
