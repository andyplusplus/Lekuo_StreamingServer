//#include "dvrAdaptor.h"
// ����DVR�����������������������豸DLL����ʵ�����º������ܽ�����ý��ϵͳ

#if !defined(DVR_ADAPTOR_H_INCLUDED)
#define DVR_ADAPTOR_H_INCLUDED

#ifdef LEKUO_DVRADAPTOR_EXPORT
#	define DVRADAPTOR_API  extern "C"	__declspec(dllexport) 
#else
#	define DVRADAPTOR_API	extern "C"	__declspec(dllimport)
#endif


#include "IDVR.h"


DVRADAPTOR_API LK_Error	Setup(IMutex* inMutex, char* inSetupStr = NULL);
DVRADAPTOR_API LK_Error	Teardown();
DVRADAPTOR_API LPIDVR	CreateDVR(const char* inCreateStr, Tag inTag, IMutex* inMutex);

typedef LK_Error	(*Setup_Ptr)(IMutex* inMutex, char* inSetupStr);
typedef LK_Error	(*Teardown_Ptr)();
typedef LPIDVR		(*CreateDVR_Ptr)(const char* inCreateStr, Tag inTag, IMutex* inMutex);


#endif	//DVR_ADAPTOR_H_INCLUDED
