# Microsoft Developer Studio Project File - Name="SafeDynamicModuleStdLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SafeDynamicModuleStdLib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SafeDynamicModuleStdLib_Note.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SafeDynamicModuleStdLib_Note.mak" CFG="SafeDynamicModuleStdLib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SafeDynamicModuleStdLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SafeDynamicModuleStdLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SafeDynamicModuleStdLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "D:\zRS_BuildOutput_60\Debug_bin\SafeDynamicModuleStdLib"
# PROP Intermediate_Dir "D:\zRS_BuildOutput_60\Debug_obj\SafeDynamicModuleStdLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "../" /I "../APIStubLib/" /I "../CommonUtilitiesLib/" /FI"../WinNTSupport/Win32header.h" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"D:\zRS_BuildOutput_60\Debug_bin\SafeDynamicModuleStdLib\SafeDynamicModuleStdLibD.lib"
# Begin Special Build Tool
TargetDir=\QTSS_Note\Debug\SafeDynamicModuleStdLib
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy     /C     /R     /Y         "$(TargetDir)\*.*"      D:\RSDLL_60\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SafeDynamicModuleStdLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SafeDynamicModuleStdLib___Win32_Release"
# PROP BASE Intermediate_Dir "SafeDynamicModuleStdLib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "D:\QTSS_Note\Release\SafeDynamicModuleStdLib"
# PROP Intermediate_Dir "D:\QTSS_Note\ReleaseObj\SafeDynamicModuleStdLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /I "../../" /I "../../APIStubLib/" /I "../../CommonUtilitiesLib/" /FI"../WinNTSupport/Win32header.h" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MT /w /W0 /O1 /Ob2 /I "../" /I "../APIStubLib/" /I "../CommonUtilitiesLib/" /FI"../WinNTSupport/Win32header.h" /D "WIN32" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"SafeDynamicModuleStdLib\SafeDynamicModuleStdLib.lib"
# ADD LIB32 /nologo /out:"D:\QTSS_Note\Release\WinNTSupport\SafeDynamicModuleStdLib.lib"
# Begin Special Build Tool
TargetDir=\QTSS_Note\Release\WinNTSupport
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy     /C     /R     /Y         "$(TargetDir)\*.*"      D:\RSDLL_60\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "SafeDynamicModuleStdLib - Win32 Debug"
# Name "SafeDynamicModuleStdLib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DynamicModuleStdLib.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\CommonUtilitiesLib\SafeStdLib.h
# End Source File
# End Group
# End Target
# End Project
