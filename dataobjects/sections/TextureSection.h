#pragma once

#include "BaseSection.h"

class CTextureSection : public CBaseSection
{
public:
					CTextureSection();
	virtual			~CTextureSection();

	virtual void	Read(Framework::CStream&) override;

private:
	uint32			m_header[4];
};
