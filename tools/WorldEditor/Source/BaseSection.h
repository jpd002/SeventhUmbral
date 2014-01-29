#pragma once

#include "Types.h"
#include "Stream.h"
#include "ResourceNode.h"
#include <memory>

class CBaseSection : public CResourceNode
{
public:
						CBaseSection();
	virtual				~CBaseSection();

	virtual void		Read(Framework::CStream&);

	uint32				GetSize() const;

	std::string			GetResourceId() const;
	void				SetResourceId(const std::string&);

	std::string			GetResourcePath() const;
	void				SetResourcePath(const std::string&);

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
	std::string			m_resourceId;
	std::string			m_resourcePath;
};

typedef std::shared_ptr<CBaseSection> SectionPtr;
