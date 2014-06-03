#pragma once

#include "win32/Dialog.h"
#include "win32/ListView.h"
#include "../dataobjects/ResourceDefs.h"

class CSheetViewerSchemaPane : public Framework::Win32::CDialog
{
public:
	enum
	{
		NOTIFY_SELCHANGE = NM_LAST - 1
	};

	struct SELCHANGE_INFO : public NMHDR
	{
		std::string		sheetName;
		uint32			sheetId;
	};

									CSheetViewerSchemaPane(HWND);
	virtual							~CSheetViewerSchemaPane();

protected:
	long							OnSize(unsigned int, unsigned int, unsigned int) override;
	long							OnNotify(WPARAM, LPNMHDR) override;

private:
	void							PopulateSchemaView();

	void							OnSchemaViewItemChanged(NMLISTVIEW*);

	CSheetSchema					m_schema;

	Framework::Win32::CListView		m_schemaListView;
};
