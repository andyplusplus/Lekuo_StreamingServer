// RelayClient.cpp: implementation of the RelayClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "RelayClient.h"
#include "Log.h"
//#include "RtspUrlFormatAsserter.h"

//#include "Channel.h"
#include "MyAssert.h"

//Start Initiated Related Stuff

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "OSMemory.h"
#include "SocketUtils.h"
#include "ClientSession.h"

//End Initiated Related Stuff


static RelayClient*		sRelayClient = NULL;
static RSClientElem*	sRCInstance = NULL;

RelayClient* RelayClient::Instance(const char* strReservedPara, IRelayClient* inIRC, Tag inTag)
{
	LK_Error theLK_Result = CRtspUrlFormatAsserter::AssertRtspUrlCmdFormat(strReservedPara, EN_RTSP_CMD_FMT_InstanceStr);
	if(LK_NoErr != theLK_Result)
	{
		Assert(FALSE);
		return NULL;
	}

	if (!sRelayClient)
	{//Start Initiated Related Stuff
		WORD wsVersion;
		WSADATA wsData;
		wsVersion = MAKEWORD(1, 1);
		(void)::WSAStartup(wsVersion, &wsData);
		
		OS::Initialize();					//1. sLastMillisMutex = NEW OSMutex();
		OSThread::Initialize();				//2. ::TlsAlloc();
		OSMemory::SetMemoryError(ENOMEM);	//3. 
		Socket::Initialize();				//4. 新建事件线程
		SocketUtils::Initialize(false);		//5.  NULL Oper
		
		TaskThreadPool::AddThreads(1);		//6. new TaskThread
		TimeoutTask::Initialize();			//7. 没有删除
		Socket::StartThread();				//8. 启动事件线程
		IdleTask::Initialize();
	};//End Initiated Related Stuff



	if (! sRCInstance)
	{
		RSClientElem* theParentElem_NULL = NULL;
		HANDLE theContainedHandle_NULL = NULL;
		 sRCInstance = new RSClientElem(strReservedPara, inIRC, inTag, theParentElem_NULL, theContainedHandle_NULL);
	}

	if (!sRelayClient)
	{
		CLog::LogFun_SSIT("RelayClient::Instance", strReservedPara, (void*) inIRC, inTag);
		sRelayClient = new RelayClient();
	}
	return sRelayClient; 
}


HANDLE RelayClient::toHandle(RelayClient* inHandle)
{
	return (HANDLE)inHandle;
}

RelayClient::RelayClient(){}

RelayClient::~RelayClient(){}


LK_Error RelayClient::DestroyInstance() 
{
	{
		//1. send a task signal information, tell the task thread to delete the task then wait the taskthread to work
		TimeoutTask::UnInitialize();

		//2. Signal The Event Thread, TimeoutTaskThread, TaskThread  to Exit
		WZD_Signal_EventThread_To_Exit();
		IdleTask::WZD_SignalIdleThreadExit();
		TaskThreadPool::WZD_SignalThreadsExit();

		TaskThreadPool::RemoveThreads();	//6.

		IdleTask::UnInitialize();

		Socket::UnInitialize();				//EventThread

		OSThread::UnInitialize();			//Delete the thread object

		SocketUtils::UnInitialize();		//Delete the IP AddrInfo

	}//End Initiated Related Stuff

	CLog::LogFun_S("RelayClient::DestroyInstance");
	
	if (sRCInstance)
	{
		delete sRCInstance;
		sRCInstance = NULL;
	}
	
	if (sRelayClient)
	{
		delete sRelayClient;
		sRelayClient = NULL;
	}
	
	return LK_NoErr;
};

//============================================================================================================================
//============================================================================================================================
//
// Create the Relay Server Object
//
//============================================================================================================================
//============================================================================================================================

HANDLE RelayClient::CreateRelayServer(HANDLE hInstance, const char* strPara, IRelayClient* inIRC, Tag inTag) 
{
	CLog::LogFun_SISIT("RelayClient::CreateRelayServer", hInstance, strPara, (void*) inIRC,inTag);


	LK_Error theLK_Result = CRtspUrlFormatAsserter::AssertRtspUrlCmdFormat(strPara, EN_RTSP_CMD_FMT_CreateRelayServer);
	if(LK_NoErr != theLK_Result)
	{
		Assert(FALSE);
		return NULL;
	}


	RSClientElem* theParentElem = sRCInstance;
	HANDLE theContainedHandle_NULL = NULL;

	RSClientElem* tRelayServer = new RSClientElem(strPara, inIRC, inTag, theParentElem, theContainedHandle_NULL);

	return (HANDLE)tRelayServer;
};


