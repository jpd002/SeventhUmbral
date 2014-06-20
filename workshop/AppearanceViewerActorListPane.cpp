#include "AppearanceViewerActorListPane.h"
#include "resource.h"
#include "string_format.h"
#include "../dataobjects/FileManager.h"

CAppearanceViewerActorListPane::CAppearanceViewerActorListPane(HWND parentWnd)
: Framework::Win32::CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER_ACTORLISTPANE), parentWnd)
{
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
	selChangeInfo.baseModelId	= 40000 + actorInfo.baseModelId;
	selChangeInfo.topModelId	= actorInfo.topModelId * 0x400;
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
			ACTORINFO actorInfo;
			actorInfo.baseModelId = actorId;
			actorInfo.topModelId = subActorId;
			m_actorInfos.push_back(actorInfo);
		}
	}
	std::sort(std::begin(m_actorInfos), std::end(m_actorInfos));
	for(unsigned int i = 0; i < m_actorInfos.size(); i++)
	{
		const auto& actorInfo = m_actorInfos[i];
		auto actorDesc = string_format(_T("w%0.3de%0.3d"), actorInfo.baseModelId, actorInfo.topModelId);
		unsigned int itemIndex = m_actorListBox.AddString(actorDesc.c_str());
		m_actorListBox.SetItemData(itemIndex, i);
	}
}

std::vector<uint32> CAppearanceViewerActorListPane::ScanSubActors(const boost::filesystem::path& actorPath)
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

std::vector<uint32> CAppearanceViewerActorListPane::ScanSubActorTextures(const boost::filesystem::path& subActorPath)
{
	std::vector<uint32> textureIds;
	//check for tex1 or tex2 folder and list all files in there
	return textureIds;
}
