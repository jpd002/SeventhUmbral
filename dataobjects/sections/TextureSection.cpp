#include "TextureSection.h"
#include "../GtexData.h"

CTextureSection::CTextureSection()
{
	memset(&m_header, 0, sizeof(m_header));
}

CTextureSection::~CTextureSection()
{

}

void CTextureSection::Read(Framework::CStream& inputStream)
{
	CBaseSection::Read(inputStream);
	inputStream.Read(m_header, sizeof(m_header));

	auto gtex = std::make_shared<CGtexData>();
	AddChild(gtex);
	gtex->Read(inputStream);
}
