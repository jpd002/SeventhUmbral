#include "AppearanceViewerActorListPane.h"
#include "resource.h"
#include "string_format.h"
#include "../dataobjects/FileManager.h"
#include "StdStreamUtils.h"

#define VARWEP_SHEET_ID		0x03A70309

CAppearanceViewerActorListPane::CAppearanceViewerActorListPane(HWND parentWnd)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER_ACTORLISTPANE), parentWnd)
{
	auto sheetFilePath = CFileManager::GetResourcePath(VARWEP_SHEET_ID);
	auto sheetStream = Framework::CreateInputStdStream(sheetFilePath.native());
	auto varEquipSheet = CSheet::Create(sheetStream);
	m_varWepSheetData = CSheetData::Create(varEquipSheet, 0,
		[](uint32 fileId)
		{
			auto filePath = CFileManager::GetResourcePath(fileId);
			return new Framework::CStdStream(Framework::CreateInputStdStream(filePath.native()));
		}
	);

	SetClassPtr();
	m_actorListBox = Framework::Win32::CListBox(GetItem(IDC_APPEARANCEVIEWER_ACTORLIST));
	ScanActors();
}

CAppearanceViewerActorListPane::~CAppearanceViewerActorListPane()
{

}

long CAppearanceViewerActorListPane::OnCommand(unsigned short id, unsigned short cmd, HWND)
{
	switch(id)
	{
	case IDC_APPEARANCEVIEWER_ACTORLIST:
		switch(cmd)
		{
		case LBN_SELCHANGE:
			OnActorListItemChanged();
			break;
		}
		break;
	}
	return FALSE;
}

long CAppearanceViewerActorListPane::OnSize(unsigned int, unsigned int, unsigned int)
{
	auto clientRect = GetClientRect();
	m_actorListBox.SetSizePosition(clientRect);
	return FALSE;
}

void CAppearanceViewerActorListPane::OnActorListItemChanged()
{
	auto currentSelection = m_actorListBox.GetCurrentSelection();
	auto actorInfoIndex = m_actorListBox.GetItemData(currentSelection);
	const auto& actorInfo = m_actorInfos[actorInfoIndex];
	SELCHANGE_INFO selChangeInfo = {};
	selChangeInfo.code			= NOTIFY_SELCHANGE;
	selChangeInfo.hwndFrom		= m_hWnd;
	selChangeInfo.idFrom		= 0;
	selChangeInfo.baseModelId	= actorInfo.baseModelId;
	selChangeInfo.topModelId	= actorInfo.topModelId;
	SendMessage(GetParent(), WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selChangeInfo));
}

void CAppearanceViewerActorListPane::ScanActors()
{
	const char* charaFolder = "wep";
	auto gamePath = CFileManager::GetGamePath();
	auto actorBasePath = boost::filesystem::path(string_format("%s/client/chara/%s/", gamePath.string().c_str(), charaFolder));
	for(boost::filesystem::directory_iterator pathIterator(actorBasePath); 
		pathIterator != boost::filesystem::directory_iterator(); pathIterator++)
	{
		const auto& path = *pathIterator;
		if(!boost::filesystem::is_directory(path))
		{
			continue;
		}
		auto dirName = path.path().leaf().string();
		unsigned int actorId = 0;
		unsigned int scanCount = sscanf(dirName.c_str(), "w%d", &actorId);
		if(scanCount == 0) continue;
		auto subActorIds = ScanSubActors(path);
		for(const auto& subActorId : subActorIds)
		{
			auto variationIds = ScanActorVariations(actorId, subActorId);
			for(const auto& varId : variationIds)
			{
				ACTORINFO actorInfo;
				actorInfo.baseModelId = 40000 + actorId;
				actorInfo.topModelId = (subActorId << 10) | varId;
				m_actorInfos.push_back(actorInfo);
			}
		}
	}
	std::sort(std::begin(m_actorInfos), std::end(m_actorInfos));
	for(unsigned int i = 0; i < m_actorInfos.size(); i++)
	{
		const auto& actorInfo = m_actorInfos[i];
		uint32 modelId = actorInfo.baseModelId - 40000;
		uint32 subModelId = actorInfo.topModelId >> 10;
		uint32 variationId = actorInfo.topModelId & 0x3FF;
		auto actorDesc = string_format(_T("w%0.3de%0.3dv%0.3d"), modelId, subModelId, variationId);
		unsigned int itemIndex = m_actorListBox.AddString(actorDesc.c_str());
		m_actorListBox.SetItemData(itemIndex, i);
	}
}

std::vector<uint32> CAppearanceViewerActorListPane::ScanSubActors(const boost::filesystem::path& actorPath) const
{
	std::vector<uint32> subActorIds;
	auto subActorPath = actorPath / "equ";
	for(boost::filesystem::directory_iterator pathIterator(subActorPath); 
		pathIterator != boost::filesystem::directory_iterator(); pathIterator++)
	{
		const auto& path = *pathIterator;
		if(!boost::filesystem::is_directory(path))
		{
			continue;
		}
		auto dirName = path.path().leaf().string();
		unsigned int subActorId = 0;
		unsigned int scanCount = sscanf(dirName.c_str(), "e%d", &subActorId);
		if(scanCount == 0) continue;
		subActorIds.push_back(subActorId);
	}
	return subActorIds;
}

std::vector<uint32> CAppearanceViewerActorListPane::ScanActorVariations(uint32 actorId, uint32 subActorId) const
{
	std::vector<uint32> result;
	const auto& rows = m_varWepSheetData.GetRows();
	uint32 varWepIdMin = 1000000000 + (actorId * 1000000) + ((subActorId + 0) * 1000);
	uint32 varWepIdMax = 1000000000 + (actorId * 1000000) + ((subActorId + 1) * 1000);
	auto varWepLowerBound = rows.lower_bound(varWepIdMin);
	auto varWepUpperBound = rows.lower_bound(varWepIdMax);
	for(auto varWepIterator = varWepLowerBound; varWepIterator != varWepUpperBound; varWepIterator++)
	{
		uint32 varWepId = varWepIterator->first;
		uint32 varId = ((varWepId - 1000000000) % 1000000) % 1000;
		result.push_back(varId);
	}
	return result;
}
