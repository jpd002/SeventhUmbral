#include <Windows.h>
#include "SheetViewer.h"
#include "AppearanceViewer.h"

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR commandLine, int)
{
	CoInitialize(NULL);
	{
//		CAppearanceViewer appearanceViewer;
//		appearanceViewer.DoModal();
		CSheetViewer sheetViewer;
		sheetViewer.DoModal();
	}
	CoUninitialize();
	return 0;
}
