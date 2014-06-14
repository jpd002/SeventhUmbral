#include "AppConfig.h"
#include "AppPreferences.h"
#include "AppearanceViewer.h"
#include "win32/Static.h"
#include "palleon\EmbedRemoteCall.h"
#include "resource.h"
#include "../dataobjects/FileManager.h"
#include "Base64.h"
#include "string_format.h"

CAppearanceViewer::CAppearanceViewer()
: CDialog(MAKEINTRESOURCE(IDD_APPEARANCEVIEWER))
{
	SetClassPtr();
	CreateActorViewer();
	m_actorListBox = Framework::Win32::CListBox(GetItem(IDC_APPEARANCEVIEWER_ACTORLIST));
	ScanActors();
}

CAppearanceViewer::~CAppearanceViewer()
{

}

void CAppearanceViewer::CreateActorViewer()
{
	auto placeholder = Framework::Win32::CStatic(GetItem(IDC_APPEARANCEVIEWER_RENDER_PLACEHOLDER));
	auto placeholderRect = placeholder.GetWindowRect();
	placeholderRect.ScreenToClient(m_hWnd);

	TCHAR moduleFileName[_MAX_PATH];
	GetModuleFileName(NULL, moduleFileName, _MAX_PATH);
	boost::filesystem::path actorViewerDataPath(moduleFileName);
	actorViewerDataPath.remove_leaf();
	actorViewerDataPath /= "actorviewer";

	m_embedControl = std::make_shared<Palleon::CWin32EmbedControl>(m_hWnd, placeholderRect,
		_T("ActorViewer.exe"), actorViewerDataPath.native().c_str());
	
	{
		Palleon::CEmbedRemoteCall rpc;
		rpc.SetMethod("SetGamePath");
		rpc.SetParam("Path", CAppConfig::GetInstance().GetPreferenceString(PREF_WORKSHOP_GAME_LOCATION));
		m_embedControl->ExecuteCommand(rpc.ToString());
	}
}

void CAppearanceViewer::ScanActors()
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

std::vector<uint32> CAppearanceViewer::ScanSubActors(const boost::filesystem::path& actorPath)
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

std::vector<uint32> CAppearanceViewer::ScanSubActorTextures(const boost::filesystem::path& subActorPath)
{
	std::vector<uint32> textureIds;
	//check for tex1 or tex2 folder and list all files in there
	return textureIds;
}

long CAppearanceViewer::OnCommand(unsigned short id, unsigned short cmd, HWND)
{
	switch(id)
	{
	case IDC_APPEARANCEVIEWER_ACTORLIST:
		switch(cmd)
		{
		case LBN_SELCHANGE:
			{
				auto currentSelection = m_actorListBox.GetCurrentSelection();
				auto actorInfoIndex = m_actorListBox.GetItemData(currentSelection);
				const auto& actorInfo = m_actorInfos[actorInfoIndex];
				SetActor(40000 + actorInfo.baseModelId, actorInfo.topModelId * 0x400);
			}
			break;
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return FALSE;
}

void CAppearanceViewer::SetActor(uint32 baseModelId, uint32 topModelId)
{
	try
	{
		Palleon::CEmbedRemoteCall rpc;
		rpc.SetMethod("SetActor");
		rpc.SetParam("BaseModelId", std::to_string(baseModelId));
		rpc.SetParam("TopModelId", std::to_string(topModelId));
		m_embedControl->ExecuteCommand(rpc.ToString());
	}
	catch(...)
	{
		
	}
}
