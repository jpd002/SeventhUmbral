#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "../../Palleon/include/palleon/win32/Win32EmbedControl.h"

class CAppearanceViewer : public Framework::Win32::CDialog
{
public:
							CAppearanceViewer();
	virtual					~CAppearanceViewer();

protected:
	long					OnCommand(unsigned short cmd, unsigned short, HWND) override;

private:
	typedef std::shared_ptr<Palleon::CWin32EmbedControl> EmbedControlPtr;

	EmbedControlPtr			m_embedControl;
};
