#pragma once

#include <unordered_map>
#include "win32/Dialog.h"
#include "win32/ListBox.h"
#include "../../Palleon/include/palleon/win32/Win32EmbedControl.h"

class CAppearanceViewer : public Framework::Win32::CDialog
{
public:
								CAppearanceViewer();
	virtual						~CAppearanceViewer();

protected:
	long						OnCommand(unsigned short cmd, unsigned short, HWND) override;

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

	typedef std::shared_ptr<Palleon::CWin32EmbedControl> EmbedControlPtr;
	typedef std::vector<ACTORINFO> ActorInfoArray;

	void						CreateActorViewer();
	void						SetActor(uint32, uint32);
	void						ScanActors();
	std::vector<uint32>			ScanSubActors(const boost::filesystem::path&);
	std::vector<uint32>			ScanSubActorTextures(const boost::filesystem::path&);

	ActorInfoArray				m_actorInfos;
	EmbedControlPtr				m_embedControl;
	Framework::Win32::CListBox	m_actorListBox;
};
