#pragma once

#include <map>
#include "Stream.h"

struct APPEARANCE_DEFINITION
{
	typedef std::map<std::string, uint32> AttributeMap;

	AttributeMap	attributes;
};

class CAppearanceDatabase
{
public:
									CAppearanceDatabase();
	virtual							~CAppearanceDatabase();

	static CAppearanceDatabase		CreateFromXml(Framework::CStream&);

	const APPEARANCE_DEFINITION*	GetAppearanceForItemId(uint32) const;

private:
	typedef std::map<uint32, APPEARANCE_DEFINITION> AppearanceMap;

	AppearanceMap					m_appearances;
};
