# Microsoft Developer Studio Project File - Name="QTSSTestDevModule" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=QTSSTestDevModule - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QTSSTestDevModule.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QTSSTestDevModule.mak" CFG="QTSSTestDevModule - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QTSSTestDevModule - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "QTSSTestDevModule - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QTSSTestDevModule - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "D:\zRS_BuildOutput_60\Debug_bin\QTSSTestDevModule"
# PROP Intermediate_Dir "D:\zRS_BuildOutput_60\Debug_obj\QTSSTestDevModule"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_RELAY_SERVER_MODULE_" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "../../" /I "../../CommonUtilitiesLib/" /I "../../APICommonCode" /I "../../RTPMetaInfoLib/" /I "../../APIStubLib/" /I "../../../_Utility/" /I "../../../Include/" /FI"../../WinNTSupport/Win32header.h" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_RELAY_SERVER_MODULE_" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 RSUtilityD.lib RSDVRAdaptorD.lib Winmm.lib Ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"LibCMT.lib" /out:"D:\zRS_BuildOutput_60\Debug_bin\QTSSTestDevModule/QTSSTestDevModuleD.dll" /pdbtype:sept /libpath:"D:\RSDLL_60"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetDir=\QTSS_Note\Debug\QTSSTestDevModule
TargetName=QTSSTestDevModuleD
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy    /C    /R    /Y        "$(TargetDir)\*.*"     D:\RSDLL_60\	xcopy    /C    /R    /Y        "$(TargetDir)\$(TargetName).dll"  "C:\Program Files\VideoRelayServer\QTSSModules\"	xcopy    /C    /R    /Y        "$(TargetDir)\$(TargetName).dll"  "C:\Program Files\Darwin Streaming Server\QTSSModules\"	xcopy    /C    /R    /Y        "$(TargetDir)\$(TargetName).dll"  "D:\RSDLL_60\QTSSModules\"	xcopy    /C    /R    /Y        "$(TargetDir)\$(TargetName).pdb"  "D:\RSDLL_60\QTSSModules\"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "QTSSTestDevModule - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "QTSSTestDevModule___Win32_Release"
# PROP BASE Intermediate_Dir "QTSSTestDevModule___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Output_Dir "D:\QTSS_Note\Release\QTSSTestDevModule"
# PROP Intermediate_Dir "D:\QTSS_Note\ReleaseObj\QTSSTestDevModule"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MTd /w /W0 /Gm /ZI /Od /I "../../" /I "../../CommonUtilitiesLib/" /I "../../APICommonCode" /I "../../RTPMetaInfoLib/" /I "../../APIStubLib/" /FI"../../WinNTSupport/Win32header.h" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_RELAY_SERVER_MODULE_" /FD /GZ /c
# ADD CPP /nologo /MT /w /W0 /O1 /Ob2 /I "../../" /I "../../CommonUtilitiesLib/" /I "../../APICommonCode" /I "../../RTPMetaInfoLib/" /I "../../APIStubLib/" /I "../../../_Utility/" /I "../../../Include/" /FI"../../WinNTSupport/Win32header.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_RELAY_SERVER_MODULE_" /FR /FD /c
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
# ADD LINK32 RSUtility.lib RSDVRAdaptor.lib Winmm.lib Ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /dll /debug /machine:I386 /out:"D:\QTSS_Note\Release\WinNTSupport\dynmodules_disabled\QTSSTestDevModule.dll" /pdbtype:sept /libpath:"D:\RSDLL_60"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetDir=\QTSS_Note\Release\WinNTSupport\dynmodules_disabled
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy                                        /C                                        /R                                        /Y                                            "$(TargetDir)\*.*"                                         D:\RSDLL_60\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "QTSSTestDevModule - Win32 Debug"
# Name "QTSSTestDevModule - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ConnectionElem.cpp
# End Source File
# Begin Source File

SOURCE=.\ModuleAdaptor.cpp
# End Source File
# Begin Source File

SOURCE=.\ModuleFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonUtilitiesLib\myPrintf.cpp
# End Source File
# Begin Source File

SOURCE=..\OSMemory_Modules\OSMemory_Modules.cpp
# End Source File
# Begin Source File

SOURCE=.\QTSSLogging.cpp
# End Source File
# Begin Source File

SOURCE=.\QTSSTestDevModule.cpp
# End Source File
# Begin Source File

SOURCE=.\StreamPackage.cpp
# End Source File
# Begin Source File

SOURCE=.\TestDevMoudleImp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConnectionElem.h
# End Source File
# Begin Source File

SOURCE=.\ModuleAdaptor.h
# End Source File
# Begin Source File

SOURCE=.\ModuleFunction.h
# End Source File
# Begin Source File

SOURCE=.\QTSSLogging.h
# End Source File
# Begin Source File

SOURCE=.\QTSSTestDevModule.h
# End Source File
# Begin Source File

SOURCE=.\StreamPackage.h
# End Source File
# Begin Source File

SOURCE=.\TestDevMoudleImp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\QTSSTestDevModule.rc
# End Source File
# End Group
# Begin Group "External"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\RSClientElem.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\RsRTSPUtility.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
