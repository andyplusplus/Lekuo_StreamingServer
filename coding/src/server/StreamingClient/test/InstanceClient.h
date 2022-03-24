// InstanceClient.h: interface for the InstanceClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTANCECLIENT_H__B70357D9_412A_4FB3_89FF_174AD21F76C0__INCLUDED_)
#define AFX_INSTANCECLIENT_H__B70357D9_412A_4FB3_89FF_174AD21F76C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LeKuo.h"

class InstanceClient : public IRelayClient  
{
public:
	InstanceClient();
	virtual ~InstanceClient();

	virtual unsigned long ProcessData(
		
		HANDLE hSender, 	
		Tag inSenderTag,
		unsigned char* inData, 		
		unsigned long inLength, 
		unsigned long* ioCallbackParam
		
		){return 0;};

};

#endif // !defined(AFX_INSTANCECLIENT_H__B70357D9_412A_4FB3_89FF_174AD21F76C0__INCLUDED_)
