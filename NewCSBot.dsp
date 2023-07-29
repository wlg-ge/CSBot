# Microsoft Developer Studio Project File - Name="NewCSBot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=NewCSBot - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NewCSBot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NewCSBot.mak" CFG="NewCSBot - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NewCSBot - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NewCSBot - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "NewCSBot - Win32 Release with Traces" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NewCSBot - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "./" /I "irc/" /I "../hlds/" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MSGBOXES" /D "HTTP_WININET" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Shlwapi.lib Winmm.lib ws2_32.lib /nologo /subsystem:windows /map /machine:I386 /out:"Release/CSBot.exe"

!ELSEIF  "$(CFG)" == "NewCSBot - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "irc/" /I "../hlds/" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MSGBOXES" /D "HTTP_WININET" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Shlwapi.lib Winmm.lib ws2_32.lib /nologo /subsystem:windows /map /debug /machine:I386 /out:"Debug/CSBot.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "NewCSBot - Win32 Release with Traces"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NewCSBot___Win32_Release_with_Traces0"
# PROP BASE Intermediate_Dir "NewCSBot___Win32_Release_with_Traces0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release With Traces"
# PROP Intermediate_Dir "Release With Traces"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "./" /I "irc/" /I "../hlds/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "MSGBOXES" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "./" /I "irc/" /I "../hlds/" /D "NDEBUG" /D "TRACES" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MSGBOXES" /D "HTTP_WININET" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Shlwapi.lib Winmm.lib /nologo /subsystem:windows /map /machine:I386 /out:"Release/CSBot.exe"
# ADD LINK32 wininet.lib Shlwapi.lib Winmm.lib ws2_32.lib /nologo /subsystem:windows /map /machine:I386 /out:"Release With Traces/CSBotD.exe"

!ENDIF 

# Begin Target

# Name "NewCSBot - Win32 Release"
# Name "NewCSBot - Win32 Debug"
# Name "NewCSBot - Win32 Release with Traces"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\GHOutputWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLWindowing.cpp
# End Source File
# Begin Source File

SOURCE=.\IGCLockable.cpp
# End Source File
# Begin Source File

SOURCE=.\IRCOutputThread.cpp
# End Source File
# Begin Source File

SOURCE=.\LogFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCSBot.cpp
# End Source File
# Begin Source File

SOURCE=.\regex.c
# ADD CPP /D "STDC_HEADERS" /D "REGEX_MALLOC"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\RegexHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeedLimiter.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WonIDDBManager.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\GHOutputWnd.h
# End Source File
# Begin Source File

SOURCE=.\GIRLWindowing.h
# End Source File
# Begin Source File

SOURCE=.\IGCLockable.h
# End Source File
# Begin Source File

SOURCE=.\IRCOutputThread.h
# End Source File
# Begin Source File

SOURCE=.\LogFilter.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\NewCSBot.h
# End Source File
# Begin Source File

SOURCE=.\regex.h
# End Source File
# Begin Source File

SOURCE=.\RegexHelper.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\SpeedLimiter.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WonIDDBManager.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\default1.bin
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_rawl.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_rcon.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_tray.ico
# End Source File
# Begin Source File

SOURCE=.\res\NewCSBot.ico
# End Source File
# Begin Source File

SOURCE=.\NewCSBot.rc
# End Source File
# Begin Source File

SOURCE=.\res\NewCSBot.rc2
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=".\res\Toolbar-bw.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tray.ico
# End Source File
# End Group
# Begin Group "IRC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\irc\CrossThreadsMessagingDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\CrossThreadsMessagingDevice.h
# End Source File
# Begin Source File

SOURCE=.\irc\irc.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\irc.h
# End Source File
# Begin Source File

SOURCE=.\irc\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\socket.h
# End Source File
# End Group
# Begin Group "Views"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CSBotView.cpp
# End Source File
# Begin Source File

SOURCE=.\CSBotView.h
# End Source File
# Begin Source File

SOURCE=.\IRCView.cpp
# End Source File
# Begin Source File

SOURCE=.\IRCView.h
# End Source File
# Begin Source File

