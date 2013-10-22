#include "SectionLoader.h"
#include "ResourceSection.h"
#include <vector>
#include <assert.h>

CSectionLoader::CSectionLoader()
{

}

CSectionLoader::~CSectionLoader()
{

}

void CSectionLoader::ReadSEDBSection(Framework::CStream& inputStream)
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
}

void CSectionLoader::ReadSections(Framework::CStream& inputStream)
{
	while(1)
	{
		if(inputStream.IsEOF()) break;
		uint32 sectionId = inputStream.Read32();
		switch(sectionId)
		{
		case 'BIWP':
			//Not of interest for now
			inputStream.Seek(0xC, Framework::STREAM_SEEK_CUR);
			break;
		case 'BDES':
			inputStream.Seek(-0x4, Framework::STREAM_SEEK_CUR);
			ReadSEDBSection(inputStream);
			break;
		}
	}
}
