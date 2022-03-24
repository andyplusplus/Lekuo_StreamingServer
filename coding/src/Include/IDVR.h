// IDVR.h: interface for the IDVR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LK_IDVR_H_INCLUDED_)
#define LK_IDVR_H_INCLUDED_

#include "LeKuo.h"
#include "IChannel.h"
#include "IMutex.h"
#include "IMsgReceiver.h"

class IDVR
{
public:
	virtual LK_Error Login(const char* inIP, int inPort, const char* inUserName, const char* inPassWord) = 0;
	virtual int		 IsLogedin() = 0;
    virtual LK_Error Logout() = 0;

	virtual LPIChannel CreateChannel(const char* inChannelStr, Tag inTag, IMutex* inMutex) = 0;

	virtual LK_Error DestroyChannel(LPIChannel inChannel)
	{
		delete inChannel;
		return 0;
	};

	virtual LK_Error DestroyAllChannel() = 0;

	virtual void AttachMsgReceiver(LPIMsgReceiver inMsgReceiver) 
	{
		m_MsgReceiver = inMsgReceiver;
	};

	virtual LPIMsgReceiver DetachMsgReceiver()
	{
		LPIMsgReceiver theReceiver = m_MsgReceiver;
		m_MsgReceiver = NULL;
		return theReceiver;
	};

public:
	virtual ~IDVR() {};
	IDVR():  m_MsgReceiver(NULL){};

protected:
	LPIMsgReceiver m_MsgReceiver;
};

typedef IDVR* LPIDVR;


#endif // !defined(LK_IDVR_H_INCLUDED_)
