#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "win32/Splitter.h"
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
	long								OnCommand(unsigned short, unsigned short, HWND) override;
	long								OnNotify(WPARAM, LPNMHDR) override;
	long								OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::unordered_map<uint32, const char*> LanguageMenuValueMap;
	typedef std::unique_ptr<Framework::Win32::CSplitter> SplitterPtr;
	typedef std::unique_ptr<CSheetViewerSchemaPane> SchemaPanePtr;
	typedef std::unique_ptr<CSheetViewerDataPane> DataPanePtr;

	void								SetLanguage(uint32);
	void								OnSchemaPaneSelChange(CSheetViewerSchemaPane::SELCHANGE_INFO*);

	static const LanguageMenuValueMap	m_languageMenuValues;

	uint32								m_schemaId = 0;
	SplitterPtr							m_splitter;
	SchemaPanePtr						m_schemaPane;
	DataPanePtr							m_dataPane;
};
