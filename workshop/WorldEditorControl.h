#pragma once

#include "win32/Dialog.h"
#include "math/Vector3.h"
#include "palleon/win32/Win32EmbedControl.h"

class CWorldEditorControl : public Framework::Win32::CDialog
{
public:
	typedef Palleon::CWin32EmbedControl::NotificationRaisedEventType NotificationRaisedEventType;

	enum class CONTROL_SCHEME
	{
		EDITOR,
		VIEWER
	};

								CWorldEditorControl(HWND);
	virtual						~CWorldEditorControl();

	void						SetMap(uint32);

	void						SetControlScheme(CONTROL_SCHEME);

	CVector3					GetCameraPosition();
	void						SetCameraPosition(const CVector3&);

	void						CreateActor(uint32);
	void						SetActorBaseModelId(uint32, uint32);
	void						SetActorTopModelId(uint32, uint32);
	void						SetActorPosition(uint32, const CVector3&);

	void						SetActive(bool);

	NotificationRaisedEventType	NotificationRaised;

protected:
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::shared_ptr<Palleon::CWin32EmbedControl> EmbedControlPtr;

	void						CreateViewer();
	
	EmbedControlPtr				m_embedControl;
};
