#pragma once

#include "win32/Dialog.h"
#include "win32/Static.h"
#include "../../Palleon/include/palleon/win32/Win32EmbedControl.h"

class CAppearanceViewerActorViewPane : public Framework::Win32::CDialog
{
public:
									CAppearanceViewerActorViewPane(HWND);
	virtual							~CAppearanceViewerActorViewPane();

	void							SetActor(uint32, uint32);

protected:
	long							OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::shared_ptr<Palleon::CWin32EmbedControl> EmbedControlPtr;

	void							CreateActorViewer();

	EmbedControlPtr					m_embedControl;
	Framework::Win32::CStatic		m_failLabel;
};
