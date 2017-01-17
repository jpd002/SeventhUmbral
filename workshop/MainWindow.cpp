#include "string_cast.h"
#include "MainWindow.h"
#include "resource.h"
#include "WelcomePage.h"
#include "SettingsPage.h"
#include "SheetViewer.h"
#include "ZoneEditor.h"
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

	{
		int smallIconSizeX = GetSystemMetrics(SM_CXSMICON);
		int smallIconSizeY = GetSystemMetrics(SM_CYSMICON);
		int bigIconSizeX = GetSystemMetrics(SM_CXICON);
		int bigIconSizeY = GetSystemMetrics(SM_CYICON);

		HICON smallIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, smallIconSizeX, smallIconSizeY, 0));
		HICON bigIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, bigIconSizeX, bigIconSizeY, 0));

		SetIcon(ICON_SMALL, smallIcon);
		SetIcon(ICON_BIG, bigIcon);
	}

	ShowWelcomePage();

	LoadWindowRect();

	m_accelerators = LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINWINDOW_ACCELERATORS));
}

CMainWindow::~CMainWindow()
{

}

void CMainWindow::DoMessageLoop()
{
	while(IsWindow())
	{
		MSG m;
		GetMessage(&m, NULL, NULL, NULL);
		if(!TranslateAccelerator(m_hWnd, m_accelerators, &m))
		{
			if(!IsDialogMessage(m_hWnd, &m))
			{
				TranslateMessage(&m);
				DispatchMessage(&m);
			}
		}
	}
}

long CMainWindow::OnCommand(unsigned short id, unsigned short cmd, HWND)
{
	switch(id)
	{
	case ID_FILE_SAVE:
		SaveCurrentDocument();
		break;
	case ID_FILE_QUIT:
		Destroy();
		break;
	case ID_MAINMENU_MODIFY_ZONE:
		ShowZoneEditor();
		break;
	case ID_MAINMENU_INSPECT_WEAPONS:
		ShowAppearanceViewer();
		break;
	case ID_MAINMENU_INSPECT_WORLD_GRIDANIAINN:
		ShowWorldViewer(0xA09B0000);
		break;
	case ID_MAINMENU_INSPECT_WORLD_LIMSALOMINSAINN:
		ShowWorldViewer(0xA09B0001);
		break;
	case ID_MAINMENU_INSPECT_WORLD_ULDAHINN:
		ShowWorldViewer(0xA09B0002);
		break;
	case ID_MAINMENU_INSPECT_GAMEDATASHEETS:
		ShowSheetViewer(CSheetSchema::GAME_SCHEMA_FILE_ID);
		break;
	case ID_MAINMENU_INSPECT_VARDATASHEETS:
		ShowSheetViewer(CSheetSchema::VAR_SCHEMA_FILE_ID);
		break;
	case ID_MAINMENU_SETTINGS_GENERALSETTINGS:
		InsertDocument(std::move(std::make_unique<CSettingsPage>(m_hWnd)));
		break;
	case ID_MAINMENU_HELP_ABOUT:
		ShowAbout();
		break;
	case ID_TABCONTEXT_CLOSETAB:
		if(m_tabs.GetItemCount())
		{
			if(m_tabContextMenuSelection == -1)
			{
				CloseTab(m_tabs.GetSelection());
			}
			else
			{
				CloseTab(m_tabContextMenuSelection);
			}
		}
		break;
	}
	return FALSE;
}

