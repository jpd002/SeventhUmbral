#pragma once

#include "win32/Dialog.h"
#include "win32/Edit.h"
#include "math/Vector3.h"
#include "palleon/win32/Win32EmbedControl.h"

class CZoneEditorWorldPane : public Framework::Win32::CDialog
{
public:
	typedef Palleon::CWin32EmbedControl::NotificationRaisedEventType NotificationRaisedEventType;

								CZoneEditorWorldPane(HWND, uint32);
	virtual						~CZoneEditorWorldPane();

	void						CreateActor(uint32);
	void						SetActorBaseModelId(uint32, uint32);
	void						SetActorPosition(uint32, const CVector3&);

	void						SetActive(bool);

	NotificationRaisedEventType	NotificationRaised;

protected:
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::shared_ptr<Palleon::CWin32EmbedControl> EmbedControlPtr;

	void						CreateViewer();
	
	EmbedControlPtr				m_embedControl;
	uint32						m_mapId = 0;
};
