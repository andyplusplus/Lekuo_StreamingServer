
#include "stdafx.h"
#include "dvrAdaptor.h"
#include "RelayDVR.h"
#include "assert.h"

#include "Log.h"
#include "MyAssert.h"
//Start Initiated Related Stuff
#include <stdio.h>
#include <stdlib.h>
#include "SocketUtils.h"
#include "ClientSession.h"
#include "MyAssert.h"
#include "QTSServerInterface.h"
//End Initiated Related Stuff



#include "adaptor_Common_Implement.cpp"

static BOOL s_isAdaptorSetup = 0;

DVRADAPTOR_API LK_Error Setup(IMutex* inMutex, char* /*inSetupStr*/)
{

	if(! s_isAdaptorSetup)
	{
		CLog::LogFun_SI("Setup", inMutex);

		s_isAdaptorSetup = TRUE;

		WORD wsVersion;
		WSADATA wsData;
		wsVersion = MAKEWORD(1, 1);
		(void)::WSAStartup(wsVersion, &wsData);

		OS::Initialize();					//1. sLastMillisMutex = NEW OSMutex();
		OSThread::Initialize();				//2. ::TlsAlloc();
		//OSMemory::SetMemoryError(ENOMEM);	//3. 
		Socket::Initialize();				//4. 新建事件线程
		SocketUtils::Initialize(false);		//5.  NULL Oper

		TaskThreadPool::AddThreads(1);		//6. new TaskThread
		TimeoutTask::Initialize();			//7. 没有删除
		Socket::StartThread();				//8. 启动事件线程
		IdleTask::Initialize();	
        //SetAssertLogger(QTSServerInterface::GetErrorLogStream());// the error log stream is our assert logger
        SetAssertLogger(NULL);// the error log stream is our assert logger
	}
	return LK_NoErr;
}


DVRADAPTOR_API LK_Error Teardown()
{
	if(s_isAdaptorSetup)
	{
		CLog::LogFun_S("Teardown");

		s_isAdaptorSetup = FALSE;

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
	}
	return LK_NoErr;
}

 
DVRADAPTOR_API LPIDVR CreateDVR(const char* inStr, Tag inTag, IMutex* inMutex)
{
	Setup(inMutex);

	CLog::LogFun_SSTI("CreateDVR", inStr, inTag, inMutex);

	LPIDVR theDVR = new RelayDVR(inStr);

	qtss_printf("**** the RelayDVR*  =			%d\n", theDVR);

	return theDVR;
}
