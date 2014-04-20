#include <assert.h>
#include "PwibSection.h"
#include "../SectionLoader.h"

CPwibSection::CPwibSection()
: m_dataOffset(0)
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
	m_dataOffset = inputStream.Read32_MSBF();
	auto child = CSectionLoader::ReadSection(shared_from_this(), inputStream);
}

uint32 CPwibSection::GetDataOffset() const
{
	return m_dataOffset;
}
