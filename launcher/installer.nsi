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
  File "..\build_win32\Win32\Release\TDEmu.dll"
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

!define REDIST_NAME "Visual C++ 2013 Redistributable"
!define REDIST_SETUP_FILENAME "vcredist_x86.exe"

Section "${REDIST_NAME}" SEC_CRT2013

  SectionIn RO

  ClearErrors
  ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\VisualStudio\12.0\VC\Runtimes\x86" "Installed"
  IfErrors 0 +2
  DetailPrint "${REDIST_NAME} registry key was not found; assumed to be uninstalled."
  StrCmp $R0 "1" 0 +3
    DetailPrint "${REDIST_NAME} is already installed; skipping!"
    Goto done

  SetOutPath "$TEMP"

  DetailPrint "Downloading ${REDIST_NAME} Setup..."
  NSISdl::download /TIMEOUT=15000 "http://download.microsoft.com/download/2/E/6/2E61CFA4-993B-4DD4-91DA-3737CD5CD6E3/vcredist_x86.exe" "${REDIST_SETUP_FILENAME}"

  Pop $R0 ;Get the return value
  StrCmp $R0 "success" OnSuccess

  Pop $R0 ;Get the return value
  StrCmp $R0 "success" +2
    MessageBox MB_OK "Could not download ${REDIST_NAME} Setup."
    Goto done

OnSuccess:
  DetailPrint "Running ${REDIST_NAME} Setup..."
  ExecWait '"$TEMP\${REDIST_SETUP_FILENAME}" /q /norestart'
  DetailPrint "Finished ${REDIST_NAME} Setup"
  
  Delete "$TEMP\${REDIST_SETUP_FILENAME}"

done:
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  ; Set output path to the installation directory (this path is used when creating shortcuts).
  SetOutPath $INSTDIR

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
  Delete $INSTDIR\TDEmu.dll
  Delete $INSTDIR\servers.xml
  Delete $INSTDIR\changelog.html
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Seventh Umbral Launcher\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Seventh Umbral Launcher"
  RMDir "$INSTDIR"

SectionEnd
