//  IncludeCfg.h

#if !defined(Include_CFG__INCLUDED_)
#define Include_CFG__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning(disable:4996) //全部关掉
#pragma warning(disable:4099)
#pragma warning(disable:4819)
	//#pragma warning(once:4996) //仅显示一个
#define _CRT_SECURE_NO_DEPRECATE

#if  !defined WINVER
#define WINVER 0x0502
#endif


#define _rsDir__test_ 1
#define _rsDir__adaptor_ 1
#define _rsDir__client_  1
#define _rsDir__common_  1
#define _rsDir__server_  1
#define _rsDir__utility_ 1

//#	pragma message("I am here1")
//this is an template
#ifdef _I__Template_Lib_
#	ifdef _DEBUG
#pragma comment(lib, "D:\\RSDLL\\")
#	else
#pragma comment(lib, "D:\\RSDLL\\")
#	endif
#endif



#ifdef _rsDir__test_

	#ifdef _I_Test
	#	ifdef _DEBUG
	#		pragma comment(lib,    "..\\..\\..\\..\\lib\\CPPUnitLib\\cppunitd_dll.lib")
	#		pragma comment(lib,    "..\\..\\..\\..\\lib\\CPPUnitLib\\testrunnerd.lib")
	#	else
	#		pragma comment(lib, 	"..\\..\\..\\..\\lib\\CPPUnitLib\\cppunit_dll.lib")
	#		pragma comment(lib, 	"..\\..\\..\\..\\lib\\CPPUnitLib\\testrunner.lib")
	#	endif
	#endif



	#ifdef _I_TestAPP

	#	ifdef _DEBUG
	#		pragma comment(lib,    "..\\..\\lib\\CPPUnitLib\\cppunitd_dll.lib")
	#		pragma comment(lib,    "..\\..\\lib\\CPPUnitLib\\testrunnerd.lib")
	#		pragma comment(lib, "D:\\RSDLL\\common.DeviceManagerD.lib")
	#	else
	#		pragma comment(lib, 	"..\\..\\lib\\CPPUnitLib\\cppunit_dll.lib")
	#		pragma comment(lib, 	"..\\..\\lib\\CPPUnitLib\\testrunner.lib")
	#		pragma comment(lib, "D:\\RSDLL\\common.DeviceManager.lib")
	#	endif

	#endif

#endif //_rsDir__test_





#ifdef _rsDir__adaptor_

	#ifdef _I_HikAdaptor
	#	pragma comment(lib, "..\\..\\..\\Lib\\HikvisionLib\\HikPlayM4.lib")
	#	pragma comment(lib, "..\\..\\..\\Lib\\HikvisionLib\\HCNetSDK.lib")
	#endif



	#ifdef _I_RsDVRAdaptor

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTraceD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtilityD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\adaptor.vHikAdaptorD.lib")		//HikAdaptorD.lib
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTrace.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtility.lib")
	#		pragma comment(lib, "D:\\RSDLL\\adaptor.vHikAdaptor.lib")
	#	endif

	#endif



	#ifdef _I__vHikAdaptor_
	#	pragma comment(lib, "..\\..\\..\\Lib\\HikvisionLib\\HikPlayM4.lib")
	#	pragma comment(lib, "..\\..\\..\\Lib\\HikvisionLib\\HCNetSDK.lib")
	#	pragma comment(lib, "..\\..\\..\\Lib\\DaHuaLib\\mp4.lib")
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParserD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtilityD.Lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTraceD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParser.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtility.Lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTrace.lib")
	#	endif
	#endif

#endif //_rsDir__adaptor_





#ifdef _rsDir__client_


	#ifdef _I_RsPlayer

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\common.DeviceManagerD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\common.DeviceManager.lib")
	#	endif
	#endif



	#ifdef _I_StreamServerControlPanel

	//#pragma comment(lib, "Shlwapi.lib")
	#	pragma comment(lib, "..\\..\\..\\Lib\\HtmlHelp.lib")
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtilityD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtility.lib")
	#	endif

	#endif



	#ifdef _I_NetDiagnosisTool
	#	pragma comment(lib, "wininet.lib")
	#	pragma comment(lib, "IPHlpApi.Lib")
	#endif



	#ifdef _I_RsDaHuaViewer
	#	pragma comment(lib, "..\\..\\..\\Lib\\DahuaLib\\mp4.lib")
	#endif



	#ifdef _I_RSGUI

	//#pragma comment(lib, "..\\011.GrandDog\\OBJ\\WIN32VC.OBJ")
	#	pragma comment(lib, "..\\..\\..\\Lib\\HikvisionLib\\HikPlayM4.lib")

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\adaptor.vHikAdaptorD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtilityD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\server.RelayClientDLLD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\adaptor.vHikAdaptor.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtility.lib")		
	#		pragma comment(lib, "D:\\RSDLL\\server.RelayClientDLL.lib")
	#	endif

	#endif


