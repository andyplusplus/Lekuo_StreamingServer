/*
#include "adaptor_Common_Implement.cpp"
*/

#if !defined(_adaptor_Common_Implement_Included_)
#define _adaptor_Common_Implement_Included_

#include "dvrAdaptor.h"
#include "playerAdaptor.h"
#include "Adaptor_Ext.h"

ADAPTOR_EXT_API	 void* RS_API_Adaptor(void* inRsAPIArgs)
{
	RsAPIArgs* tArg = (RsAPIArgs*)inRsAPIArgs;
	switch (tArg->FunType)
	{
#ifdef LEKUO_DVRADAPTOR_EXPORT
	case Functions_Setup:
		Setup(tArg->inMutex);
		return NULL;

	case Functions_Teardown:
		Teardown();
		return NULL;

	case Functions_CreateDVR:
		return (void*) CreateDVR(tArg->inStr, tArg->inTag, tArg->inMutex);
#endif

#ifdef LEKUO_PLAYERADAPTOR_EXPORT
	case Functions_CreatePlayer:
		return (void*) CreatePlayer(tArg->inStr, tArg->inTag);
#endif

	default:
		//assert(FALSE);
		break;
	}
	return NULL;
}

#endif	//_adaptor_Common_Implement_Included_


