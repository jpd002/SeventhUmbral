#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "Document.h"
#include "WorldEditorControl.h"

class CWorldViewer : public Framework::Win32::CDialog, public IDocument
{
public:
								CWorldViewer(HWND, uint32);
	virtual						~CWorldViewer();

	virtual std::string			GetName() const override;

	virtual void				SetActive(bool) override;

protected:
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::unique_ptr<CWorldEditorControl> WorldEditorControlPtr;

	WorldEditorControlPtr		m_viewer;
	uint32						m_mapId = 0;
};
