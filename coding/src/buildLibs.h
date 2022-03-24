//  buildLibs.h
//  This file Include the build libs for the Samples code of streaming system

#if !defined(Include_BUILDLIBS__INCLUDED)
#define Include_BUILDLIBS__INCLUDED


#pragma warning(disable:4996)
#pragma warning(disable:4099)
#pragma warning(disable:4819)
#pragma warning(disable:4512)	
#pragma warning(disable:4099)
#define _CRT_SECURE_NO_DEPRECATE

#if  !defined WINVER
#define WINVER 0x0502
#endif

//#	pragma message("Build the buildLibs.h")

#ifdef _rsDir__test_

	#ifdef _I_Test
	#	ifdef _DEBUG
	#		pragma comment(lib,    "..\\..\\..\\..\\lib\\CPPUnit\\cppunitd_dll.lib")
	#		pragma comment(lib,    "..\\..\\..\\..\\lib\\CPPUnit\\testrunnerd.lib")
	#	else
	#		pragma comment(lib, 	"..\\..\\..\\..\\lib\\CPPUnit\\cppunit_dll.lib")
	#		pragma comment(lib, 	"..\\..\\..\\..\\lib\\CPPUnit\\testrunner.lib")
	#	endif
	#endif

#endif


#endif // !defined(Include_BUILDLIBS__INCLUDED)


