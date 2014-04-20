#pragma once

#include "BaseChunk.h"
#include <vector>

class CContainerChunk : public CBaseChunk
{
public:
					CContainerChunk();
	virtual			~CContainerChunk() override;

	virtual void	Read(Framework::CStream&) override;

protected:
	void			LoadChildren(Framework::CStream&);

	uint32			m_childrenCount;
	uint32			m_unknown1;
	uint32			m_unknown2;
	uint32			m_unknown3;
};
