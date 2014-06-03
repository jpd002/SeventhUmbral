#pragma once

#include <string>
#include "win32/Dialog.h"
#include "win32/ListView.h"
#include "SheetColumnsDefinition.h"

class CSheetViewerDataPane : public Framework::Win32::CDialog
{
public:
									CSheetViewerDataPane(HWND);
	virtual							~CSheetViewerDataPane();
	
	void							SetLanguage(const char*);
	void							SetSheetInfo(const std::string&, uint32);

protected:
	long							OnSize(unsigned int, unsigned int, unsigned int) override;
	long							OnNotify(WPARAM, LPNMHDR) override;

private:
	typedef std::vector<std::tstring> StringArray;

	void							PopulateDataView(uint32);

	void							CreateDataViewColumns(unsigned int);
	void							DeleteDataViewColumns();

	void							OnDataViewGetDispInfo(NMLVDISPINFO*);

	CSheetColumnsDefinition			m_sheetColumnsDefinitions;

	std::string						m_sheetName;
	uint32							m_sheetId = 0;
	const char*						m_language = "en";

	Framework::Win32::CListView		m_dataListView;
	std::vector<StringArray>		m_displayRows;
};
