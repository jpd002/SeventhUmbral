#pragma once

#include "BaseChunk.h"

class CNameChunk : public CBaseChunk
{
public:
	virtual void		Read(Framework::CStream&) override;

	std::string			GetName() const;

private:
	std::string			m_name;
};
