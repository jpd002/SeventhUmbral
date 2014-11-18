#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "win32/Splitter.h"
#include "Document.h"
#include "ZoneEditorPropertiesPane.h"
#include "ZoneEditorWorldPane.h"

class CZoneEditor : public Framework::Win32::CDialog, public IDocument
{
public:
								CZoneEditor(HWND, uint32);
	virtual						~CZoneEditor();

	virtual std::string			GetName() const override;

	virtual void				SetActive(bool) override;

protected:
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	struct ACTOR
	{
		uint32 baseModelId = 0;
	};

	typedef std::unique_ptr<CZoneEditorPropertiesPane> PropertiesPanePtr;
	typedef std::unique_ptr<CZoneEditorWorldPane> WorldPanePtr;
	typedef std::unique_ptr<Framework::Win32::CSplitter> SplitterPtr;
	typedef std::unordered_map<uint32, ACTOR> ActorMap;

	void						WorldNotificationRaised(const std::string&);

	SplitterPtr					m_mainSplitter;
	PropertiesPanePtr			m_propertiesPane;
	WorldPanePtr				m_worldPane;
	ActorMap					m_actors;
};
