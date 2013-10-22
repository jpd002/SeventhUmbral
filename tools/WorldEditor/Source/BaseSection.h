#pragma once

#include "Types.h"
#include "Stream.h"
#include <memory>

class CBaseSection
{
public:
						CBaseSection();
	virtual				~CBaseSection();

	virtual void		Read(Framework::CStream&);

protected:
	struct SEDB_HEADER
	{
		uint32	sectionId;
		uint32	subSectionId;
		uint32	version;
		uint32	unknown1;
		uint32	sectionSize;
		uint8	unknown2[0x1C];
	};
	static_assert(sizeof(SEDB_HEADER) == 0x30, "Size of SEDB_HEADER struct must be 48 bytes.");

	SEDB_HEADER			m_header;
};

typedef std::shared_ptr<CBaseSection> SectionPtr;
