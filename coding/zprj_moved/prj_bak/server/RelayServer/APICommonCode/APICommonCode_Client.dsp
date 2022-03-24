# Microsoft Developer Studio Project File - Name="APICommonCode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=APICommonCode - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "APICommonCode_Client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "APICommonCode_Client.mak" CFG="APICommonCode - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "APICommonCode - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "APICommonCode - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/", AAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "APICommonCode - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\RSRelease\Bin\RC_APICommonCode\Debug"
# PROP Intermediate_Dir "..\..\..\RSRelease\Obj\RC_APICommonCode\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I ".." /I "../CommonUtilitiesLib/" /I "../APIStubLib/" /I "../RTPMetaInfoLib/" /FI"../WinNTSupport/Win32header.h" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_MY_PRINTF_" /FR /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\RSRelease\Bin\RC_APICommonCode\Debug\APICommonCodeD.lib"
# Begin Special Build Tool
TargetDir=\_Cv\020.Dev\20.myProject\01.StreamingServer\RSRelease\Bin\RC_APICommonCode\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy            /C            /R            /Y                "$(TargetDir)\*.*"             D:\RSClientDLL\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "APICommonCode - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "APICommonCode___Win32_Release"
# PROP BASE Intermediate_Dir "APICommonCode___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\RSRelease\Bin\RC_APICommonCode\Release"
# PROP Intermediate_Dir "..\..\..\RSRelease\Obj\RC_APICommonCode\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Gm /ZI /Od /I ".." /I "../CommonUtilitiesLib/" /I "../APIStubLib/" /I "../RTPMetaInfoLib/" /FI"../WinNTSupport/Win32header.h" /D "WIN32" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# ADD CPP /nologo /MT /w /W0 /O1 /I ".." /I "../CommonUtilitiesLib/" /I "../APIStubLib/" /I "../RTPMetaInfoLib/" /FI"../WinNTSupport/Win32header.h" /D "WIN32" /D "_MBCS" /D "_LIB" /FR /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\RSRelease\Bin\RC_APICommonCode\Release\APICommonCode.lib"
# Begin Special Build Tool
TargetDir=\_Cv\020.Dev\20.myProject\01.StreamingServer\RSRelease\Bin\RC_APICommonCode\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy           /C           /R           /Y               "$(TargetDir)\*.*"            D:\RSClientDLL\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "APICommonCode - Win32 Debug"
# Name "APICommonCode - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\CommonUtilitiesLib\myPrintf.cpp
# End Source File
# Begin Source File

SOURCE=.\QTAccessFile.cpp
# End Source File
# Begin Source File

SOURCE=..\Server.tproj\QTSSDictionary.cpp
# End Source File
# Begin Source File

SOURCE=.\QTSSModuleUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\QTSSRollingLog.cpp
# End Source File
# Begin Source File

SOURCE=..\RTPMetaInfoLib\RTPMetaInfoPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\SDPSourceInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\SourceInfo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
