#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "win32/Edit.h"
#include "win32/Splitter.h"
#include "AppearanceViewerActorListPane.h"
#include "AppearanceViewerActorViewPane.h"
#include "AppearanceViewerActorInfoPane.h"
#include "Document.h"

class CAppearanceViewer : public Framework::Win32::CDialog, public IDocument
{
public:
								CAppearanceViewer(HWND);
	virtual						~CAppearanceViewer();

	virtual std::string			GetName() const override;

	virtual void				SetActive(bool) override;

protected:
	long						OnSize(unsigned int, unsigned int, unsigned int) override;
	long						OnNotify(WPARAM, LPNMHDR) override;

private:
	typedef std::unique_ptr<CAppearanceViewerActorListPane> ActorListPanePtr;
	typedef std::unique_ptr<CAppearanceViewerActorViewPane> ActorViewPanePtr;
	typedef std::unique_ptr<CAppearanceViewerActorInfoPane> ActorInfoPanePtr;
	typedef std::unique_ptr<Framework::Win32::CSplitter> SplitterPtr;

	void						OnActorListPaneSelChange(CAppearanceViewerActorListPane::SELCHANGE_INFO*);

	SplitterPtr					m_mainSplitter;
	SplitterPtr					m_subSplitter;

	ActorListPanePtr			m_actorListPane;
	ActorViewPanePtr			m_actorViewPane;
	ActorInfoPanePtr			m_actorInfoPane;
};
