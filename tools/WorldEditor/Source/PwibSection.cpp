#include <assert.h>
#include "PwibSection.h"
#include "SectionLoader.h"

CPwibSection::CPwibSection()
{

}

CPwibSection::~CPwibSection()
{

}

void CPwibSection::Read(Framework::CStream& inputStream)
{
	uint32 signature = inputStream.Read32();
	assert(signature == 'BIWP');
	uint32 fileSize = inputStream.Read32_MSBF();
	uint32 unknown = inputStream.Read32_MSBF();
	uint32 dataOffset = inputStream.Read32_MSBF();
	auto child = CSectionLoader::ReadSection(inputStream);
	m_children.push_back(child);
}
