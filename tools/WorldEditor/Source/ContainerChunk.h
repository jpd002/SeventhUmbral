#pragma once

#include "BaseChunk.h"

class CContainerChunk : public CBaseChunk
{
public:
					CContainerChunk();
	virtual			~CContainerChunk();

	virtual void	Read(Framework::CStream&) override;

protected:
	uint32			m_childrenCount;
	uint32			m_unknown1;
	uint32			m_unknown2;
	uint32			m_unknown3;
};
