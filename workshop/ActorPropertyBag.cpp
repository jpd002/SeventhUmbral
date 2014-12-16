#include "ActorPropertyBag.h"
#include "string_format.h"
#include "string_cast.h"
#include "math/MathStringUtils.h"

CActorPropertyBag::CActorPropertyBag(CZoneEditor::ACTOR& actor)
: m_actor(actor)
{

}

CActorPropertyBag::~CActorPropertyBag()
{

}

unsigned int CActorPropertyBag::GetPropertyCount() const
{
	return ACTOR_PROPERTY_MAX;
}

std::tstring CActorPropertyBag::GetPropertyName(unsigned int index) const
{
	switch(index)
	{
	case ACTOR_PROPERTY_ID:
		return _T("Id");
	case ACTOR_PROPERTY_BASEMODELID:
		return _T("Base Model Id");
	case ACTOR_PROPERTY_NAMESTRINGID:
		return _T("Name String Id");
	case ACTOR_PROPERTY_POSITION:
		return _T("Position");
	default:
		assert(0);
		return _T("(Unknown Property)");
	}
}

std::tstring CActorPropertyBag::GetPropertyValue(unsigned int index) const
{
	switch(index)
	{
	case ACTOR_PROPERTY_ID:
		return string_format(_T("%d"), m_actor.id);
		break;
	case ACTOR_PROPERTY_BASEMODELID:
		return string_format(_T("%d"), m_actor.baseModelId);
		break;
	case ACTOR_PROPERTY_NAMESTRINGID:
		return string_format(_T("%d"), m_actor.nameStringId);
		break;
	case ACTOR_PROPERTY_POSITION:
		return string_cast<std::tstring>(MathStringUtils::ToString(m_actor.position));
		break;
	default:
		assert(0);
		return _T("");
	}
}

void CActorPropertyBag::SetPropertyValue(unsigned int index, const std::tstring& valueString)
{
	switch(index)
	{
	case ACTOR_PROPERTY_BASEMODELID:
		{
			uint32 value = _ttoi(valueString.c_str());
			if(m_actor.baseModelId == value) return;
			m_actor.baseModelId = value;
			PropertyChanged(index);
		}
		break;
	}
}
