#pragma once

#include "BaseChunk.h"

class CStringChunk : public CBaseChunk
{
public:
	virtual void		Read(Framework::CStream&) override;

	std::string			GetString() const;

private:
	std::string			m_string;
};