SOURCE=.\PlayerListView.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerListView.h
# End Source File
# Begin Source File

SOURCE=.\RawLogView.cpp
# End Source File
# Begin Source File

SOURCE=.\RawLogView.h
# End Source File
# Begin Source File

SOURCE=.\RconView.cpp
# End Source File
# Begin Source File

SOURCE=.\RconView.h
# End Source File
# Begin Source File

SOURCE=.\SchedView.cpp
# End Source File
# Begin Source File

SOURCE=.\SchedView.h
# End Source File
# End Group
# Begin Group "Misc dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConnectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConnectDlgCmds.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectDlgCmds.h
# End Source File
# Begin Source File

SOURCE=.\ConnectDlgGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectDlgGeneral.h
# End Source File
# Begin Source File

SOURCE=.\ConnectDlgReconnect.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectDlgReconnect.h
# End Source File
# Begin Source File

SOURCE=.\ConnectIRCDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectIRCDlg.h
# End Source File
# Begin Source File

SOURCE=.\ConnectIRCGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectIRCGeneral.h
# End Source File
# Begin Source File

SOURCE=.\ConnectIRCIdent.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectIRCIdent.h
# End Source File
# Begin Source File

SOURCE=.\CustomBanDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomBanDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditIRCMessageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditIRCMessageDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditPlayerListCommandDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditPlayerListCommandDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditTrustedPersonDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditTrustedPersonDlg.h
# End Source File
# Begin Source File

SOURCE=.\IRCColorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IRCColorDlg.h
# End Source File
# Begin Source File

SOURCE=.\NotifyUpdateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NotifyUpdateDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProfileName.cpp
# End Source File
# Begin Source File

SOURCE=.\ProfileName.h
# End Source File
# Begin Source File

SOURCE=.\SummonCmdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SummonCmdDlg.h
# End Source File
# Begin Source File

SOURCE=.\WonIDDatabaseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WonIDDatabaseDlg.h
# End Source File
# End Group
# Begin Group "Options dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OptionsGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsGeneral.h
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCIn.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCIn.h
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCMsgs.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCMsgs.h
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCMsgsColors.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCMsgsColors.h
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCMsgsMsgs.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCMsgsMsgs.h
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCOut.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsIRCOut.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPlayerList.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsPlayerList.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsSheet.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSummon.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsSummon.h
# End Source File
# End Group
# Begin Group "RconSched"

# PROP Default_Filter ""
# Begin Group "Dialgos"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EventDayDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventDayDlg.h
# End Source File
# Begin Source File

SOURCE=.\EventHourDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventHourDlg.h
# End Source File
# Begin Source File

SOURCE=.\EventMinuteDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventMinuteDlg.h
# End Source File
# Begin Source File

SOURCE=.\EventOnceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventOnceDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\RconEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\RconEvent.h
# End Source File
# Begin Source File

SOURCE=.\RconEvents.cpp
# End Source File
# Begin Source File

SOURCE=.\RconEvents.h
# End Source File
# End Group
# Begin Group "HLDS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GIRLUdpServer.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLUdpServer.h
# End Source File
# Begin Source File

SOURCE=.\hlinfos.h
# End Source File
# End Group
# Begin Group "Log Parsing Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GIRLDefs.h
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParser.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParser.h
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserCS.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserCS.h
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserDefault.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserDefault.h
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserDOD.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserDOD.h
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserManager.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserManager.h
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserNS.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserNS.h
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserTFC.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLLogParserTFC.h
# End Source File
# Begin Source File

SOURCE=.\GIRLLogReceiver.cpp
# End Source File
# Begin Source File

SOURCE=.\GIRLLogReceiver.h
# End Source File
# End Group
# Begin Group "Docs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\docs\COPYING.txt
# End Source File
# Begin Source File

SOURCE=".\docs\CSBot-Readme.txt"
# End Source File
# Begin Source File

SOURCE=".\docs\Quick-Setup-Guide.txt"
# End Source File
# End Group
# Begin Group "Installer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Installer\CSBot.nsi
# End Source File
# End Group
# End Target
# End Project
