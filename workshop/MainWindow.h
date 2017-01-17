#pragma once

#include <map>
#include "win32/Dialog.h"
#include "win32/Tab.h"

class IDocument;

class CMainWindow : public Framework::Win32::CDialog
{
public:
								CMainWindow();
	virtual						~CMainWindow();

	void						DoMessageLoop();

protected:
	long						OnCommand(unsigned short, unsigned short, HWND) override;
	long						OnSysCommand(unsigned int, LPARAM) override;
	long						OnNotify(WPARAM, NMHDR*) override;
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::unique_ptr<Framework::Win32::CWindow> DocumentPtr;
	typedef std::map<uint32, DocumentPtr> DocumentMap;

	void						LoadWindowRect();
	void						SaveWindowRect();
	unsigned int				Destroy() override;

	void						SaveCurrentDocument();

	void						ShowWelcomePage();
	void						ShowZoneEditor();
	void						ShowAppearanceViewer();
	void						ShowWorldViewer(uint32);
	void						ShowSheetViewer(uint32);
	void						ShowAbout();

	void						InsertDocument(DocumentPtr&&);
	void						OnDocumentStateChanged();
	void						UpdateLayout();

	void						SelectTab(int);
	void						UnselectTab(int);

	void						OnTabRightClick();
	void						CloseTab(int);

	Framework::Win32::CTab		m_tabs;
	DocumentMap					m_documents;
	uint32						m_nextDocumentId = 1;
	int							m_tabContextMenuSelection = -1;
	HACCEL						m_accelerators = 0;
};
