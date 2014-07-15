#include "WelcomePage.h"
#include "resource.h"
#include "string_cast.h"

CWelcomePage::CWelcomePage(HWND parentWnd)
: CDialog(MAKEINTRESOURCE(IDD_WELCOMEPAGE), parentWnd)
{
	SetClassPtr();

	m_webBrowser = Framework::Win32::CWebBrowser(m_hWnd, GetClientRect());
	m_webBrowser.Navigate(_T("about:blank"));
	LoadPage();
}

CWelcomePage::~CWelcomePage()
{

}

std::string CWelcomePage::GetName() const
{
	return "Start";
}

long CWelcomePage::OnSize(unsigned int, unsigned int, unsigned int)
{
	m_webBrowser.SetSizePosition(GetClientRect());
	return TRUE;
}

void CWelcomePage::LoadPage()
{
	auto document = m_webBrowser.GetDocument();
	if(!document.IsEmpty())
	{
		auto page = GetPageContents();

		HRESULT result = S_OK;

		BSTR documentText = SysAllocString(page.c_str());

		{
			SAFEARRAYBOUND documentBounds = {};
			documentBounds.cElements = 1;
			documentBounds.lLbound = 0;
			auto documentArray = SafeArrayCreate(VT_VARIANT, 1, &documentBounds);
			{
				result = SafeArrayLock(documentArray);
				assert(SUCCEEDED(result));
				auto& elementVar = reinterpret_cast<VARIANT*>(documentArray->pvData)[0];
				elementVar.vt		= VT_BSTR;
				elementVar.bstrVal	= documentText;
				result = SafeArrayUnlock(documentArray);
				assert(SUCCEEDED(result));
			}
			result = document->write(documentArray);
			assert(SUCCEEDED(result));
			SafeArrayDestroy(documentArray);
		}

		SysFreeString(documentText);

		document->close();
	}
}

std::tstring CWelcomePage::GetPageContents()
{
	HRSRC pageResource = FindResource(NULL, MAKEINTRESOURCE(IDR_WELCOMEPAGE_CONTENTS), RT_RCDATA);
	assert(pageResource != NULL);
	HGLOBAL pageResourceHandle = LoadResource(NULL, pageResource);
	DWORD pageResourceSize = SizeofResource(NULL, pageResource);
	assert(pageResourceHandle != NULL);
	const char* pageResourceData = static_cast<const char*>(LockResource(pageResourceHandle));
	std::string pageText = std::string(pageResourceData, pageResourceData + pageResourceSize);
	UnlockResource(pageResourceHandle);
	return string_cast<std::tstring>(pageText);
}
