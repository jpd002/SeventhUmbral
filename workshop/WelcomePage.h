#pragma once

#include "win32/Dialog.h"
#include "win32/WebBrowser.h"
#include "Document.h"

class CWelcomePage : public Framework::Win32::CDialog, public IDocument
{
public:
									CWelcomePage(HWND);
	virtual							~CWelcomePage();

	std::string						GetName() const override;

protected:
	long							OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	void							LoadPage();
	std::tstring					GetPageContents();

	Framework::Win32::CWebBrowser	m_webBrowser;
};
