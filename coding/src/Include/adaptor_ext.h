//#include "Adaptor_Ext.h"
//该目录下的内容和 Include目录下的"Adaptor_Ext.h"，相同，只不是为另一种调用方法

#if !defined(_RSADAPTOR_EXT_H_INCLUDED)
#define      _RSADAPTOR_EXT_H_INCLUDED

#include "IDVR.h"
#include "IPlayer.h"

#ifdef LEKUO_DVRADAPTOR_EXT_EXPORT
#	define ADAPTOR_EXT_API   extern "C"	__declspec(dllexport) 
#else
#	define ADAPTOR_EXT_API	extern "C"	__declspec(dllimport)
#endif



ADAPTOR_EXT_API	 void* RS_API_Adaptor(void* inRsAPIArgs);
         typedef void* (*RS_API_Adaptor_Ptr)(void* inRsAPIArgs);


enum RsAPIArgs_Functions
{
	Functions_Setup,
	Functions_Teardown,
	Functions_CreateDVR,
	Functions_CreatePlayer
};


struct RsAPIArgs
{
	RsAPIArgs(RsAPIArgs_Functions pFun, const char* pchar=NULL, Tag pTag=0, IMutex* pMutex=NULL) : FunType(pFun), inStr(pchar), inTag(pTag), inMutex(pMutex){};
	RsAPIArgs(RsAPIArgs_Functions pFun, IMutex* pMutex) : FunType(pFun), inStr(NULL), inTag(0), inMutex(pMutex){};

	RsAPIArgs_Functions  FunType;
	const char*	inStr;
	Tag				  inTag;
	IMutex*			inMutex;
};


#endif	//_RSADAPTOR_EXT_H_INCLUDED
