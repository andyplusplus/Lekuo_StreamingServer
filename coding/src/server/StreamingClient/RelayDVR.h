// #include "RelayDVR.h"
#pragma once

#include "LeKuo.h"
#include "IDVR.h"
#include "StdRsType.h"

class RelayDVR : public IDVR
{
public:
	RelayDVR(rsString inStr);

	virtual ~RelayDVR();

	virtual LK_Error Login(const char* inIP, int inPort, const char* inUserName, const char* inPassWord);
	virtual int IsLogedin();
	virtual LK_Error Logout();

	virtual LPIChannel CreateChannel(const char* inChannelStr, Tag inTag, IMutex* inMutex);
	virtual LK_Error DestroyChannel(LPIChannel inChannel);
	//virtual LK_Error DestroyChannel(const char* inChannelStr);

	virtual LK_Error DestroyAllChannel();

	void ReceiveMsg( HANDLE hSender, Tag inSenderTag, unsigned long inMsgType, unsigned char* inMsgData, unsigned long inLength, int isChanDeleted);

private:
	void DelChanTask(LPIChannel inChannel);
	LPIChannel getChannel(rsString inCreateStr);
	rsString getChannelKey(rsString inCreateStr);

	rsString m_CreateDVRStr;
	rsString m_LoginStr;
	rsString m_IP;
	BOOL m_isLogin;

	Channel_map m_Channel_map;
};

