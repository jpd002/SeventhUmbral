#pragma once

#define PARAMETER_UPDATE_GAME		_T("--update-game")

class CPatchProcess
{
public:
	static void			CheckGameVersionAndStartUpdate();
	static void			PatchGame();

private:
	static bool			IsGameUpToDate();
};
