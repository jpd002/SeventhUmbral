#include "SectionLoader.h"
#include "ResourceSection.h"
#include "PwibSection.h"
#include <vector>
#include <assert.h>

ResourceNodePtr CSectionLoader::ReadSEDBSection(Framework::CStream& inputStream)
{
	uint32 sectionId = inputStream.Read32();
	uint32 subSectionId = inputStream.Read32();
	inputStream.Seek(-8, Framework::STREAM_SEEK_CUR);
	SectionPtr section;
	switch(subSectionId)
	{
	case ' SER':	//RES 
		section = std::make_shared<CResourceSection>();
		break;
	default:
		assert(0);
		break;
	}
	section->Read(inputStream);
	return section;
}

ResourceNodePtr CSectionLoader::ReadSection(Framework::CStream& inputStream)
{
	uint32 sectionId = inputStream.Read32();
	inputStream.Seek(-4, Framework::STREAM_SEEK_CUR);
	ResourceNodePtr result;
	switch(sectionId)
	{
	case 'BIWP':
		{
			auto section = std::make_shared<CPwibSection>();
			section->Read(inputStream);
			result = section;
		}
		break;
	case 'BDES':
		result = ReadSEDBSection(inputStream);
		break;
	default:
		assert(0);
		break;
	}
	return result;
}
