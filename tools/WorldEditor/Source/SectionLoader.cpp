#include "SectionLoader.h"
#include "ResourceSection.h"
#include "PwibSection.h"
#include "TextureSection.h"
#include <vector>
#include <assert.h>

ResourceNodePtr CSectionLoader::ReadSEDBSection(const ResourceNodePtr& parent, Framework::CStream& inputStream)
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
	case '\0bxt':
		section = std::make_shared<CTextureSection>();
		break;
	default:
		assert(0);
		break;
	}
	if(parent)
	{
		parent->AddChild(section);
	}
	section->Read(inputStream);
	return section;
}

ResourceNodePtr CSectionLoader::ReadSection(const ResourceNodePtr& parent, Framework::CStream& inputStream)
{
	uint32 sectionId = inputStream.Read32();
	inputStream.Seek(-4, Framework::STREAM_SEEK_CUR);
	ResourceNodePtr result;
	switch(sectionId)
	{
	case 'BIWP':
		{
			auto section = std::make_shared<CPwibSection>();
			if(parent)
			{
				parent->AddChild(section);
			}
			section->Read(inputStream);
			result = section;
		}
		break;
	case 'BDES':
		result = ReadSEDBSection(parent, inputStream);
		break;
	default:
		assert(0);
		break;
	}
	return result;
}
