//  IncludeCfg_RSDLL_22.h

#if !defined(Include_CFG__INCLUDED_)
#define Include_CFG__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning(disable:4996) //全部关掉
#pragma warning(disable:4099)
#pragma warning(disable:4819)

//warning C4512: 'XXX' : assignment operator could not be generated
#pragma warning(disable:4512)	

//warning C4127: conditional expression is constant
#pragma warning(disable:4127)

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
#pragma comment(lib, "D:\\RSDLL_22\\")
#	else
#pragma comment(lib, "D:\\RSDLL_22\\")
#	endif
#endif



#ifdef _rsDir__test_

	#ifdef _I_Test
	#	ifdef _DEBUG
	#		pragma comment(lib,    	"D:\\RSDLL_22\\cppunitd_dll.lib")
	#		pragma comment(lib,    	"D:\\RSDLL_22\\testrunnerd.lib")
	#	else
	#		pragma comment(lib, 	"D:\\RSDLL_22\\cppunit_dll.lib")
	#		pragma comment(lib, 	"D:\\RSDLL_22\\testrunner.lib")
	#	endif
	#endif



	#ifdef _I_TestAPP

	#	ifdef _DEBUG
	#		pragma comment(lib,    	"D:\\RSDLL_22\\cppunitd_dll.lib")
	#		pragma comment(lib,    	"D:\\RSDLL_22\\testrunnerd.lib")
	#		pragma comment(lib,    	"D:\\RSDLL_22\\adaptor.DeviceManagerD.lib")
	#	else
	#		pragma comment(lib, 	"D:\\RSDLL_22\\cppunit_dll.lib")
	#		pragma comment(lib, 	"D:\\RSDLL_22\\testrunner.lib")
	#		pragma comment(lib, 	"D:\\RSDLL_22\\adaptor.DeviceManager.lib")
	#	endif

	#endif

#endif //_rsDir__test_





#ifdef _rsDir__adaptor_

	#ifdef _I_HikAdaptor
	#	pragma comment(lib, 	"D:\\RSDLL_22\\HikPlayM4.lib")
	#	pragma comment(lib, 	"D:\\RSDLL_22\\HCNetSDK.lib")
	#endif



	#ifdef _I_RsDVRAdaptor

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTraceD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.vDevAdaptorD.lib")		//HikAdaptorD.lib
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTrace.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtility.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.vDevAdaptor.lib")
	#	endif

	#endif



	#ifdef _I__vHikAdaptor_
	#	pragma comment(lib, 	"D:\\RSDLL_22\\HikPlayM4.lib")
	#	pragma comment(lib, 	"D:\\RSDLL_22\\HCNetSDK.lib")
	#	pragma comment(lib, 	"D:\\RSDLL_22\\mp4.lib")
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParserD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.Lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTraceD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParser.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtility.Lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTrace.lib")
	#	endif
	#endif

#endif //_rsDir__adaptor_



#ifdef _rsDir__client_


	#ifdef _I_RsPlayer

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.DeviceManagerD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.DeviceManager.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
	#	endif
	#endif



	#ifdef _I_StreamServerControlPanel

	//#pragma comment(lib, "Shlwapi.lib")
	#	pragma comment(lib, 	"D:\\RSDLL_22\\HtmlHelp.lib")
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtility.lib")
	#	endif

	#endif



	#ifdef _I_NetDiagnosisTool
	#	pragma comment(lib, "wininet.lib")
	#	pragma comment(lib, "IPHlpApi.Lib")
	#endif



	#ifdef _I_RsDaHuaViewer
	#	pragma comment(lib, "D:\\RSDLL_22\\mp4.lib")
	#endif



	#ifdef _I_RSGUI

	//#pragma comment(lib, 	"D:\\RSDLL_22\\WIN32VC.OBJ")
	#	pragma comment(lib, "D:\\RSDLL_22\\HikPlayM4.lib")

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.vDevAdaptorD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.LkPlayerD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\server.RelayClientDLLD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.vDevAdaptor.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.LkPlayer.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtility.lib")		
	#		pragma comment(lib, "D:\\RSDLL_22\\server.RelayClientDLL.lib")
	#	endif

	#endif


#endif //_rsDir__client_





#ifdef _rsDir__common_

#ifdef _I_DeviceManager
	#	ifdef _DEBUG
	//#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.vHikAdaptorD.lib")
	//#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.HikAdaptorLiteD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTraceD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RsUtilityD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParserD.lib")
	#	else
	//#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.vHikAdaptor.lib")
	//#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.HikAdaptorLite.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTrace.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RsUtility.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParser.lib")
	#	endif
#endif

#endif //_rsDir__common_




