#pragma once

#include "win32/PropertyGrid.h"
#include "ZoneEditor.h"

class CActorPropertyBag : public Framework::Win32::CPropertyBag
{
public:
	typedef boost::signals2::signal<void (unsigned int)> PropertyChangedEvent;

	enum PROPERTIES
	{
		ACTOR_PROPERTY_ID,
		ACTOR_PROPERTY_BASEMODELID,
		ACTOR_PROPERTY_NAMESTRINGID,
		ACTOR_PROPERTY_POSITION,
		ACTOR_PROPERTY_MAX,
	};

							CActorPropertyBag(CZoneEditor::ACTOR& actor);
	virtual					~CActorPropertyBag();

	unsigned int			GetPropertyCount() const override;

	std::tstring			GetPropertyName(unsigned int) const override;
	std::tstring			GetPropertyValue(unsigned int) const override;
	void					SetPropertyValue(unsigned int, const std::tstring&) override;

	PropertyChangedEvent	PropertyChanged;

private:
	CZoneEditor::ACTOR&		m_actor;
};