#endif //_rsDir__client_





#ifdef _rsDir__common_

#ifdef _I_DeviceManager
	#	ifdef _DEBUG
	//#		pragma comment(lib, "D:\\RSDLL\\adaptor.vHikAdaptorD.lib")
	//#		pragma comment(lib, "D:\\RSDLL\\adaptor.HikAdaptorLiteD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTraceD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RsUtilityD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParserD.lib")
	#	else
	//#		pragma comment(lib, "D:\\RSDLL\\adaptor.vHikAdaptor.lib")
	//#		pragma comment(lib, "D:\\RSDLL\\adaptor.HikAdaptorLite.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTrace.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RsUtility.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParser.lib")
	#	endif
#endif

#endif //_rsDir__common_




#ifdef _rsDir__server_


	#ifdef _I_RelayClientDLL

	#	ifdef _DEBUG

	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTraceD.Lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtilityD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParserD.lib")

	#		pragma comment(lib, "D:\\RSDLL\\SC.APICommonCodeD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.APIStubLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.CommonUtilitiesLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.RTSPClientLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.SafeDynamicModuleStdLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.SafeInternalStdLibD.lib")

	#	else
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTrace.Lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtility.lib")	
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParser.lib")

	#		pragma comment(lib, "D:\\RSDLL\\SC.APICommonCode.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.APIStubLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.CommonUtilitiesLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.RTSPClientLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.SafeDynamicModuleStdLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.SafeInternalStdLib.lib")
	#	endif

	#endif



	//We will use the same program to test the RelayClientDLL && rsDVRAdaptor
	#ifdef _I__RelayClientDLLTest				//RelayClientDLL
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\server.RelayClientDLLD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\server.RelayClientDLL.lib")
	#	endif
	#endif
	//
	#ifdef _I__RelayClientDLLTest_NotUsed		//rsDVRAdaptor
	#	ifdef _DEBUG
	//#pragma comment(lib, "D:\\RSDLL\\RsDVRAdaptorD.lib")
	#	else
	//#pragma comment(lib, "D:\\RSDLL\\RsDVRAdaptor.lib")
	#	endif
	#endif





	#ifdef _I__PlayListBroadcaster_
		#define _I_Common_Server_Libs_	1
	#endif

	#ifdef _I__PlayListBroadcaster_
		#define _I_Common_Server_Libs_	1
	#endif

	#ifdef _I__RelayServer_
		#define _I_Common_Server_Libs_	1
	#endif

	#ifdef _I_Common_Server_Libs_
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\RS.APICommonCodeD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.APIStubLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.CommonUtilitiesLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.QTFileLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.QTFileExternalLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.SafeInternalStdLibD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\RS.APICommonCode.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.APIStubLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.CommonUtilitiesLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.QTFileLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.QTFileExternalLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\RS.SafeInternalStdLib.lib")
	#	endif
	#endif //_I_Common_Server_Libs_




	#ifdef _I__RelayServer_
	//#	ifdef _DEBUG
	//#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtilityD.lib")
	//#		pragma comment(lib, "D:\\RSDLL\\server.AdaptorModuleD.lib")
	//#	else
	//#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtility.lib")
	//#		pragma comment(lib, "D:\\RSDLL\\server.AdaptorModule.lib")
	//#	endif
	#endif	//_I__RelayServer_



	#ifdef _I__AdaptorModule_
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParserD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTraceD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\common.DeviceManagerD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtilityD.lib")
	//#		pragma comment(lib, "D:\\RSDLL\\adaptor.RsDVRAdaptorD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.APICommonCodeD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.APIStubLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.CommonUtilitiesLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.SafeDynamicModuleStdLibD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParser.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTrace.lib")
	#		pragma comment(lib, "D:\\RSDLL\\common.DeviceManager.lib")
	#		pragma comment(lib, "D:\\RSDLL\\utility.RSUtility.lib")
	//#		pragma comment(lib, "D:\\RSDLL\\adaptor.RsDVRAdaptor.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.APICommonCode.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.APIStubLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.CommonUtilitiesLib.lib")
	#		pragma comment(lib, "D:\\RSDLL\\SC.SafeDynamicModuleStdLib.lib")
	#	endif
	#endif //_I_RelayServerAdaptor


#endif //_rsDir__server_





#ifdef _rsDir__utility_


	#ifdef _I_Log4Interface_
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTraceD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\utility.LogTrace.lib")
	#	endif
	#endif

	#ifdef _I_RSUtility
	#endif


	#ifdef _I_RSUtility_Test

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\utility.RsUtilityD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\utility.RsUtility.lib")
	#	endif
	#endif


	#ifdef _I_LkStrParser

	#endif


	#ifdef _I_LkStrParserTest

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParserD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL\\utility.LkStrParser.lib")
	#	endif

	#endif

#endif  //_rsDir__utility_




#endif // !defined(Include_CFG__INCLUDED_)


