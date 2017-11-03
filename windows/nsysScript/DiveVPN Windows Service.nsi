
!define APP_NAME "DiveVPN Windows Client"
!define COMP_NAME "Demlabs Inc."
!define WEB_SITE "http://demlabs.net/"
!define VERSION "00.00.1.00"
!define COPYRIGHT "Demlabs Inc. © 2017"
!define DESCRIPTION "DiveVPN Windows Client and Service"
!define INSTALLER_NAME "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\DiveVPNClient_setup.exe"
!define MAIN_APP_EXE "DapVpnGui.exe"
!define INSTALL_TYPE "SetShellVarContext all"
!define REG_ROOT "HKLM"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define REG_AUTO_RUN_PATH "Software\Microsoft\Windows\CurrentVersion\Run"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
;!define pkgdir "C:\Users\user\Documents\dap\DapVpnClient\MSWindows"
!define DIVESERVICE_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
######################################################################

;!define MUI_FINISHPAGE_TEXT_REBOOT "Now we need to reboot the system"
;!define MUI_FINISHPAGE_TEXT_REBOOTNOW "Reboot Now"
;!define MUI_FINISHPAGE_TEXT_REBOOTLATER "Reboot Later"

VIProductVersion  "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"

######################################################################

SetCompressor /SOLID Lzma
Name "${APP_NAME}"
Caption "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME}"
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$PROGRAMFILES\DiveVPNService"

######################################################################

!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

!insertmacro MUI_PAGE_WELCOME

!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "DiveVPN Windows Service"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SM_Folder
!endif

!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

######################################################################

Section -MainProgram
${INSTALL_TYPE}
SetOverwrite ifnewer
SetOutPath "$INSTDIR"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\capi.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\dasync.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\libcrypto-1_1.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\libgcc_s_dw2-1.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\libssl-1_1.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\libstdc++-6.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\libwinpthread-1.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\msvcr120.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\ossltest.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\padlock.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Charts.dll"
;File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Chartsd.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Core.dll"
;File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Cored.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Gui.dll"
;File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Guid.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Network.dll"
;File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Networkd.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Widgets.dll"
;File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Widgetsd.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Xml.dll"
;File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\Qt5Xmld.dll"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\binary\DapVpnGui.exe"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\binary\DapVpnService.exe"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\depends\tap-windows-9.21.2.exe"

SetOutPath "$INSTDIR\platforms"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\platforms\qwindows.dll"
;File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\platforms\qwindowsd.dll"
SetOutPath "$INSTDIR\imagefomats"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\imageformats\qgif.dll"
;File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\imageformats\qgifd.dll"
SetOutPath "$INSTDIR\sqldrivers"
File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\sqldrivers\qsqlite.dll"
;File "C:\Users\user\Documents\dap\DapVpnClient\MSWindows\dlls\sqldrivers\qsqlited.dll"

;MessageBox MB_ICONINFORMATION|MB_OK "For DiveVPN need install TUNTAP Driver" IDOK +2
MessageBox MB_YESNO|MB_ICONQUESTION "DiveVPNClient requires to install the TunTap driver. Install the TunTap driver?" IDYES true IDNO false
true:
    ExecWait "C:\Program Files (x86)\DiveVPNService\tap-windows-9.21.2.exe"
    Goto next
false:
    Goto next
next:    
;ExpandEnvStrings $0 %COMSPEC%
;ExecWait '"$0" /C "C:\Program Files (x86)\DiveVPNService\driver\addtap.bat"'
;MessageBox MB_ICONINFORMATION|MB_OK "For finished installation required reboot.$\nPlease, always reboot system manually letter!" IDOK +2 -- now not needed
SectionEnd

######################################################################

Section -Icons_Reg
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
CreateDirectory "$SMPROGRAMS\$SM_Folder"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
CreateDirectory "$SMPROGRAMS\DiveVPN Windows Service"
CreateShortCut "$SMPROGRAMS\DiveVPN Windows Service\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\DiveVPNService\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!endif

WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"
;WriteRegStr ${REG_ROOT} "${REG_AUTO_RUN_PATH}" "DiveVPNCLient" "C:\Program Files (x86)\DiveVPNService\DapVpnService.exe"

!ifdef WEB_SITE
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "URLInfoAbout" "${WEB_SITE}"
!endif

; registration our service
ExecWait '"Sc.exe" create DiveVPNService binPath="C:\Program Files (x86)\DiveVPNService\DapVpnService.exe" DisplayName="Dive VPN Service" type=own start=auto'

; start service
ExecWait '"Sc.exe" start "DiveVPNService"'

;MessageBox MB_YESNO|MB_ICONQUESTION "For finished installation required reboot.$\nDo you wish to reboot the system?" IDNO +2 --- -- now not needed
;  Reboot --- -- now not needed
SectionEnd

######################################################################

Section Uninstall
${INSTALL_TYPE}
ExecWait '"Sc.exe" stop "DiveVPNService"'
ExecWait '"Sc.exe" delete "DiveVPNService"'

Delete "$INSTDIR\capi.dll"
Delete "$INSTDIR\dasync.dll"
Delete "$INSTDIR\libcrypto-1_1.dll"
Delete "$INSTDIR\libgcc_s_dw2-1.dll"
Delete "$INSTDIR\libssl-1_1.dll"
Delete "$INSTDIR\libstdc++-6.dll"
Delete "$INSTDIR\libwinpthread-1.dll"
Delete "$INSTDIR\msvcr120.dll"
Delete "$INSTDIR\ossltest.dll"
Delete "$INSTDIR\padlock.dll"
Delete "$INSTDIR\Qt5Charts.dll"
;Delete "$INSTDIR\Qt5Chartsd.dll"
Delete "$INSTDIR\Qt5Core.dll"
;Delete "$INSTDIR\Qt5Cored.dll"
Delete "$INSTDIR\Qt5Gui.dll"
;Delete "$INSTDIR\Qt5Guid.dll"
Delete "$INSTDIR\Qt5Network.dll"
;Delete "$INSTDIR\Qt5Networkd.dll"
Delete "$INSTDIR\Qt5Widgets.dll"
;Delete "$INSTDIR\Qt5Widgetsd.dll"
Delete "$INSTDIR\Qt5Xml.dll"
;Delete "$INSTDIR\Qt5Xmld.dll"
Delete "$INSTDIR\DapVpnGui.exe"
Delete "$INSTDIR\DapVpnService.exe"
Delete "$INSTDIR\tap-windows-9.21.2.exe"
Delete "$INSTDIR\platforms\qwindows.dll"
;Delete "$INSTDIR\platforms\qwindowsd.dll"
Delete "$INSTDIR\imagefomats\qgif.dll"
;Delete "$INSTDIR\imagefomats\qgifd.dll"
Delete "$INSTDIR\sqldrivers\qsqlite.dll"
;Delete "$INSTDIR\sqldrivers\qsqlited.dll"
 
;RmDir "$INSTDIR\driver"

RmDir "$INSTDIR\platforms"
RmDir "$INSTDIR\imagefomats"
RmDir "$INSTDIR\sqldrivers"
 
Delete "$INSTDIR\uninstall.exe"
!ifdef WEB_SITE
Delete "$INSTDIR\${APP_NAME} website.url"
!endif

RmDir "$INSTDIR"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SM_Folder
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\$SM_Folder"
!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\DiveVPN Windows Service\${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\DiveVPN Windows Service\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\DiveVPN Windows Service"
!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################

