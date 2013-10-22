#pragma once

#include "BaseChunk.h"

class CStringChunk : public CBaseChunk
{
public:
	virtual void		Read(Framework::CStream&) override;

private:
	std::string			m_string;
};
