// hannel.cpp: implementation of the Channel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClientSessionPara.h"
//#include "Channel.h"
//#include "Device.h"
#include "RSMemroy.h"
#include "RsRTSPUtility.h"

#include "Log.h"

//目前该代码还不支持对象删除，以及请求合并
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//The following function need to do nothing here, the Server side need to implement this function
unsigned long  Channel::GetMetadataLength(){return 0;};
unsigned long  Channel::GetMetadataHeader(unsigned char* pDataBuffer, INT32 nBufferLength){return NULL;};


Channel::Channel(HANDLE hDevice, const char* inElemPara, IRelayClient* fIRelayClient, Tag inTag) : 
	RSClientElem(inElemPara, fIRelayClient, inTag, (RSClientElem*) hDevice, NULL)
{
	fLastPlayPara = NULL;
	fClientSessionPara = NULL;
}


Channel::~Channel()
{
	RSCharMemroy::releaseMemroy(&fLastPlayPara);
	if (fClientSessionPara)
	{
		delete 	fClientSessionPara;
		fClientSessionPara = NULL;
	}
}


LK_Error Channel::Play(const char *strPlayPara)
{
	RSCharMemroy::releaseMemroy(&fLastPlayPara);

	char* theCreateChannelinRelayServer = getElementCreationPara();
	char* theCreateRelayServerPara = getParentElem()->getElementCreationPara();

	//1. get the Play URL 
	fLastPlayPara = RsRTSPUtility::GeneratePlayURL(theCreateRelayServerPara, theCreateChannelinRelayServer, strPlayPara);
	CLog::LogFun_S(fLastPlayPara);

	//2. Setup the RTSP Session according to the play URL
	if (fClientSessionPara)
	{
		delete 	fClientSessionPara;
		fClientSessionPara = NULL;
	}

	fClientSessionPara = new ClientSessionPara(fLastPlayPara, this);
	return LK_NoErr;
}


//1. TBD: Request the streaming server to stop sending data
LK_Error Channel::Stop(const char *strPara)
{
	if (fClientSessionPara)
	{
		delete 	fClientSessionPara;
		fClientSessionPara = NULL;
	}
	return LK_NoErr;
}



LK_Error Channel::SendCommands(const char *strPara)
{
	//1. TBD: Send str commands on exisiting link
	return LK_NoErr;
}


LK_Error Channel::Destroy()
{
	//1. TBD: Request the streaming server to stop sending data
	//2. TBD: Remove the Obj from queue
	//3. TBD: Delete the Obj
	this->Stop(NULL);

	return LK_NoErr;
}
