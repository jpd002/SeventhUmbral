!include "MUI2.nsh"

!searchparse /file ./AppDef.h '#define APP_VERSIONSTR _T("' APP_VERSION '")'

; The name of the installer
Name "Seventh Umbral Workshop v${APP_VERSION}"

; The file to write
OutFile "sumworkshop-${APP_VERSION}.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Seventh Umbral Workshop"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\NSIS_SeventhUmbralWorkshop" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

;!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
!insertmacro MUI_LANGUAGE "English"
;--------------------------------

; Pages

;Page components
;Page directory
;Page instfiles

;UninstPage uninstConfirm
;UninstPage instfiles

;--------------------------------

!define REG_UNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\SeventhUmbralWorkshop"

; The stuff to install
Section "Seventh Umbral Workshop (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "..\build_win32\Win32\Release\Workshop.exe"
  File "..\build_win32\Win32\Release\ActorViewer.exe"
  File "..\build_win32\Win32\Release\PalleonEmbedProxy.dll"
  File "..\build_win32\Win32\Release\D3DCompiler_47.dll"
  File /r "..\build_win32\Win32\Release\actorviewer"
  File "changelog.html"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\NSIS_SeventhUmbralWorkshop "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "${REG_UNINSTALL}" "DisplayName" "Seventh Umbral Workshop"
  WriteRegStr HKLM "${REG_UNINSTALL}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "${REG_UNINSTALL}" "NoModify" 1
  WriteRegDWORD HKLM "${REG_UNINSTALL}" "NoRepair" 1
  WriteRegStr HKLM "${REG_UNINSTALL}" "DisplayIcon" '"$INSTDIR\Workshop.exe"'
  WriteRegStr HKLM "${REG_UNINSTALL}" "DisplayVersion" "${APP_VERSION}"
  WriteUninstaller "uninstall.exe"
  
SectionEnd

!include "vcredist2013_x86.nsh"

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  ; Set output path to the installation directory (this path is used when creating shortcuts).
  SetOutPath $INSTDIR

  CreateDirectory "$SMPROGRAMS\Seventh Umbral Workshop"
  CreateShortCut "$SMPROGRAMS\Seventh Umbral Workshop\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Seventh Umbral Workshop\Seventh Umbral Workshop.lnk" "$INSTDIR\Workshop.exe" "" "$INSTDIR\Workshop.exe" 0
  CreateShortCut "$SMPROGRAMS\Seventh Umbral Workshop\changelog.html.lnk" "$INSTDIR\changelog.html" "" "$INSTDIR\changelog.html" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "${REG_UNINSTALL}"
  DeleteRegKey HKLM SOFTWARE\NSIS_SeventhUmbralWorkshop

  ; Remove files and uninstaller
  RMDir /r "$INSTDIR\actorviewer"
  Delete "$INSTDIR\*"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Seventh Umbral Workshop\*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Seventh Umbral Workshop"
  RMDir "$INSTDIR"

SectionEnd
