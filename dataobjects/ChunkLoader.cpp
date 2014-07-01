#include "ResourceDefs.h"
#include "ChunkLoader.h"

ChunkPtr CChunkLoader::Load(const ResourceNodePtr& parent, Framework::CStream& inputStream)
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
	case 'MARP':
		result = std::make_shared<CPramChunk>();
		break;
	case 'PRGP':
		result = std::make_shared<CPgrpChunk>();
		break;
	case 'BBAA':
		if(auto boundingBoxContainerChunk = std::dynamic_pointer_cast<CBoundingBoxContainerChunk>(parent))
		{
			result = std::make_shared<CBoundingBoxChunk>();
		}
		else
		{
			result = std::make_shared<CBoundingBoxContainerChunk>();
		}
		break;
	case 'PMOC':
		result = std::make_shared<CCompChunk>();
		break;
	default:
		//Unknown chunk type
		result = std::make_shared<CBaseChunk>();
		break;
	}

	result->Read(inputStream);

	return result;
}
