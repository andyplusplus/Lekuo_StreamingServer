# Microsoft Developer Studio Project File - Name="_RelayClientDLLTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=_RelayClientDLLTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "_RelayClientDLLTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "_RelayClientDLLTest.mak" CFG="_RelayClientDLLTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "_RelayClientDLLTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "_RelayClientDLLTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "_RelayClientDLLTest - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\RSRelease\Bin\_RelayClientDLLTest\Release"
# PROP Intermediate_Dir "..\..\..\RSRelease\Obj\_RelayClientDLLTest\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\externals\Includess\CPPUnit" /I "..\Include" /D "_I__RelayClientDLLTest" /D "_I_Test" /D "_I_Test_GUI_Suite" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
TargetDir=\_Cv\020.Dev\20.myProject\01.StreamingServer\RSRelease\Bin\_RelayClientDLLTest\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy                                /C                                /R                                /Y                                    "$(TargetDir)\*.*"                                 D:\RSDLL_60\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "_RelayClientDLLTest - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\RSRelease\Bin\_RelayClientDLLTest\Debug"
# PROP Intermediate_Dir "..\..\..\RSRelease\Obj\_RelayClientDLLTest\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\externals\Includess\CPPUnit" /I "..\Include" /D "_I__RelayClientDLLTest" /D "_I_Test" /D "_I_Test_GUI_Suite" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\RSRelease\Bin\_RelayClientDLLTestDebug/_RelayClientDLLTestD.exe" /pdbtype:sept
# Begin Special Build Tool
TargetDir=\_Cv\020.Dev\20.myProject\01.StreamingServer\Development\RSRelease\Bin\_RelayClientDLLTestDebug
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy                                /C                                /R                                /Y                                    "$(TargetDir)\*.*"                                 D:\RSDLL_60\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "_RelayClientDLLTest - Win32 Release"
# Name "_RelayClientDLLTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\__RelayClientDLLTestTest.cpp
# End Source File
# Begin Source File

SOURCE=.\_CppUnitTestSuite.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\_RelayClientDLLTest.cpp
# End Source File
# Begin Source File

SOURCE=.\_RelayClientDLLTest.rc
# End Source File
# Begin Source File

SOURCE=.\ChannelClient.cpp
# End Source File
# Begin Source File

SOURCE=.\InstanceClient.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerClient.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\__RelayClientDLLTestSuite.h
# End Source File
# Begin Source File

SOURCE=.\__RelayClientDLLTestTest.h
# End Source File
# Begin Source File

SOURCE=.\_RelayClientDLLTest.h
# End Source File
# Begin Source File

SOURCE=.\ChannelClient.h
# End Source File
# Begin Source File

SOURCE=.\InstanceClient.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ServerClient.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\_RelayClientDLLTest.ico
# End Source File
# Begin Source File

SOURCE=.\res\_RelayClientDLLTest.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
