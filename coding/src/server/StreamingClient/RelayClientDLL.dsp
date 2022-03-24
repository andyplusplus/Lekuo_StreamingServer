# Microsoft Developer Studio Project File - Name="RelayClientDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=RelayClientDLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RelayClientDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RelayClientDLL.mak" CFG="RelayClientDLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RelayClientDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RelayClientDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RelayClientDLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\RSRelease\Bin\RelayClientDLL\Release"
# PROP Intermediate_Dir "..\..\RSRelease\Obj\RelayClientDLL\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RELAYCLIENTDLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Include" /I "..\_Utility" /I "..\LogTrace" /I "./" /I "../" /I "../01.RelayServer/CommonUtilitiesLib/" /I "../01.RelayServer/QTFileLib/" /I "../01.RelayServer/RTPMetaInfoLib/" /I "../01.RelayServer/RTSPClientLib/" /I "../01.RelayServer/APICommonCode/" /I "../01.RelayServer/APIStubLib/" /I "../01.RelayServer/" /FI"../WinNTSupport/Win32header.h" /D "_I_RelayClientDLL" /D "_RELAY_CLIENT_EXPORT_" /D "_WZD_CODE_ENABLED_" /D "_MY_PRINTF_" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /U "_WZD_FILE_TEST_" /U "_WZD_CSEQ_TEST_" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 user32.lib ws2_32.lib winmm.lib /nologo /dll /machine:I386
# Begin Special Build Tool
TargetDir=\_Cv\020.Dev\20.myProject\01.StreamingServer\RSRelease\Bin\RelayClientDLL\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy                          /C                          /R                          /Y                              "$(TargetDir)\*.*"                           D:\RSClientDLL\                 	xcopy                                      /C                                      /R                                      /Y                                          "$(TargetDir)\*.*"                                       D:\RSDLL_60\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "RelayClientDLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\RSRelease\Bin\RelayClientDLL\Debug"
# PROP Intermediate_Dir "..\..\RSRelease\Obj\RelayClientDLL\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RELAYCLIENTDLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Include" /I "..\_Utility" /I "..\LogTrace" /I "./" /I "../" /I "../01.RelayServer/CommonUtilitiesLib/" /I "../01.RelayServer/QTFileLib/" /I "../01.RelayServer/RTPMetaInfoLib/" /I "../01.RelayServer/RTSPClientLib/" /I "../01.RelayServer/APICommonCode/" /I "../01.RelayServer/APIStubLib/" /I "../01.RelayServer/" /FI"../WinNTSupport/Win32header.h" /D "_I_RelayClientDLL" /D "_RELAY_CLIENT_EXPORT_" /D "_WZD_CODE_ENABLED_" /D "_MY_PRINTF_" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /U "_WZD_FILE_TEST_" /U "_WZD_CSEQ_TEST_" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib ws2_32.lib winmm.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"Libcmt.lib" /out:"..\RSRelease\Bin\RelayClientDLL\Debug/RelayClientDLLD.dll" /pdbtype:sept
# Begin Special Build Tool
TargetDir=\_Cv\020.Dev\20.myProject\01.StreamingServer\Development\RSRelease\Bin\RelayClientDLL\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy                                      /C                                      /R                                      /Y                                          "$(TargetDir)\*.*"                                       D:\RSClientDLL\                            	xcopy                                      /C                                      /R                                      /Y                                          "$(TargetDir)\*.*"                                       D:\RSDLL_60\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "RelayClientDLL - Win32 Release"
# Name "RelayClientDLL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Channel.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSessionPara.cpp
# End Source File
# Begin Source File

SOURCE=..\01.RelayServer\CommonUtilitiesLib\myPrintf.cpp

!IF  "$(CFG)" == "RelayClientDLL - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "RelayClientDLL - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RelayClient.cpp
# End Source File
# Begin Source File

SOURCE=.\RelayClientDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\Channel.h
# End Source File
# Begin Source File

SOURCE=.\ClientSessionPara.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Include\RelayClient.h
# End Source File
# Begin Source File

SOURCE=.\RelayClientDLL.rc
# End Source File
# End Group
# Begin Group "ClientSession"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\01.RelayServer\RTSPClientLib\ClientSession.cpp

!IF  "$(CFG)" == "RelayClientDLL - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "RelayClientDLL - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\01.RelayServer\RTSPClientLib\ClientSocket.cpp

!IF  "$(CFG)" == "RelayClientDLL - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "RelayClientDLL - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\01.RelayServer\OSMemoryLib\OSMemory.cpp

!IF  "$(CFG)" == "RelayClientDLL - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "RelayClientDLL - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\01.RelayServer\RTSPClientLib\RTSPClient.cpp

!IF  "$(CFG)" == "RelayClientDLL - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "RelayClientDLL - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
