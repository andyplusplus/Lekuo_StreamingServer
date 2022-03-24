# Microsoft Developer Studio Project File - Name="RSUtility" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=RSUtility - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RSUtility.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RSUtility.mak" CFG="RSUtility - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RSUtility - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "RSUtility - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RSUtility - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\RSRelease\Bin\_Utility\Release"
# PROP Intermediate_Dir "..\..\RSRelease\Obj\_Utility\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Include" /I "../01.RelayServer/CommonUtilitiesLib/" /I "../01.RelayServer/APICommonCode/" /I "../01.RelayServer/" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
TargetDir=\_Cv\020.Dev\20.myProject\01.StreamingServer\RSRelease\Bin\_Utility\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy                        /C                        /R                        /Y                            "$(TargetDir)\*.*"                         D:\RSDLL_60\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "RSUtility - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\RSRelease\Bin\_Utility\Debug"
# PROP Intermediate_Dir "..\..\RSRelease\Obj\_Utility\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "..\Include" /I "../01.RelayServer/CommonUtilitiesLib/" /I "../01.RelayServer/APICommonCode/" /I "../01.RelayServer/" /D "_I_RSUtility" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\RSRelease\Bin\_Utility\Debug\RSUtilityD.lib"
# Begin Special Build Tool
TargetDir=\_Cv\020.Dev\20.myProject\01.StreamingServer\RSRelease\Bin\_Utility\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy                        /C                        /R                        /Y                            "$(TargetDir)\*.*"                         D:\RSDLL_60\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "RSUtility - Win32 Release"
# Name "RSUtility - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\IniReader.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\IniReader2.cpp
# End Source File
# Begin Source File

SOURCE=.\RSClientElem.cpp
# End Source File
# Begin Source File

SOURCE=.\RSMemroy.cpp

!IF  "$(CFG)" == "RSUtility - Win32 Release"

!ELSEIF  "$(CFG)" == "RSUtility - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RSOSMutex.cpp
# End Source File
# Begin Source File

SOURCE=.\RsOSQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\RsRTPUtility.cpp
# End Source File
# Begin Source File

SOURCE=.\RsRTSPUtility.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\IniReader2.h
# End Source File
# Begin Source File

SOURCE=.\RSClientElem.h
# End Source File
# Begin Source File

SOURCE=..\Include\RSMemroy.h
# End Source File
# Begin Source File

SOURCE=.\RSOSMutex.h
# End Source File
# Begin Source File

SOURCE=.\RsOSQueue.h
# End Source File
# Begin Source File

SOURCE=.\RsRTPUtility.h
# End Source File
# Begin Source File

SOURCE=.\RsRTSPUtility.h
# End Source File
# Begin Source File

SOURCE=..\Include\RSTag.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