long CMainWindow::OnSysCommand(unsigned int cmd, LPARAM)
{
	switch(cmd)
	{
	case SC_CLOSE:
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
		case NM_RCLICK:
			OnTabRightClick();
			break;
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

unsigned int CMainWindow::Destroy()
{
	for(const auto& documentPair : m_documents)
	{
		documentPair.second->Destroy();
	}
	m_documents.clear();
	SaveWindowRect();
	return CWindow::Destroy();
}

void CMainWindow::SaveCurrentDocument()
{
	unsigned int selection = m_tabs.GetSelection();
	auto documentId = m_tabs.GetTabData(selection);
	auto documentIterator = m_documents.find(documentId);
	assert(documentIterator != m_documents.end());
	if(documentIterator == m_documents.end()) return;
	auto documentWindow = documentIterator->second.get();
	auto document = dynamic_cast<IDocument*>(documentWindow);
	if(document)
	{
		document->Save();
	}
}

void CMainWindow::ShowWelcomePage()
{
	auto welcomePage = std::make_unique<CWelcomePage>(m_hWnd);
	InsertDocument(std::move(welcomePage));
}

void CMainWindow::ShowZoneEditor()
{
	auto document = std::make_unique<CZoneEditor>(m_hWnd, 0xA09B0000);
	InsertDocument(std::move(document));
}

void CMainWindow::ShowAppearanceViewer()
{
	auto appearanceViewer = std::make_unique<CAppearanceViewer>(m_hWnd);
	InsertDocument(std::move(appearanceViewer));
}

void CMainWindow::ShowWorldViewer(uint32 mapId)
{
	auto worldViewer = std::make_unique<CWorldViewer>(m_hWnd, mapId);
	InsertDocument(std::move(worldViewer));
}

void CMainWindow::ShowSheetViewer(uint32 sheetSchemaId)
{
	auto sheetViewer = std::make_unique<CSheetViewer>(m_hWnd, sheetSchemaId);
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
	uint32 documentId = m_nextDocumentId;
	std::string documentName = "New Document";
	auto document = dynamic_cast<IDocument*>(documentWindow.get());
	if(document)
	{
		documentName = document->GetName();
	}
	m_documents.insert(std::make_pair(documentId, std::move(documentWindow)));
	unsigned int tabIndex = m_tabs.InsertTab(string_cast<std::tstring>(documentName).c_str());
	m_tabs.SetTabData(tabIndex, documentId);
	m_tabs.SetSelection(tabIndex);
	if(document)
	{
		document->StateChanged.connect([this] () { OnDocumentStateChanged(); });
	}
	SelectTab(tabIndex);
	m_nextDocumentId++;
}

void CMainWindow::OnDocumentStateChanged()
{
	//We assume that only the current document's state can be changed
	auto tabIndex = m_tabs.GetSelection();
	assert(tabIndex != -1);
	auto documentId = m_tabs.GetTabData(tabIndex);
	auto documentIterator = m_documents.find(documentId);
	assert(documentIterator != std::end(m_documents));
	if(documentIterator == std::end(m_documents)) return;
	auto document = dynamic_cast<IDocument*>(documentIterator->second.get());
	assert(document);
	if(document == nullptr) return;
	auto documentName = document->GetName();
	if(document->GetDirty())
	{
		documentName += "*";
	}
	m_tabs.SetTabText(tabIndex, string_cast<std::tstring>(documentName).c_str());
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

void CMainWindow::CloseTab(int tabIndex)
{
	if(m_tabs.GetSelection() == tabIndex)
	{
		UnselectTab(tabIndex);
	}
	uint32 documentId = m_tabs.GetTabData(tabIndex);
	auto documentIterator = m_documents.find(documentId);
	assert(documentIterator != std::end(m_documents));
	if(documentIterator != std::end(m_documents))
	{
		documentIterator->second->Destroy();
		m_documents.erase(documentIterator);
	}
	m_tabs.DeleteTab(tabIndex);
	if(m_tabs.GetItemCount() > 0)
	{
		m_tabs.SetSelection(0);
		SelectTab(0);
	}
}

void CMainWindow::OnTabRightClick()
{
	POINT cursorScreenPos = {};
	GetCursorPos(&cursorScreenPos);

	POINT cursorClientPos = cursorScreenPos;
	ScreenToClient(m_tabs.m_hWnd, &cursorClientPos);
	m_tabContextMenuSelection = m_tabs.HitTest(cursorClientPos.x, cursorClientPos.y);
	if(m_tabContextMenuSelection == -1) return;

	HMENU contextMenuRoot = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_TABCONTEXT));
	HMENU contextMenu = GetSubMenu(contextMenuRoot, 0);
	TrackPopupMenu(contextMenu, 0, cursorScreenPos.x, cursorScreenPos.y, 0, m_hWnd, nullptr);
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
