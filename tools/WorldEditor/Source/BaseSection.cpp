#include "BaseSection.h"
#include <string.h>
#include <assert.h>

CBaseSection::CBaseSection()
{
	memset(&m_header, 0, sizeof(m_header));
}

CBaseSection::~CBaseSection()
{

}

void CBaseSection::Read(Framework::CStream& inputStream)
{
	inputStream.Read(&m_header, sizeof(SEDB_HEADER));
	assert(m_header.sectionId == 'BDES');
}

void CBaseSection::SetResourceId(const std::string& resourceId)
{
	m_resourceId = resourceId;
}

std::string CBaseSection::GetResourcePath() const
{
	return m_resourcePath;
}

void CBaseSection::SetResourcePath(const std::string& resourcePath)
{
	m_resourcePath = resourcePath;
}