HANDLE RelayClient::CreateChannelInRelayServer(HANDLE hServer, const char* strPara, IRelayClient* inIRC, Tag  inTag) 
{
	CLog::LogFun_SISIT("RelayClient::CreateChannelInRelayServer", hServer, strPara, (void*) inIRC, inTag);

	LK_Error theLK_Result = CRtspUrlFormatAsserter::AssertRtspUrlCmdFormat(strPara, EN_RTSP_CMD_FMT_CreateChanInServer);
	if(LK_NoErr != theLK_Result)
	{
		Assert(FALSE);
		return NULL;
	}

	Channel* tChannel = new Channel(hServer, strPara, inIRC, inTag);
	return (HANDLE)tChannel;
};



LK_Error RelayClient::PlayChannel(HANDLE hChannel, const char* strPara) 
{
	CLog::LogFun_SIS("RelayClient::PlayChannel", hChannel, strPara);

	LK_Error theLK_Result = CRtspUrlFormatAsserter::AssertRtspUrlCmdFormat(strPara, EN_RTSP_CMD_FMT_PlayChannel);
	if(LK_NoErr != theLK_Result)
	{
		Assert(FALSE);
		return theLK_Result;
	}

	return ((Channel*) hChannel)->Play(strPara);
};


//The Relay Server dont support this function, so it returns 0
unsigned long GetMetadataLength(HANDLE hChannel)
{
	return 0;
};


//The Relay Server dont support this function, so it returns 0
unsigned long GetMetadataHeader(HANDLE hChannel, unsigned char* inDataBuffer, INT32 inBufferLength)
{
	return 0;
}



LK_Error RelayClient::StopChannel(HANDLE hChannel, const char* strPara)
{
	CLog::LogFun_SIS("RelayClient::StopChannel",hChannel, strPara);

	LK_Error theLK_Result = CRtspUrlFormatAsserter::AssertRtspUrlCmdFormat(strPara, EN_RTSP_CMD_FMT_StopChannel);
	if(LK_NoErr != theLK_Result)
	{
		Assert(FALSE);
		return theLK_Result;
	}

	return ((Channel*) hChannel)->Stop(strPara);
};



LK_Error RelayClient::SendChannelCommands(HANDLE hChannel, const char* Commands) 
{
	CLog::LogFun_SIS("RelayClient::SendChannelCommands", hChannel, Commands);

	LK_Error theLK_Result = CRtspUrlFormatAsserter::AssertRtspUrlCmdFormat(Commands, EN_RTSP_CMD_FMT_SendChannelCmds);
	if(LK_NoErr != theLK_Result)
	{
		Assert(FALSE);
		return theLK_Result;
	}

	return ((Channel*) hChannel)->SendCommands(Commands);
};



LK_Error RelayClient::DestroyChannel(HANDLE hChannel) 
{
	CLog::LogFun_SI("RelayClient::DestroyChannel", hChannel);

	return ((Channel*) hChannel)->Destroy();
};


LK_Error RelayClient::DestroyALLChannelInRelayServer(HANDLE hServer) 
{
	CLog::LogFun_SI("RelayClient::DestroyALLChannelInRelayServer", hServer);

	((RSClientElem*) hServer)->DeleteAllElemInQueue();

	return LK_NoErr;
};


LK_Error RelayClient::DestroyRelayServer(HANDLE hRelayServer) 
{
	CLog::LogFun_SI("RelayClient::DestoryRelayServer", hRelayServer);

	((RSClientElem*) hRelayServer)->DeleteAllElemInQueue();

	return LK_NoErr; 
}; 



LK_Error RelayClient::DestroyALLRelayServer() 
{
	CLog::LogFun_S("RelayClient::DestroyALLRelayServer()");
	
	((RSClientElem*) sRCInstance)->DeleteAllElemInQueue();
	
	return LK_NoErr;
};


void RelayClient::logInfo(const char* inStr)
{
	CLog::LogFun_S(inStr);
}
