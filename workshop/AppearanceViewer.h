#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "win32/Edit.h"
#include "win32/Splitter.h"
#include "AppearanceViewerActorListPane.h"
#include "AppearanceViewerActorViewPane.h"

class CAppearanceViewer : public Framework::Win32::CDialog
{
public:
								CAppearanceViewer(HWND);
	virtual						~CAppearanceViewer();

protected:
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::unique_ptr<CAppearanceViewerActorListPane> ActorListPanePtr;
	typedef std::unique_ptr<CAppearanceViewerActorViewPane> ActorViewPanePtr;
	typedef std::unique_ptr<Framework::Win32::CSplitter> SplitterPtr;

	SplitterPtr					m_splitter;
	ActorListPanePtr			m_actorListPane;
	ActorViewPanePtr			m_actorViewPane;

	Framework::Win32::CEdit		m_modelIdEdit;
};
