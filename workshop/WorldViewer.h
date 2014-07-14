#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "win32/Static.h"
#include "Document.h"
#include "../../Palleon/include/palleon/win32/Win32EmbedControl.h"

class CWorldViewer : public Framework::Win32::CDialog, public IDocument
{
public:
								CWorldViewer(HWND);
	virtual						~CWorldViewer();

	virtual std::string			GetName() const override;

protected:
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::shared_ptr<Palleon::CWin32EmbedControl> EmbedControlPtr;

	void						CreateViewer();

	EmbedControlPtr				m_embedControl;
};
