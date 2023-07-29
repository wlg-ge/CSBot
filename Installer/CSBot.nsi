; CSBot - An IRC bot for publishing CS and NS games
; Copyright (C) 2001, 2002, 2003, 2004 Ilan Tayary ([NoCt]Yonatanz)
; Email: yonatanz@unitedadmins.com
; Website: http://www.csbot.org
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
;--------------------------------

; Define your application name
!define APPNAME "CSBot"
!define VERSION "v0.46b"
!define EXENAME "CSBot.exe"

!define APPNAMEANDVERSION "${APPNAME} ${VERSION}"
!define BASEKEY "Software\UnitedAdmins\${APPNAME}"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\UnitedAdmins\CSBot"
InstallDirRegKey HKLM "${BASEKEY}" "Install_Dir"
OutFile "CSBot-Setup-${VERSION}.exe"

; Modern interface settings
!include "MUI.nsh"

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\docs\COPYING.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

Section "CSBot (required)" Section1

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "..\Release\${EXENAME}"
	File "..\docs\COPYING.txt"
	File "..\docs\CSBot-Readme.txt"
	File "..\docs\Quick-Setup-Guide.txt"

	CreateDirectory "$SMPROGRAMS\CSBot"
	CreateShortCut "$SMPROGRAMS\CSBot\CSBot.lnk" "$INSTDIR\${EXENAME}" "" "$INSTDIR\${EXENAME}" 0 "" "" "Launch ${APPNAME}"
	CreateShortCut "$SMPROGRAMS\CSBot\ReadMe.lnk" "$INSTDIR\CSBot-Readme.txt" "" "" "" "" "" "Read the CSBot release notes"
	CreateShortCut "$SMPROGRAMS\CSBot\License Information.lnk" "$INSTDIR\COPYING.txt" "" "" "" "" "" "View the GNU General Public License"
	CreateShortCut "$SMPROGRAMS\CSBot\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0 "" "" "Remove CSBot from your computer"
SectionEnd

Section -FinishSection

	WriteRegStr HKLM "${BASEKEY}" "Install_Dir" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${Section1} ""
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "${BASEKEY}"
	DeleteRegKey HKCU "${BASEKEY}"

	; Delete self
	Delete "$INSTDIR\uninstall.exe"

	; Delete Shortcuts
	Delete "$SMPROGRAMS\CSBot\CSBot.lnk"
	Delete "$SMPROGRAMS\CSBot\ReadMe.lnk"
	Delete "$SMPROGRAMS\CSBot\License Information.lnk"
	Delete "$SMPROGRAMS\CSBot\Uninstall.lnk"

	; Clean up CSBot
	Delete "$INSTDIR\${EXENAME}"
	Delete "$INSTDIR\COPYING.txt"
	Delete "$INSTDIR\CSBot-Readme.txt"
	Delete "$INSTDIR\Quick-Setup-Guide.txt"

	; Remove remaining directories
	RMDir "$SMPROGRAMS\CSBot"
	RMDir "$INSTDIR\"

SectionEnd

; eof