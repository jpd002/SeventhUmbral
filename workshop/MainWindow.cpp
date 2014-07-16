#include "string_cast.h"
#include "MainWindow.h"
#include "resource.h"
#include "WelcomePage.h"
#include "SheetViewer.h"
#include "AppearanceViewer.h"
#include "WorldViewer.h"
#include "AboutWindow.h"
#include "AppConfig.h"
#include "Document.h"

#define PREF_MAINWINDOW_RECT_LEFT		"workshop.mainwindow.rect.left"
#define PREF_MAINWINDOW_RECT_TOP		"workshop.mainwindow.rect.top"
#define PREF_MAINWINDOW_RECT_RIGHT		"workshop.mainwindow.rect.right"
#define PREF_MAINWINDOW_RECT_BOTTOM		"workshop.mainwindow.rect.bottom"
#define PREF_MAINWINDOW_MAXIMIZED		"workshop.mainwindow.maximized"

CMainWindow::CMainWindow()
: CDialog(MAKEINTRESOURCE(IDD_MAINWINDOW))
{
	CAppConfig::GetInstance().RegisterPreferenceInteger(PREF_MAINWINDOW_RECT_LEFT, 0);
	CAppConfig::GetInstance().RegisterPreferenceInteger(PREF_MAINWINDOW_RECT_TOP, 0);
	CAppConfig::GetInstance().RegisterPreferenceInteger(PREF_MAINWINDOW_RECT_RIGHT, 0);
	CAppConfig::GetInstance().RegisterPreferenceInteger(PREF_MAINWINDOW_RECT_BOTTOM, 0);
	CAppConfig::GetInstance().RegisterPreferenceBoolean(PREF_MAINWINDOW_MAXIMIZED, true);

	SetClassPtr();
	m_tabs = Framework::Win32::CTab(GetItem(IDC_MAINWINDOW_TABS));

	ShowWelcomePage();

	LoadWindowRect();
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
	case ID_MAINMENU_INSPECT_WORLD:
		ShowWorldViewer();
		break;
	case ID_MAINMENU_INSPECT_GAMEDATASHEETS:
		ShowGameDataSheetViewer();
		break;
	case ID_MAINMENU_INSPECT_VARDATASHEETS:
		ShowVarDataSheetViewer();
		break;
	case ID_HELP_ABOUT:
		ShowAbout();
		break;
	case ID_FILE_QUIT:
	case IDCANCEL:
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

void CMainWindow::LoadWindowRect()
{
	Framework::Win32::CRect windowRect(0, 0, 0, 0);
	windowRect.SetLeft(CAppConfig::GetInstance().GetPreferenceInteger(PREF_MAINWINDOW_RECT_LEFT));
	windowRect.SetTop(CAppConfig::GetInstance().GetPreferenceInteger(PREF_MAINWINDOW_RECT_TOP));
	windowRect.SetRight(CAppConfig::GetInstance().GetPreferenceInteger(PREF_MAINWINDOW_RECT_RIGHT));
	windowRect.SetBottom(CAppConfig::GetInstance().GetPreferenceInteger(PREF_MAINWINDOW_RECT_BOTTOM));
	bool maximized = CAppConfig::GetInstance().GetPreferenceBoolean(PREF_MAINWINDOW_MAXIMIZED);
	if(windowRect.Width() != 0 && windowRect.Height() != 0)
	{
		SetSizePosition(windowRect);
	}
	Show(maximized ? SW_SHOWMAXIMIZED : SW_SHOW);
}

void CMainWindow::SaveWindowRect()
{
	WINDOWPLACEMENT windowPlacement = {};
	windowPlacement.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(m_hWnd, &windowPlacement);
	CAppConfig::GetInstance().SetPreferenceInteger(PREF_MAINWINDOW_RECT_LEFT, windowPlacement.rcNormalPosition.left);
	CAppConfig::GetInstance().SetPreferenceInteger(PREF_MAINWINDOW_RECT_TOP, windowPlacement.rcNormalPosition.top);
	CAppConfig::GetInstance().SetPreferenceInteger(PREF_MAINWINDOW_RECT_RIGHT, windowPlacement.rcNormalPosition.right);
	CAppConfig::GetInstance().SetPreferenceInteger(PREF_MAINWINDOW_RECT_BOTTOM, windowPlacement.rcNormalPosition.bottom);
	CAppConfig::GetInstance().SetPreferenceBoolean(PREF_MAINWINDOW_MAXIMIZED, windowPlacement.showCmd == SW_SHOWMAXIMIZED);
}

void CMainWindow::Destroy()
{
	for(const auto& documentPair : m_documents)
	{
		documentPair.second->Destroy();
	}
	m_documents.clear();
	SaveWindowRect();
	CWindow::Destroy();
}

void CMainWindow::ShowWelcomePage()
{
	auto welcomePage = std::make_unique<CWelcomePage>(m_hWnd);
	InsertDocument(std::move(welcomePage));
}

void CMainWindow::ShowApperanceViewer()
{
	auto appearanceViewer = std::make_unique<CAppearanceViewer>(m_hWnd);
	InsertDocument(std::move(appearanceViewer));
}

void CMainWindow::ShowWorldViewer()
{
	auto worldViewer = std::make_unique<CWorldViewer>(m_hWnd);
	InsertDocument(std::move(worldViewer));
}

void CMainWindow::ShowGameDataSheetViewer()
{
	auto sheetViewer = std::make_unique<CSheetViewer>(m_hWnd, CSheetSchema::GAME_SCHEMA_FILE_ID);
	InsertDocument(std::move(sheetViewer));
}

void CMainWindow::ShowVarDataSheetViewer()
{
	auto sheetViewer = std::make_unique<CSheetViewer>(m_hWnd, CSheetSchema::VAR_SCHEMA_FILE_ID);
	InsertDocument(std::move(sheetViewer));
}

void CMainWindow::ShowAbout()
{
	CAboutWindow aboutWindow;
	aboutWindow.DoModal(m_hWnd);
}

void CMainWindow::InsertDocument(DocumentPtr&& documentWindow)
{
	int currentSelection = m_tabs.GetSelection();
	if(currentSelection != -1)
	{
		UnselectTab(currentSelection);
	}
	std::string documentName = "New Document";
	if(auto document = dynamic_cast<IDocument*>(documentWindow.get()))
	{
		documentName = document->GetName();
	}
	m_documents.insert(std::make_pair(m_nextDocumentId, std::move(documentWindow)));
	unsigned int tabIndex = m_tabs.InsertTab(string_cast<std::tstring>(documentName).c_str());
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
		const auto& documentWindow = documentIterator->second;
		if(auto document = dynamic_cast<IDocument*>(documentWindow.get()))
		{
			document->SetActive(true);
		}
		documentWindow->Show(SW_SHOW);
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
		const auto& documentWindow = documentIterator->second;
		documentWindow->Show(SW_HIDE);
		if(auto document = dynamic_cast<IDocument*>(documentWindow.get()))
		{
			document->SetActive(false);
		}
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
