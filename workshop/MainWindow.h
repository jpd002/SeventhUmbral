#pragma once

#include <map>
#include "win32/Dialog.h"
#include "win32/Tab.h"

class CMainWindow : public Framework::Win32::CDialog
{
public:
								CMainWindow();
	virtual						~CMainWindow();

protected:
	long						OnCommand(unsigned short, unsigned short, HWND) override;
	long						OnNotify(WPARAM, NMHDR*) override;
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::unique_ptr<Framework::Win32::CWindow> DocumentPtr;
	typedef std::map<uint32, DocumentPtr> DocumentMap;

	void						Destroy();
	void						ShowDataSheetViewer();
	void						ShowApperanceViewer();

	void						InsertDocument(DocumentPtr&&);
	void						UpdateLayout();
	void						SelectTab(int);
	void						UnselectTab(int);

	Framework::Win32::CTab		m_tabs;
	DocumentMap					m_documents;
	uint32						m_nextDocumentId = 1;
};
