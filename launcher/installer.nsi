!include "MUI2.nsh"

!searchparse /file ./AppDef.h '#define APP_VERSIONSTR _T("' APP_VERSION '")'

; The name of the installer
Name "Seventh Umbral Launcher v${APP_VERSION}"

; The file to write
OutFile "sumlauncher-${APP_VERSION}.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Seventh Umbral Launcher"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\NSIS_SeventhUmbralLauncher" "Install_Dir"

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

!define REG_UNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\SeventhUmbralLauncher"

; The stuff to install
Section "Seventh Umbral Launcher (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "..\build_win32\Win32\Release\Launcher.exe"
  File "changelog.html"
  File "servers.xml"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\NSIS_SeventhUmbralLauncher "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "${REG_UNINSTALL}" "DisplayName" "Seventh Umbral Launcher"
  WriteRegStr HKLM "${REG_UNINSTALL}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "${REG_UNINSTALL}" "NoModify" 1
  WriteRegDWORD HKLM "${REG_UNINSTALL}" "NoRepair" 1
  WriteRegStr HKLM "${REG_UNINSTALL}" "DisplayIcon" '"$INSTDIR\Launcher.exe"'
  WriteRegStr HKLM "${REG_UNINSTALL}" "DisplayVersion" "${APP_VERSION}"
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Seventh Umbral Launcher"
  CreateShortCut "$SMPROGRAMS\Seventh Umbral Launcher\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Seventh Umbral Launcher\Seventh Umbral Launcher.lnk" "$INSTDIR\Launcher.exe" "" "$INSTDIR\Launcher.exe" 0
  CreateShortCut "$SMPROGRAMS\Seventh Umbral Launcher\changelog.html.lnk" "$INSTDIR\changelog.html" "" "$INSTDIR\changelog.html" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "${REG_UNINSTALL}"
  DeleteRegKey HKLM SOFTWARE\NSIS_SeventhUmbralLauncher

  ; Remove files and uninstaller
  Delete $INSTDIR\Launcher.exe
  Delete $INSTDIR\servers.xml
  Delete $INSTDIR\changelog.html
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Seventh Umbral Launcher\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Seventh Umbral Launcher"
  RMDir "$INSTDIR"

SectionEnd
