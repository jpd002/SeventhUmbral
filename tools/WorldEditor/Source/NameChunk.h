#pragma once

#include "BaseChunk.h"

class CNameChunk : public CBaseChunk
{
public:
	virtual void		Read(Framework::CStream&) override;

private:
	std::string			m_name;
};
