#pragma once

#include "win32/Dialog.h"
#include "win32/ListBox.h"
#include "../dataobjects/SheetData.h"

class CAppearanceViewerActorListPane : public Framework::Win32::CDialog
{
public:
	enum
	{
		NOTIFY_SELCHANGE = NM_LAST - 1
	};

	struct SELCHANGE_INFO : public NMHDR
	{
		uint32		baseModelId;
		uint32		topModelId;
	};

									CAppearanceViewerActorListPane(HWND);
	virtual							~CAppearanceViewerActorListPane();

protected:
	long							OnCommand(unsigned short id, unsigned short cmd, HWND) override;
	long							OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	struct ACTORINFO
	{
		uint32		baseModelId = 0;
		uint32		topModelId = 0;

		bool operator < (const ACTORINFO& rhs) const
		{
			if(baseModelId == rhs.baseModelId)
			{
				return topModelId < rhs.topModelId;
			}
			else
			{
				return baseModelId < rhs.baseModelId;
			}
		}
	};

	typedef std::vector<ACTORINFO> ActorInfoArray;

	void							OnActorListItemChanged();

	void							ScanActors();
	std::vector<uint32>				ScanSubActors(const boost::filesystem::path&) const;
	std::vector<uint32>				ScanActorVariations(uint32, uint32) const;

	ActorInfoArray					m_actorInfos;
	Framework::Win32::CListBox		m_actorListBox;
	CSheetData						m_varWepSheetData;
};
