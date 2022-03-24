// ChannelClient.h: interface for the ChannelClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANNELCLIENT_H__DFEAE129_A45A_472E_B987_C512769FB019__INCLUDED_)
#define AFX_CHANNELCLIENT_H__DFEAE129_A45A_472E_B987_C512769FB019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LeKuo.h"

class ChannelClient : public IRelayClient  
{
public:
	ChannelClient();
	virtual ~ChannelClient();

	virtual unsigned long ProcessData(
		
		HANDLE hSender, 	
		Tag inSenderTag,
		unsigned char* inData, 
		unsigned long inLength, 
		unsigned long* ioCallbackParam
		
		){return 0;};

};

#endif // !defined(AFX_CHANNELCLIENT_H__DFEAE129_A45A_472E_B987_C512769FB019__INCLUDED_)