#ifdef _rsDir__server_

	#ifdef _I_LkDemo

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.DeviceManagerD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.DeviceManager.lib")
	#	endif

	#endif


	#ifdef _I_RelayClientDLL

	#	ifdef _DEBUG

	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTraceD.Lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParserD.lib")

	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APICommonCodeD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APIStubLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.CommonUtilitiesLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.RTSPClientLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeDynamicModuleStdLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeInternalStdLibD.lib")

	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTrace.Lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtility.lib")	
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParser.lib")

	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APICommonCode.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APIStubLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.CommonUtilitiesLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.RTSPClientLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeDynamicModuleStdLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeInternalStdLib.lib")
	#	endif

	#endif



	//We will use the same program to test the RelayClientDLL && rsDVRAdaptor
	#ifdef _I__RelayClientDLLTest				//RelayClientDLL
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\server.RelayClientDLLD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\server.RelayClientDLL.lib")
	#	endif
	#endif
	//
	#ifdef _I__RelayClientDLLTest_NotUsed		//rsDVRAdaptor
	#	ifdef _DEBUG
	//#pragma comment(lib, 		"D:\\RSDLL_22\\RsDVRAdaptorD.lib")
	#	else
	//#pragma comment(lib, 		"D:\\RSDLL_22\\RsDVRAdaptor.lib")
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
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APICommonCodeD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APIStubLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.CommonUtilitiesLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.QTFileLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.QTFileExternalLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeInternalStdLibD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtility.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APICommonCode.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APIStubLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.CommonUtilitiesLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.QTFileLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.QTFileExternalLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeInternalStdLib.lib")
	#	endif
	#endif //_I_Common_Server_Libs_




	#ifdef _I__RelayServer_
	//#	ifdef _DEBUG
	//#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
	//#		pragma comment(lib, "D:\\RSDLL_22\\server.AdaptorModuleD.lib")
	//#	else
	//#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtility.lib")
	//#		pragma comment(lib, "D:\\RSDLL_22\\server.AdaptorModule.lib")
	//#	endif
	#endif	//_I__RelayServer_



	#ifdef _I__AdaptorModule_
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParserD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTraceD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.DeviceManagerD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
	//#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.RsDVRAdaptorD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APICommonCodeD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APIStubLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.CommonUtilitiesLibD.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeDynamicModuleStdLibD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParser.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTrace.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.DeviceManager.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtility.lib")
	//#		pragma comment(lib, "D:\\RSDLL_22\\adaptor.RsDVRAdaptor.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APICommonCode.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.APIStubLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.CommonUtilitiesLib.lib")
	#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeDynamicModuleStdLib.lib")
	#	endif
	#endif //_I_RelayServerAdaptor


#endif //_rsDir__server_





#ifdef _rsDir__utility_


	#ifdef _I_Log4Interface_
	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTraceD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTrace.lib")
	#	endif
	#endif

	#ifdef _I_RSUtility
	#endif


	#ifdef _I_RSUtility_Test

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RsUtilityD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.RsUtility.lib")
	#	endif
	#endif


	#ifdef _I_LkStrParser

	#endif


	#ifdef _I_LkStrParserTest

	#	ifdef _DEBUG
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParserD.lib")
	#	else
	#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParser.lib")
	#	endif

	#endif

#endif  //_rsDir__utility_


#ifdef _RS_RTSP_CLIENT_LIB_

#	ifdef _DEBUG

#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTraceD.Lib")
#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtilityD.lib")
#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParserD.lib")

#		pragma comment(lib, "D:\\RSDLL_22\\RS.APICommonCodeD.lib")
#		pragma comment(lib, "D:\\RSDLL_22\\RS.APIStubLibD.lib")
#		pragma comment(lib, "D:\\RSDLL_22\\RS.CommonUtilitiesLibD.lib")
#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeDynamicModuleStdLibD.lib")
#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeInternalStdLibD.lib")

#	else
#		pragma comment(lib, "D:\\RSDLL_22\\utility.LogTrace.Lib")
#		pragma comment(lib, "D:\\RSDLL_22\\utility.RSUtility.lib")	
#		pragma comment(lib, "D:\\RSDLL_22\\utility.LkStrParser.lib")

#		pragma comment(lib, "D:\\RSDLL_22\\RS.APICommonCode.lib")
#		pragma comment(lib, "D:\\RSDLL_22\\RS.APIStubLib.lib")
#		pragma comment(lib, "D:\\RSDLL_22\\RS.CommonUtilitiesLib.lib")
#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeDynamicModuleStdLib.lib")
#		pragma comment(lib, "D:\\RSDLL_22\\RS.SafeInternalStdLib.lib")
#	endif

#endif



#endif // !defined(Include_CFG__INCLUDED_)


