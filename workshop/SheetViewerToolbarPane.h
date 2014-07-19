#pragma once

#include <map>
#include "win32/Dialog.h"
#include "win32/ToolBar.h"

class CSheetViewerToolbarPane : public Framework::Win32::CDialog
{
public:
	enum LANGUAGE
	{
		LANGUAGE_JAPANESE = 0xBEEF,
		LANGUAGE_ENGLISH,
		LANGUAGE_FRENCH,
		LANGUAGE_GERMAN,
	};

	enum
	{
		NOTIFY_LANGUAGE_CHANGE = NM_LAST - 1
	};

	struct LANGUAGE_CHANGE_INFO : public NMHDR
	{
		LANGUAGE	language;
	};

										CSheetViewerToolbarPane(HWND);
	virtual								~CSheetViewerToolbarPane();

	LANGUAGE							GetSelectedLanguage() const;

protected:
	long								OnSize(unsigned int, unsigned int, unsigned int) override;
	long								OnCommand(unsigned short, unsigned short, HWND) override;
	long								OnNotify(WPARAM, NMHDR*) override;

private:
	struct LANGUAGE_TOOLBAR_ITEM
	{
		LANGUAGE_TOOLBAR_ITEM(const TCHAR* buttonText = nullptr, const TCHAR* toolTipText = nullptr)
			: buttonText(buttonText), toolTipText(toolTipText)
		{
		
		}

		const TCHAR*	buttonText;
		const TCHAR*	toolTipText;
	};

	typedef std::map<LANGUAGE, LANGUAGE_TOOLBAR_ITEM> LanguageToolBarItemMap;

	Framework::Win32::CToolBar			m_toolbar;
	static const LanguageToolBarItemMap	m_languageToolBarItems;
	LANGUAGE							m_selectedLanguage = LANGUAGE_JAPANESE;
};
