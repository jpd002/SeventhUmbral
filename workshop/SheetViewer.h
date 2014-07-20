#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "win32/Splitter.h"
#include "win32/ToolBar.h"
#include "SheetViewerSchemaPane.h"
#include "SheetViewerDataPane.h"
#include "Document.h"

class CSheetViewer : public Framework::Win32::CDialog, public IDocument
{
public:
										CSheetViewer(HWND, uint32);
	virtual								~CSheetViewer();

	virtual std::string					GetName() const override;

protected:
	long								OnNotify(WPARAM, LPNMHDR) override;
	long								OnCommand(unsigned short, unsigned short, HWND) override;
	long								OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	enum LANGUAGE
	{
		LANGUAGE_JAPANESE = 0xBEEF,
		LANGUAGE_ENGLISH,
		LANGUAGE_FRENCH,
		LANGUAGE_GERMAN,
	};

	struct LANGUAGE_TOOLBAR_ITEM
	{
		LANGUAGE_TOOLBAR_ITEM(const TCHAR* buttonText = nullptr, const TCHAR* toolTipText = nullptr, const char* languageCode = nullptr)
			: buttonText(buttonText), toolTipText(toolTipText), languageCode(languageCode)
		{
		
		}

		const TCHAR*	buttonText;
		const TCHAR*	toolTipText;
		const char*		languageCode;
	};

	typedef std::map<LANGUAGE, LANGUAGE_TOOLBAR_ITEM> LanguageToolBarItemMap;
	typedef std::unique_ptr<Framework::Win32::CSplitter> SplitterPtr;
	typedef std::unique_ptr<CSheetViewerSchemaPane> SchemaPanePtr;
	typedef std::unique_ptr<CSheetViewerDataPane> DataPanePtr;

	void								SetLanguage(LANGUAGE);
	void								OnSchemaPaneSelChange(CSheetViewerSchemaPane::SELCHANGE_INFO*);

	static const LanguageToolBarItemMap	m_languageToolBarItems;

	uint32								m_schemaId = 0;
	LANGUAGE							m_selectedLanguage = LANGUAGE_JAPANESE;

	Framework::Win32::CToolBar			m_toolbar;
	SplitterPtr							m_splitter;
	SchemaPanePtr						m_schemaPane;
	DataPanePtr							m_dataPane;
};
