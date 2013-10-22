#pragma once

#include "BaseSection.h"

class CResourceSection : public CBaseSection
{
public:
						CResourceSection();
	virtual				~CResourceSection();

	virtual void		Read(Framework::CStream&) override;

protected:
	struct SEDBRES_HEADER
	{
		uint32	resourceCount;
		uint32	stringsOffset;
		uint32	stringCount;
		uint32	resourceType;
	};
	static_assert(sizeof(SEDBRES_HEADER) == 0x10, "Size of SEDBRES_HEADER struct must be 16 bytes.");

	struct SEDBRES_ENTRY
	{
		uint32	index;
		uint32	offset;
		uint32	size;
		uint32	type;
	};
	static_assert(sizeof(SEDBRES_ENTRY) == 0x10, "Size of SEDBRED_ENTRY struct must be 16 bytes.");
};
