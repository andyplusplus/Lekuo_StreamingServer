C:\Program Files (x86)\hotlab\StreamMediaServer\
C:\Program Files\hotlab\StreamMediaServer\

- [ ] 2022.03.01
	[v] 当前出现问题
		[ ] Visual Studio 2010 windows 10， need to download framework
				windows xp OK
		[ ] replaced _10.vcxproj to _22.vcxproj in *.vcxproj of project dependences
		[ ] replaced _10.vcxproj to _22.vcxproj in *.vcxproj of project dependences
		[ ] Handled
					Line 59:       <TypeLibraryName>D:\QTSS_Note\Debug\PlaylistBroadcaster/PlaylistBroadcaster_Note.tlb</TypeLibraryName>
					Line 85:       <ProgramDatabaseFile>D:\QTSS_Note\Debug\PlaylistBroadcaster/PlaylistBroadcasterD.pdb</ProgramDatabaseFile>		
		Replaced D: related folders
			
			$(OutDir)$(ProjectName)D.tlb
			$(OutDir)$(ProjectName)D.pdb
			$(OutDir)$(ProjectName)D.map


			$(OutDir)$(ProjectName).tlb
			$(OutDir)$(ProjectName).pdb
			$(OutDir)$(ProjectName).map
		[ ] renamed IncludeCfg_.h  
		[ ] renamed IncludeCfg_05_.h
		[ ] added to RS.APICommonCode postbuild Event
					xcopy          /C          /R          /Y              "..\..\..\..\lib_merged\*.*"           D:\RSDLL_22\
					xcopy          /C /R   /Y     /E         "..\..\..\..\lib_merged\*.*"           D:\RSDLL_22\
					for /r "..\..\..\..\lib" %f in (*.*) do @xcopy "%f" "D:\RSDLL_22\test" /f/y
					for /r "..\..\..\..\lib"                           %f in (*.*) do @xcopy "%f" "D:\RSDLL_22\test" /f/y
					for /r "D:\RelayServer\my_Surveillance\coding\lib" %f in (*.*) do @xcopy "%f" "D:\RSDLL_22\test" /f/y

	[v] backup to D:\RelayServer\my_Surveillance\coding\src_moved
		*_Client.dsp
		*_Client.vcproj
		*_Client_??.vcproj
		*_Client.vcxproj
		*_05.sln
		*old.dsp
		*_Note.vcproj
		*_05.vcproj
		*_10.vcproj
		*_22.vcproj


- [ ] 2022.02.17
	[v] Install Visual Studio 2010 in vm
	[v] Rebuild Project 2010 in vm
	[v] Backup not related projects 6.0, 2005, 2008, 12

- [ ] 2022.02.16
	[v] Done with Visual Studio 2010
	[v] Create Project & Sln Visual Studio 2022, 
		- [v] All_Rs_Project_22.sln   Set compatible with 2010
		- [v] RelayServer_10.sln   Set compatible with 2010

		

- [v] 2022.02.14
		- [v] Delete _08.xxx
		- [v] Rename _05.sln  _05.vcproj --> _08.xxx
		- [v] Created _22.sln  _22.vcproj, since visual studio 2022 supports 2020, therefore visual studio 2010 is installed
		- [v] Build Visual Studio:
			- [v] All_Rs_Project_10.sln   
				LkStrParser_10.vcxproj   Only this project depended on Boost, there this project could be skipped, removed
			- [v] RelayServer_10.sln

#ifndef ENOTCONN
#endif

- [ ] 2022.02.13
	- [ ] RelayServer_Dev_2008-8-6__with_PDB  --> E:\_Svn
		- [v] build with visual studio 2008: D:\_Svn\RelayServer\dev\coding\src\server\RelayServer\RelayServer_05.sln
			- [ ] Unknown compiler version - please run the configure tests and report the results
			- [v] output to : D:\RSDLL_05
			- [v] VLC:  rtsp://localhost/sample_100kbit.mp4
		- [ ] build with visual studio 2008: D:\_Svn\RelayServer\dev\coding\src\All_Rs_Project_05.sln
			- [v] Seperate build with Visual Studio 2008 utility.LogTraceD
			- [v] Run client.RsPlayer and retrieve four videos