#include <memory>
#include "AppearanceDatabase.h"
#include "xml/Parser.h"
#include "xml/Utils.h"

CAppearanceDatabase::CAppearanceDatabase()
{

}

CAppearanceDatabase::~CAppearanceDatabase()
{

}

CAppearanceDatabase CAppearanceDatabase::CreateFromXml(Framework::CStream& stream)
{
	CAppearanceDatabase result;
	auto documentNode = std::unique_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(stream));
	auto appearanceNodes = documentNode->SelectNodes("Appearances/Appearance");
	for(const auto& appearanceNode : appearanceNodes)
	{
		APPEARANCE_DEFINITION appearance;
		uint32 itemId = Framework::Xml::GetAttributeIntValue(appearanceNode, "ItemId");
		for(const auto& appearanceAttribute : appearanceNode->GetAttributes())
		{
			if(appearanceAttribute.first == "ItemId") continue;
			uint32 attributeValue = atoi(appearanceAttribute.second.c_str());
			appearance.attributes.insert(std::make_pair(appearanceAttribute.first, attributeValue));
		}
		result.m_appearances.insert(std::make_pair(itemId, appearance));
	}
	return result;
}

const APPEARANCE_DEFINITION* CAppearanceDatabase::GetAppearanceForItemId(uint32 itemId) const
{
	auto appearanceIterator = m_appearances.find(itemId);
	if(appearanceIterator == std::end(m_appearances))
	{
		return nullptr;
	}
	return &appearanceIterator->second;
}
