# Microsoft Developer Studio Project File - Name="QTSSRawFileModule" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=QTSSRawFileModule - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QTSSRawFileModule_Note.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QTSSRawFileModule_Note.mak" CFG="QTSSRawFileModule - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QTSSRawFileModule - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "QTSSRawFileModule - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QTSSRawFileModule - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "D:\zRS_BuildOutput_60\Debug_bin\QTSSRawFileModule"
# PROP Intermediate_Dir "D:\zRS_BuildOutput_60\Debug_obj\QTSSRawFileModule"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QTSSRAWFILEMODULE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "../../" /I "../../CommonUtilitiesLib/" /I "../../APICommonCode" /I "../../RTPMetaInfoLib/" /I "../../APIStubLib/" /FI"../../WinNTSupport/Win32header.h" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QTSSRAWFILEMODULE_EXPORTS" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"D:\zRS_BuildOutput_60\Debug_bin\QTSSRawFileModule\QTSSRawFileModuleD.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetDir=\zRS_BuildOutput_60\Debug_bin\QTSSRawFileModule
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy       /C       /R       /Y           "$(TargetDir)\*.*"        D:\RSDLL_60\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "QTSSRawFileModule - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "QTSSRawFileModule___Win32_Release"
# PROP BASE Intermediate_Dir "QTSSRawFileModule___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Output_Dir "D:\QTSS_Note\Release\QTSSRawFileModule"
# PROP Intermediate_Dir "D:\QTSS_Note\ReleaseObj\QTSSRawFileModule"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MTd /w /W0 /Gm /ZI /Od /I "../../" /I "../../CommonUtilitiesLib/" /I "../../APICommonCode" /I "../../RTPMetaInfoLib/" /I "../../APIStubLib/" /FI"../../WinNTSupport/Win32header.h" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QTSSRAWFILEMODULE_EXPORTS" /FD /GZ /c
# ADD CPP /nologo /MT /w /W0 /O1 /Ob2 /I "../../" /I "../../CommonUtilitiesLib/" /I "../../APICommonCode" /I "../../RTPMetaInfoLib/" /I "../../APIStubLib/" /FI"../../WinNTSupport/Win32header.h" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QTSSRAWFILEMODULE_EXPORTS" /FR /FD /c
# ADD BASE MTL /nologo /mktyplib203 /win32
# ADD MTL /nologo /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"D:\QTSS_Note\Release\WinNTSupport\dynmodules_disabled\QTSSRawFileModule.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetDir=\QTSS_Note\Release\WinNTSupport\dynmodules_disabled
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy       /C       /R       /Y           "$(TargetDir)\*.*"        D:\RSDLL_60\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "QTSSRawFileModule - Win32 Debug"
# Name "QTSSRawFileModule - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\OSMemory_Modules\OSMemory_Modules.cpp
# End Source File
# Begin Source File

SOURCE=.\QTSSRawFileModule.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
