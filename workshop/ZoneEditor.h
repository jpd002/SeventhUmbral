#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "win32/Splitter.h"
#include "win32/ToolBar.h"
#include "math/Vector3.h"
#include "Document.h"
#include "ZoneEditorPropertiesPane.h"
#include "WorldEditorControl.h"

class CZoneEditor : public Framework::Win32::CDialog, public IDocument
{
public:
	struct ACTOR
	{
		uint32		id = 0;
		uint32		baseModelId = 0;
		CVector3	position = CVector3(0, 0, 0);
		uint32		nameStringId = 0;
	};

								CZoneEditor(HWND, uint32);
	virtual						~CZoneEditor();

	virtual std::string			GetName() const override;

	virtual void				SetActive(bool) override;

	virtual bool				GetDirty() const override;

	virtual void				Save();

protected:
	long						OnCommand(unsigned short, unsigned short, HWND) override;
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::unique_ptr<CZoneEditorPropertiesPane> PropertiesPanePtr;
	typedef std::unique_ptr<CWorldEditorControl> WorldPanePtr;
	typedef std::unique_ptr<Framework::Win32::CSplitter> SplitterPtr;
	typedef std::unordered_map<uint32, ACTOR> ActorMap;

	void						CreateNewActor();

	uint32						GenerateUnusedActorId() const;
	void						LoadZoneDefinition();
	void						SaveZoneDefinition();

	void						SetDirty(bool);

	void						PropertyChanged(unsigned int);
	void						WorldNotificationRaised(const std::string&);

	Framework::Win32::CToolBar	m_toolbar;
	SplitterPtr					m_mainSplitter;
	PropertiesPanePtr			m_propertiesPane;
	WorldPanePtr				m_worldPane;
	ActorMap					m_actors;
	uint32						m_selectedActorId = 0;
	bool						m_documentDirty = false;
};
