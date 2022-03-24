// ServerClient.h: interface for the ServerClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERCLIENT_H__9FCE2B06_1190_4C7E_83CB_BC458BB259A1__INCLUDED_)
#define AFX_SERVERCLIENT_H__9FCE2B06_1190_4C7E_83CB_BC458BB259A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LeKuo.h"

class ServerClient : public IRelayClient  
{
public:
	ServerClient();
	virtual ~ServerClient();

	virtual unsigned long ProcessData(
		
		HANDLE hSender, 	
		Tag inSenderTag,
		unsigned char* inData, 		
		unsigned long inLength, 
		unsigned long* ioCallbackParam
		
		){return 0;};

};

#endif // !defined(AFX_SERVERCLIENT_H__9FCE2B06_1190_4C7E_83CB_BC458BB259A1__INCLUDED_)
