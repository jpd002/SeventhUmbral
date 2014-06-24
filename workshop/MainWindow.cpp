#include "MainWindow.h"
#include "resource.h"
#include "SheetViewer.h"
#include "AppearanceViewer.h"

CMainWindow::CMainWindow()
: CDialog(MAKEINTRESOURCE(IDD_MAINWINDOW))
{
	SetClassPtr();
	m_tabs = Framework::Win32::CTab(GetItem(IDC_MAINWINDOW_TABS));

	UpdateLayout();
}

CMainWindow::~CMainWindow()
{

}

long CMainWindow::OnCommand(unsigned short id, unsigned short cmd, HWND)
{
	switch(id)
	{
	case ID_MAINMENU_INSPECT_WEAPONS:
		ShowApperanceViewer();
		break;
	case ID_MAINMENU_INSPECT_DATASHEETS:
		ShowDataSheetViewer();
		break;
	case IDCANCEL:
		for(const auto& documentPair : m_documents)
		{
			documentPair.second->Destroy();
		}
		m_documents.clear();
		Destroy();
		break;
	}
	return FALSE;
}

long CMainWindow::OnNotify(WPARAM param, NMHDR* hdr)
{
	if(CWindow::IsNotifySource(&m_tabs, hdr))
	{
		switch(hdr->code)
		{
		case TCN_SELCHANGING:
			UnselectTab(m_tabs.GetSelection());
			break;
		case TCN_SELCHANGE:
			SelectTab(m_tabs.GetSelection());
			break;
		}
	}
	return FALSE;
}

long CMainWindow::OnSize(unsigned int, unsigned int, unsigned int)
{
	UpdateLayout();
	return FALSE;
}

void CMainWindow::ShowApperanceViewer()
{
	auto appearanceViewer = std::make_unique<CAppearanceViewer>(m_hWnd);
	InsertDocument(std::move(appearanceViewer));
}

void CMainWindow::ShowDataSheetViewer()
{
	auto sheetViewer = std::make_unique<CSheetViewer>(m_hWnd);
	InsertDocument(std::move(sheetViewer));
}

void CMainWindow::InsertDocument(DocumentPtr&& document)
{
	int currentSelection = m_tabs.GetSelection();
	if(currentSelection != -1)
	{
		UnselectTab(currentSelection);
	}
	m_documents.insert(std::make_pair(m_nextDocumentId, std::move(document)));
	unsigned int tabIndex = m_tabs.InsertTab(_T("New Document"));
	m_tabs.SetTabData(tabIndex, m_nextDocumentId);
	m_tabs.SetSelection(tabIndex);
	SelectTab(tabIndex);
	m_nextDocumentId++;
}

void CMainWindow::SelectTab(int selection)
{
	assert(selection != -1);
	uint32 documentId = m_tabs.GetTabData(selection);
	auto documentIterator = m_documents.find(documentId);
	assert(documentIterator != std::end(m_documents));
	if(documentIterator != std::end(m_documents))
	{
		documentIterator->second->Show(SW_SHOW);
		UpdateLayout();
	}
}

void CMainWindow::UnselectTab(int selection)
{
	assert(selection != -1);
	uint32 documentId = m_tabs.GetTabData(selection);
	auto documentIterator = m_documents.find(documentId);
	assert(documentIterator != std::end(m_documents));
	if(documentIterator != std::end(m_documents))
	{
		documentIterator->second->Show(SW_HIDE);
	}
}

void CMainWindow::UpdateLayout()
{
	auto clientRect = GetClientRect();
	m_tabs.SetSizePosition(clientRect);

	int selection = m_tabs.GetSelection();
	if(selection != -1)
	{
		uint32 documentId = m_tabs.GetTabData(selection);
		auto documentIterator = m_documents.find(documentId);
		if(documentIterator != std::end(m_documents))
		{
			auto displayAreaRect = m_tabs.GetDisplayAreaRect();
			displayAreaRect.ClientToScreen(m_tabs.m_hWnd);
			displayAreaRect.ScreenToClient(m_hWnd);
			documentIterator->second->SetSizePosition(displayAreaRect);
			SetWindowPos(m_tabs.m_hWnd, documentIterator->second->m_hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}
	}
}
