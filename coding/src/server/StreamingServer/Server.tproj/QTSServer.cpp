/*
 *
 * APPLE_LICENSE_HEADER_START@
 *
 * Copyright c 1999-2008 Apple Inc.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 the 'License'. You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 */


/*
    File:       QTSServer.cpp

    Contains:   Implements object defined in QTSServer.h
    
    

*/


#ifndef __Win32__
#include <sys/types.h>
#include <dirent.h>
#endif
#include <errno.h>

#ifndef __Win32__
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#endif

#include "QTSServer.h"

#include "OSMemory.h"
#include "OSArrayObjectDeleter.h"
#include "SocketUtils.h"
#include "TCPListenerSocket.h"
#include "Task.h"

#include "QTSS_Private.h"
#include "QTSSCallbacks.h"
#include "QTSSModuleUtils.h"

//Compile time modules
#include "QTSSErrorLogModule.h"
#include "QTSSFileModule.h"
#include "QTSSAccessLogModule.h"
#include "QTSSFlowControlModule.h"
#include "QTSSReflectorModule.h"
#ifdef PROXYSERVER
#include "QTSSProxyModule.h"
#endif
#include "QTSSRelayModule.h"
#include "QTSSPosixFileSysModule.h"
#include "QTSSAdminModule.h"
#include "QTSSAccessModule.h"
#include "QTSSMP3StreamingModule.h"
#if MEMORY_DEBUGGING
#include "QTSSWebDebugModule.h"
#endif



#include "RTSPRequestInterface.h"
#include "RTSPSessionInterface.h"
#include "RTPSessionInterface.h"
#include "RTSPSession.h"
#include "RTPStream.h"
#include "RTCPTask.h"
#include "QTSSFile.h"

// CLASS DEFINITIONS

class RTSPListenerSocket : public TCPListenerSocket
{
    public:
    
        RTSPListenerSocket() {}
        virtual ~RTSPListenerSocket() {}
        
        //sole job of this object is to implement this function
        virtual Task*   GetSessionTask(TCPSocket** outSocket);
        
        //check whether the Listener should be idling
        Bool16 OverMaxConnections(UInt32 buffer);

};

class RTPSocketPool : public UDPSocketPool
{
    public:
    
        // Pool of UDP sockets for use by the RTP server
        
        RTPSocketPool() {}
        ~RTPSocketPool() {}
        
        virtual UDPSocketPair*  ConstructUDPSocketPair();
        virtual void            DestructUDPSocketPair(UDPSocketPair* inPair);

        virtual void            SetUDPSocketOptions(UDPSocketPair* inPair);
};



char*           QTSServer::sPortPrefString = "rtsp_port";
QTSS_Callbacks  QTSServer::sCallbacks;
XMLPrefsParser* QTSServer::sPrefsSource = NULL;
PrefsSource*    QTSServer::sMessagesSource = NULL;


QTSServer::~QTSServer()
{
    //
    // Grab the server mutex. This is to make sure all gets & set values on this
    // object complete before we start deleting stuff
    OSMutexLocker serverlocker(this->GetServerObjectMutex());
    
    //
    // Grab the prefs mutex. This is to make sure we can't reread prefs
    // WHILE shutting down, which would cause some weirdness for QTSS API
    // (some modules could get QTSS_RereadPrefs_Role after QTSS_Shutdown, which would be bad)
    OSMutexLocker locker(this->GetPrefs()->GetMutex());

    QTSS_ModuleState theModuleState;
    theModuleState.curRole = QTSS_Shutdown_Role;
    theModuleState.curTask = NULL;
    OSThread::SetMainThreadData(&theModuleState);

    for (UInt32 x = 0; x < QTSServerInterface::GetNumModulesInRole(QTSSModule::kShutdownRole); x++)
        (void)QTSServerInterface::GetModule(QTSSModule::kShutdownRole, x)->CallDispatch(QTSS_Shutdown_Role, NULL);

    OSThread::SetMainThreadData(NULL);
}

//1. 初始化回调函数		2. 相关的字典初始化		3. 服务器存根初始化		4. 创建全局对象fSocketPool, fRTPMap 
//5. 加载错误日志模块	g)	设置默认IP地址、并记录启动时间		h)	创建侦听对象, 并开始侦听
Bool16 QTSServer::Initialize(XMLPrefsParser* inPrefsSource, PrefsSource* inMessagesSource, UInt16 inPortOverride, Bool16 createListeners)
{
    static const UInt32 kRTPSessionMapSize = 577;
    fServerState = qtssFatalErrorState;
    sPrefsSource = inPrefsSource;
    sMessagesSource = inMessagesSource;
    this->InitCallbacks();	//初始化回调函数, 使sCallbacks的中的数组元素指向QTSSCallbacks中的成员函数的函数地址

    //
    // DICTIONARY INITIALIZATION
    // 由于所有的模块的属性都由QTSSDictionaryMap::sDictionaryMaps[kNumDictionaries + kNumDynamicDictionaryTypes];
	// 维护, 所以在初始化时可以在类的初始化以前初始化
    QTSSModule::Initialize();			//初始化模块的属性字典
    QTSServerPrefs::Initialize();		//初始化服务器配置参数类对应的属性
    QTSSMessages::Initialize();			//初始化消息提示, 包括错误提示, 报告提示
    RTSPRequestInterface::Initialize();	
    RTSPSessionInterface::Initialize();
    RTPSessionInterface::Initialize();
    RTPStream::Initialize();
    RTSPSession::Initialize();
    QTSSFile::Initialize();
    QTSSUserProfile::Initialize();
    
    //
    // STUB SERVER INITIALIZATION
    //
    // Construct stub versions of the prefs and messages dictionaries. We need
    // both of these to initialize the server, but they have to be stubs because
    // their QTSSDictionaryMaps will presumably be modified when modules get loaded.
    // 创建存跟版参数字典和消息字典。我们需要通过此初始化服务器，由于模块装载时，
	// 字典的QTSSDictionaryMap可能会初修改，字典必须以存跟形式初始化。
	//
    fSrvrPrefs = new QTSServerPrefs(inPrefsSource, false); // First time, don't write changes to the prefs file
    fSrvrMessages = new QTSSMessages(inMessagesSource);
    QTSSModuleUtils::Initialize(fSrvrMessages, this, QTSServerInterface::GetErrorLogStream());	//把参数分别把参数赋值给sMessages, sServer, sErrorLog

    //
    // SETUP ASSERT BEHAVIOR
    //
    // Depending on the server preference, we will either break when we hit an
    // assert, or log the assert to the error log
    if (!fSrvrPrefs->ShouldServerBreakOnAssert())
        SetAssertLogger(this->GetErrorLogStream());// the error log stream is our assert logger
        
    //
    // CREATE GLOBAL OBJECTS
    fSocketPool = new RTPSocketPool();				//创建RTP端口对,包括发送者端口,以及接收者端口
    fRTPMap = new OSRefTable(kRTPSessionMapSize);	//服务器对象的fRTPMap,key设置为RTSP的客户端会话ID

    //
    // Load ERROR LOG module only. This is good in case there is a startup error.
    
    QTSSModule* theLoggingModule = new QTSSModule("QTSSErrorLogModule");		//新建QTSS模块,该模块是Task类的子类
    (void)theLoggingModule->SetupModule(&sCallbacks, &QTSSErrorLogModule_Main);	//
    (void)AddModule(theLoggingModule);
    this->BuildModuleRoleArrays();

    //
    // DEFAULT IP ADDRESS & DNS NAME
    if (!this->SetDefaultIPAddr())
        return false;

    //
    // STARTUP TIME - record it
    fStartupTime_UnixMilli = OS::Milliseconds();
    fGMTOffset = OS::GetGMTOffset();
        
    //
    // BEGIN LISTENING
    if (createListeners)
    {
        if ( !this->CreateListeners(false, fSrvrPrefs, inPortOverride) )	
            QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssMsgSomePortsFailed, 0);
		///////到此为止, 可以接收RTSP请求了, 并且在接收RTSP请求以后, 能传信相应的任务对象////////
    }
    
    if ( fNumListeners == 0 )
    {   if (createListeners)
            QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssMsgNoPortsSucceeded, 0);
        return false;
    }

    
    fServerState = qtssStartingUpState;
    return true;
}

//该函数加载并初始化所有模块
void QTSServer::InitModules(QTSS_ServerState inEndState)
{
    //
    // LOAD AND INITIALIZE ALL MODULES
        
    // temporarily set the verbosity on missing prefs when starting up to debug level
    // This keeps all the pref messages being written to the config file from being logged.
    // don't exit until the verbosity level is reset back to the initial prefs.
    //当从Debug级启动时，暂时把丢失参数的verbosity。这样可把所有的pref消息写入日志的配置文件。
	//恢复verbosity级别以前，不要离开本函数。
    LoadModules(fSrvrPrefs);		//加载动态库模块
    LoadCompiledInModules();		//加载预编译模块
    this->BuildModuleRoleArrays();	//构造模块数组

    fSrvrPrefs->SetErrorLogVerbosity(qtssWarningVerbosity); //初始化编译的模块期间，关闭消息 // turn off info messages while initializing compiled in modules.
   //
    // CREATE MODULE OBJECTS AND READ IN MODULE PREFS
    
    // Finish setting up modules. Create our final prefs & messages objects,
    // register all global dictionaries, and invoke the modules in their Init roles.
	//创建模块对象并读入模块参数，完成模块设置，创建我们的最终的参数对象和消息对象，注册所有全局字典，并用初始化角色invoke模块
    fStubSrvrPrefs = fSrvrPrefs;			
    fStubSrvrMessages = fSrvrMessages;

    fSrvrPrefs = new QTSServerPrefs(sPrefsSource, true); //true表示要写入丢失的参数  // Now write changes to the prefs file. First time, we don't because the error messages won't get printed.
    QTSS_ErrorVerbosity serverLevel = fSrvrPrefs->GetErrorLogVerbosity(); //获得并保存real参数verbosity // get the real prefs verbosity and save it.
    fSrvrPrefs->SetErrorLogVerbosity(qtssWarningVerbosity); //加载动态模块时，关闭消息// turn off info messages while loading dynamic modules
   
 
    fSrvrMessages = new QTSSMessages(sMessagesSource);
    QTSSModuleUtils::Initialize(fSrvrMessages, this, QTSServerInterface::GetErrorLogStream());
	
    this->SetVal(qtssSvrMessages, &fSrvrMessages, sizeof(fSrvrMessages));
    this->SetVal(qtssSvrPreferences, &fSrvrPrefs, sizeof(fSrvrPrefs));

    //
    // ADD REREAD PREFERENCES SERVICE
    // 增加重读参数服务，把该服务加入到以kServiceDictIndex为关键字的字典中
    (void)QTSSDictionaryMap::GetMap(QTSSDictionaryMap::kServiceDictIndex)->
                AddAttribute(QTSS_REREAD_PREFS_SERVICE, (QTSS_AttrFunctionPtr)QTSServer::RereadPrefsService, qtssAttrDataTypeUnknown, qtssAttrModeRead);

    //
	// INVOKE初始化角色	    // INVOKE INITIALIZE ROLE
    this->DoInitRole();

    if (fServerState != qtssFatalErrorState)
        fServerState = inEndState; //服务器完成启动 // Server is done starting up!   


    fSrvrPrefs->SetErrorLogVerbosity(serverLevel); //恢复服务器的初始verbosity// reset the server's verbosity back to the original prefs level.
}

//该函数主要实现
//  1. 启动RTCPTask任务
//  2. 启动RTPStatsUpdaterTask任务
//  3. 启动RTSP事件通知机制, 通过::WSAAsyncSelect(req->er_handle, sMsgWindow, theMsg, theEvent)
void QTSServer::StartTasks()
{
    fRTCPTask = new RTCPTask();
    fStatsTask = new RTPStatsUpdaterTask();

    //
    // Start listening
    for (UInt32 x = 0; x < fNumListeners; x++)
        fListeners[x]->RequestEvent(EV_RE);
}

Bool16 QTSServer::SetDefaultIPAddr()
{
    //check to make sure there is an available ip interface
    if (SocketUtils::GetNumIPAddrs() == 0)
    {
        QTSSModuleUtils::LogError(qtssFatalVerbosity, qtssMsgNotConfiguredForIP, 0);
        return false;
    }

    //find out what our default IP addr is & dns name
    UInt32 theNumAddrs = 0;
    UInt32* theIPAddrs = this->GetRTSPIPAddrs(fSrvrPrefs, &theNumAddrs);
    if (theNumAddrs == 1)
        fDefaultIPAddr = SocketUtils::GetIPAddr(0);
    else
        fDefaultIPAddr = theIPAddrs[0];
    delete [] theIPAddrs;
        
    for (UInt32 ipAddrIter = 0; ipAddrIter < SocketUtils::GetNumIPAddrs(); ipAddrIter++)
    {
        if (SocketUtils::GetIPAddr(ipAddrIter) == fDefaultIPAddr)
        {
            this->SetVal(qtssSvrDefaultDNSName, SocketUtils::GetDNSNameStr(ipAddrIter));
            Assert(this->GetValue(qtssSvrDefaultDNSName)->Ptr != NULL);
            this->SetVal(qtssSvrDefaultIPAddrStr, SocketUtils::GetIPAddrStr(ipAddrIter));		//qtssSvrDefaultIPAddrStr 服务器的缺省IP地址，是一个字符串。
            Assert(this->GetValue(qtssSvrDefaultDNSName)->Ptr != NULL);
            break;
        }
    }
    if (this->GetValue(qtssSvrDefaultDNSName)->Ptr == NULL)
    {
        //If we've gotten here, what has probably happened is the IP address (explicitly
        //entered as a preference) doesn't exist
        QTSSModuleUtils::LogError(qtssFatalVerbosity, qtssMsgDefaultRTSPAddrUnavail, 0);
        return false;   
    }
    return true;
}               

///////////////////////////////////////////////////////////////////////////////////////////////////////
//该函数包括三项参数
//Bool16 startListeningNow		//是否开始侦听  
//QTSServerPrefs* inPrefs		//服务器参数对象  
//UInt16 inPortOverride			//是否重载端口，如果该值为0，则使用inPrefs中的端口  
//
//函数主要完成:
//1.	从参数选项读取IP地址，把地址和端口保存在thePortTrackers[currentIndex]中。
//2.	建新的TCPListenerSocket数组 
//3.	接着断定正在侦听哪些端口，如果正在侦听的端口与当前的设置的侦听端口一致，侧侦听的端口保存到新建立的newListenerArray 数组，该过程主要通过两种嵌套方法实现(复杂度为m*n)，否则 。 
//4.	然后建立新的侦听端口，判断是否需要建立侦听端口，如果需要则建立，然后初始化:
//5.	删除不需要的已经在侦听的端口
//6.	把端口加入到服务器属性中
//总之, 到此为止, 已经完成了RTSP Listener的建立, 这个时候应该已经可以接收RTSP请求了. 端口为默认激活端口554, 7070, 8000, 以及8001。
///////////////////////////////////////////////////////////////////////////////////////////////////////
//fListeners及fNumListeners是现有的数组

Bool16 QTSServer::CreateListeners(Bool16 startListeningNow, QTSServerPrefs* inPrefs, UInt16 inPortOverride)
{
    struct PortTracking
    {
        PortTracking() : fPort(0), fIPAddr(0), fNeedsCreating(true) {}
        
        UInt16 fPort;
        UInt32 fIPAddr;
        Bool16 fNeedsCreating;
    };
    
    PortTracking* thePortTrackers = NULL;   
    UInt32 theTotalPortTrackers = 0;
    
    // Get the IP addresses from the pref
//1.从参数选项读取IP地址，把地址和端口保存在thePortTrackers[currentIndex]中。
    UInt32 theNumAddrs = 0;
    UInt32* theIPAddrs = this->GetRTSPIPAddrs(inPrefs, &theNumAddrs);		
    UInt32 index = 0;
    
    if ( inPortOverride != 0)
    {
        theTotalPortTrackers = theNumAddrs; // one port tracking struct for each IP addr
        thePortTrackers = NEW PortTracking[theTotalPortTrackers];
        for (index = 0; index < theNumAddrs; index++)
        {
            thePortTrackers[index].fPort = inPortOverride;
            thePortTrackers[index].fIPAddr = theIPAddrs[index];
        }
    }
    else
    {
        UInt32 theNumPorts = 0;
        UInt16* thePorts = GetRTSPPorts(inPrefs, &theNumPorts);
        theTotalPortTrackers = theNumAddrs * theNumPorts;
        thePortTrackers = NEW PortTracking[theTotalPortTrackers];
        
        UInt32 currentIndex  = 0;
        
        for (index = 0; index < theNumAddrs; index++)
        {
            for (UInt32 portIndex = 0; portIndex < theNumPorts; portIndex++)
            {
                currentIndex = (theNumPorts * index) + portIndex;
                
                thePortTrackers[currentIndex].fPort = thePorts[portIndex];		//默认激活端口554, 7070, 8000, 以及8001。
                thePortTrackers[currentIndex].fIPAddr = theIPAddrs[index];		//默认为0?, 值为0说明服务器将接收本地所有激活的IP地址上的连接。该属性的默认值是0.
            }
        }
                
                delete [] thePorts;
    }
    
        delete [] theIPAddrs;
    //
    // Now figure out which of these ports we are *already* listening on.
    // If we already are listening on that port, just move the pointer to the
    // listener over to the new array
//2.建新的TCPListenerSocket数组, newListenerArray为指向指针的指针, 指向TCPListenerSocket指针数组, 
		//每个数组元素指向一个RTSPListenerSocket对象
    TCPListenerSocket** newListenerArray = NEW TCPListenerSocket*[theTotalPortTrackers];		//
    UInt32 curPortIndex = 0;
//3.接着断定正在侦听哪些端口，如果正在侦听的端口与当前的设置的侦听端口一致，
	//侧侦听的端口保存到新建立的newListenerArray 数组，该过程主要通过两种嵌套方法实现(复杂度为m*n)，否则 。
    for (UInt32 count = 0; count < theTotalPortTrackers; count++)
    {
        for (UInt32 count2 = 0; count2 < fNumListeners; count2++)
        {
            if ((fListeners[count2]->GetLocalPort() == thePortTrackers[count].fPort) &&
                (fListeners[count2]->GetLocalAddr() == thePortTrackers[count].fIPAddr))
            {
                thePortTrackers[count].fNeedsCreating = false;
                newListenerArray[curPortIndex++] = fListeners[count2];
                Assert(curPortIndex <= theTotalPortTrackers);
                break;
            }
        }
    }
    
    //
    // Create any new listeners we need
//4.然后建立新的侦听端口，判断是否需要建立侦听端口，如果需要则建立，然后初始化, 每次初始化以后发出一次请求, 从而启动消息机制
    for (UInt32 count3 = 0; count3 < theTotalPortTrackers; count3++)
    {
        if (thePortTrackers[count3].fNeedsCreating)
        {
            newListenerArray[curPortIndex] = NEW RTSPListenerSocket();		//RTSPListenerSocket在TCPListenerSocket的基础之上额外实现了GetSessionTask(TCPSocket** outSocket);
			//初始化主要工作是
			//    1. 是打开,			fFileDesc = ::socket(PF_INET, theType, 0);
			//    2. 绑定地址, 端口,	::bind(fFileDesc, (sockaddr *)&fLocalAddr, sizeof(fLocalAddr));
			//    3. 设置缓存大小,		this->SetSocketRcvBufSize(96 * 1024); 
			//    3. 侦听socket端口 ::listen(fFileDesc, queueLength);
            QTSS_Error err = newListenerArray[curPortIndex]->Initialize(thePortTrackers[count3].fIPAddr, thePortTrackers[count3].fPort);

            char thePortStr[20];
            qtss_sprintf(thePortStr, "%hu", thePortTrackers[count3].fPort);
            
            //
            // If there was an error creating this listener, destroy it and log an error
            if ((startListeningNow) && (err != QTSS_NoErr))
                delete newListenerArray[curPortIndex];

            if (err == EADDRINUSE)
                QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssListenPortInUse, 0, thePortStr);
            else if (err == EACCES)
                QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssListenPortAccessDenied, 0, thePortStr);
            else if (err != QTSS_NoErr)
                QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssListenPortError, 0, thePortStr);
            else
            {
                //
                // This listener was successfully created.
                if (startListeningNow)
                    newListenerArray[curPortIndex]->RequestEvent(EV_RE);
                curPortIndex++;
            }
        }
    }
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//到此为止, 可以接收RTSP请求了, 并且在接收RTSP请求以后, 能传信相应的任务对象//
	//////////////////////////////////////////////////////////////////////////////////////////////////

//5.删除不需要的已经在侦听的端口    
    //
    // Kill any listeners that we no longer need
    for (UInt32 count4 = 0; count4 < fNumListeners; count4++)
    {
        Bool16 deleteThisOne = true;
        
        for (UInt32 count5 = 0; count5 < curPortIndex; count5++)
        {
            if (newListenerArray[count5] == fListeners[count4])
                deleteThisOne = false;
        }
        
        if (deleteThisOne)
            fListeners[count4]->Signal(Task::kKillEvent);
    }
//6.把端口加入到服务器属性中
    //
    // Finally, make our server attributes and fListener privy to the new...
    fListeners = newListenerArray;
    fNumListeners = curPortIndex;
    UInt32 portIndex = 0;
    
    for (UInt32 count6 = 0; count6 < fNumListeners; count6++)
    {
        if  (fListeners[count6]->GetLocalAddr() != INADDR_LOOPBACK)
        {
            UInt16 thePort = fListeners[count6]->GetLocalPort();
			//qtssSvrRTSPPorts 为包含服务器正在侦听端口的索引化的属性
            (void)this->SetValue(qtssSvrRTSPPorts, portIndex, &thePort, sizeof(thePort), QTSSDictionary::kDontObeyReadOnly);
            portIndex++;
        }
    }
    this->SetNumValues(qtssSvrRTSPPorts, portIndex);		//qtssSvrRTSPPorts 这是一个索引过的属性，含有服务器正在侦听的所有端口

    delete [] thePortTrackers;
    return (fNumListeners > 0);
}

// 获当前系统设置中RTSP的IP地址数, 把获得的IP地址数目放在outNumAddrsPtr中, 返回IP地址数的指针
UInt32* QTSServer::GetRTSPIPAddrs(QTSServerPrefs* inPrefs, UInt32* outNumAddrsPtr)
{
    UInt32 numAddrs = inPrefs->GetNumValues(qtssPrefsRTSPIPAddr);		//qtssPrefsRTSPIPAddr 说明服务器应接收连接的IP地址(以带点的十进制表示)。当计算机有一个或多个IP地址时，该属性用来说明服务器应侦听的IP地址。值为0说明服务器将接收本地所有激活的IP地址上的连接。该属性的默认值是0.	
    UInt32* theIPAddrArray = NULL;
    
    if (numAddrs == 0)
    {
        *outNumAddrsPtr = 1;
        theIPAddrArray = NEW UInt32[1];
        theIPAddrArray[0] = INADDR_ANY;
    }
    else
    {
        theIPAddrArray = NEW UInt32[numAddrs + 1];
        UInt32 arrIndex = 0;
        
        for (UInt32 theIndex = 0; theIndex < numAddrs; theIndex++)
        {
            // Get the ip addr out of the prefs dictionary
            QTSS_Error theErr = QTSS_NoErr;
            
            char* theIPAddrStr = NULL;
            theErr = inPrefs->GetValueAsString(qtssPrefsRTSPIPAddr, theIndex, &theIPAddrStr);
            if (theErr != QTSS_NoErr)
            {
                delete [] theIPAddrStr;
                break;
            }

            
            UInt32 theIPAddr = 0;
            if (theIPAddrStr != NULL)
            {
                theIPAddr = SocketUtils::ConvertStringToAddr(theIPAddrStr);
                delete [] theIPAddrStr;
                
                if (theIPAddr != 0)
                    theIPAddrArray[arrIndex++] = theIPAddr;
            }   
        }
        
        if ((numAddrs == 1) && (arrIndex == 0))
            theIPAddrArray[arrIndex++] = INADDR_ANY;
        else
            theIPAddrArray[arrIndex++] = INADDR_LOOPBACK;
    
        *outNumAddrsPtr = arrIndex;
    }
    
    return theIPAddrArray;
}

//获得RTSP端口
UInt16* QTSServer::GetRTSPPorts(QTSServerPrefs* inPrefs, UInt32* outNumPortsPtr)
{
    *outNumPortsPtr = inPrefs->GetNumValues(qtssPrefsRTSPPorts);
    
    if (*outNumPortsPtr == 0)
        return NULL;
        
    UInt16* thePortArray = NEW UInt16[*outNumPortsPtr];
    
    for (UInt32 theIndex = 0; theIndex < *outNumPortsPtr; theIndex++)
    {
        // Get the ip addr out of the prefs dictionary
        UInt32 theLen = sizeof(UInt16);
        QTSS_Error theErr = QTSS_NoErr;
        theErr = inPrefs->GetValue(qtssPrefsRTSPPorts, theIndex, &thePortArray[theIndex], &theLen);
        Assert(theErr == QTSS_NoErr);   
    }
    
    return thePortArray;
}

//为每个本地IP建立UDP端口对(一个端口用于RTP, 另一个端口用于RTCP),并且在该端口上启动事件通知机制, 
//udp sockets在RTCP任务初始化以后建立
Bool16  QTSServer::SetupUDPSockets()
{
    //function finds all IP addresses on this machine, and binds 1 RTP / RTCP
    //socket pair to a port pair on each address.
    
    UInt32 theNumAllocatedPairs = 0;
    for (UInt32 theNumPairs = 0; theNumPairs < SocketUtils::GetNumIPAddrs(); theNumPairs++)
    {
        UDPSocketPair* thePair = fSocketPool->CreateUDPSocketPair(SocketUtils::GetIPAddr(theNumPairs), 0);
                if (thePair != NULL)
                {
            theNumAllocatedPairs++;
                        thePair->GetSocketA()->RequestEvent(EV_RE);
                        thePair->GetSocketB()->RequestEvent(EV_RE);
                }
        }
    //only return an error if we couldn't allocate ANY pairs of sockets
    if (theNumAllocatedPairs == 0)
        {
                fServerState = qtssFatalErrorState; // also set the state to fatal error
                return false;
        }
    return true;
}

//NOT USED
Bool16  QTSServer::SwitchPersonality()
{
#ifndef __Win32__  //not supported
    OSCharArrayDeleter runGroupName(fSrvrPrefs->GetRunGroupName());
    OSCharArrayDeleter runUserName(fSrvrPrefs->GetRunUserName());

    if (::strlen(runGroupName.GetObject()) > 0)
    {
        struct group* gr = ::getgrnam(runGroupName.GetObject());
        if (gr == NULL || ::setgid(gr->gr_gid) == -1)
        {
            char buffer[kErrorStrSize];

            QTSSModuleUtils::LogError(qtssFatalVerbosity, qtssMsgCannotSetRunGroup, 0,
                    runGroupName.GetObject(), qtss_strerror(OSThread::GetErrno(), buffer, sizeof(buffer)));
            return false;
        }
    }
    
    if (::strlen(runUserName.GetObject()) > 0)
    {
        struct passwd* pw = ::getpwnam(runUserName.GetObject());
        if (pw == NULL || ::setuid(pw->pw_uid) == -1)
        {
            QTSSModuleUtils::LogError(qtssFatalVerbosity, qtssMsgCannotSetRunUser, 0,
                    runUserName.GetObject(), strerror(OSThread::GetErrno()));
            return false;
        }
    }

#endif  
   return true;
}




void    QTSServer::LoadCompiledInModules()
{
#ifndef DSS_DYNAMIC_MODULES_ONLY
    // MODULE DEVELOPERS SHOULD ADD THE FOLLOWING THREE LINES OF CODE TO THIS
    // FUNCTION IF THEIR MODULE IS BEING COMPILED INTO THE SERVER.
    //
    // QTSSModule* myModule = new QTSSModule("__MODULE_NAME__");
    // (void)myModule->Initialize(&sCallbacks, &__MODULE_MAIN_ROUTINE__);
    // (void)AddModule(myModule);
    //
    // The following modules are all compiled into the server. 
    
    QTSSModule* theFileModule = new QTSSModule("QTSSFileModule");
    (void)theFileModule->SetupModule(&sCallbacks, &QTSSFileModule_Main);
    (void)AddModule(theFileModule);

    QTSSModule* theReflectorModule = new QTSSModule("QTSSReflectorModule");
    (void)theReflectorModule->SetupModule(&sCallbacks, &QTSSReflectorModule_Main);
    (void)AddModule(theReflectorModule);

	QTSSModule* theRelayModule = new QTSSModule("QTSSRelayModule");
    (void)theRelayModule->SetupModule(&sCallbacks, &QTSSRelayModule_Main);
    (void)AddModule(theRelayModule);

    QTSSModule* theAccessLog = new QTSSModule("QTSSAccessLogModule");
    (void)theAccessLog->SetupModule(&sCallbacks, &QTSSAccessLogModule_Main);
    (void)AddModule(theAccessLog);

    QTSSModule* theFlowControl = new QTSSModule("QTSSFlowControlModule");
    (void)theFlowControl->SetupModule(&sCallbacks, &QTSSFlowControlModule_Main);
    (void)AddModule(theFlowControl);

    QTSSModule* theFileSysModule = new QTSSModule("QTSSPosixFileSysModule");
    (void)theFileSysModule->SetupModule(&sCallbacks, &QTSSPosixFileSysModule_Main);
    (void)AddModule(theFileSysModule);

    QTSSModule* theAdminModule = new QTSSModule("QTSSAdminModule");
    (void)theAdminModule->SetupModule(&sCallbacks, &QTSSAdminModule_Main);
    (void)AddModule(theAdminModule);

    QTSSModule* theMP3StreamingModule = new QTSSModule("QTSSMP3StreamingModule");
    (void)theMP3StreamingModule->SetupModule(&sCallbacks, &QTSSMP3StreamingModule_Main);
    (void)AddModule(theMP3StreamingModule);

#if MEMORY_DEBUGGING
    QTSSModule* theWebDebug = new QTSSModule("QTSSWebDebugModule");
    (void)theWebDebug->SetupModule(&sCallbacks, &QTSSWebDebugModule_Main);
    (void)AddModule(theWebDebug);
#endif

    QTSSModule* theQTACESSmodule = new QTSSModule("QTSSAccessModule");
    (void)theQTACESSmodule->SetupModule(&sCallbacks, &QTSSAccessModule_Main);
    (void)AddModule(theQTACESSmodule);

#endif //DSS_DYNAMIC_MODULES_ONLY

#ifdef PROXYSERVER
    QTSSModule* theProxyModule = new QTSSModule("QTSSProxyModule");
    (void)theProxyModule->SetupModule(&sCallbacks, &QTSSProxyModule_Main);
    (void)AddModule(theProxyModule);
#endif

}


//初始化回调函数, 使sCallbacks的中的数组元素指向QTSSCallbacks中的成员函数的函数地址
void    QTSServer::InitCallbacks()
{
    sCallbacks.addr[kNewCallback] =                 (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_New;
    sCallbacks.addr[kDeleteCallback] =              (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Delete;
    sCallbacks.addr[kMillisecondsCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Milliseconds;
    sCallbacks.addr[kConvertToUnixTimeCallback] =   (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_ConvertToUnixTime;

    sCallbacks.addr[kAddRoleCallback] =             (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_AddRole;
    sCallbacks.addr[kCreateObjectTypeCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_CreateObjectType;
    sCallbacks.addr[kAddAttributeCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_AddAttribute;
    sCallbacks.addr[kIDForTagCallback] =            (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_IDForAttr;
    sCallbacks.addr[kGetAttributePtrByIDCallback] = (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_GetValuePtr;
    sCallbacks.addr[kGetAttributeByIDCallback] =    (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_GetValue;
    sCallbacks.addr[kSetAttributeByIDCallback] =    (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_SetValue;
    sCallbacks.addr[kCreateObjectValueCallback] =   (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_CreateObject;
    sCallbacks.addr[kGetNumValuesCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_GetNumValues;

    sCallbacks.addr[kWriteCallback] =               (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Write;
    sCallbacks.addr[kWriteVCallback] =              (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_WriteV;
    sCallbacks.addr[kFlushCallback] =               (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Flush;
    sCallbacks.addr[kReadCallback] =                (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Read;
    sCallbacks.addr[kSeekCallback] =                (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Seek;
    sCallbacks.addr[kAdviseCallback] =              (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Advise;

    sCallbacks.addr[kAddServiceCallback] =          (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_AddService;
    sCallbacks.addr[kIDForServiceCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_IDForService;
    sCallbacks.addr[kDoServiceCallback] =           (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_DoService;

    sCallbacks.addr[kSendRTSPHeadersCallback] =     (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_SendRTSPHeaders;
    sCallbacks.addr[kAppendRTSPHeadersCallback] =   (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_AppendRTSPHeader;
    sCallbacks.addr[kSendStandardRTSPCallback] =    (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_SendStandardRTSPResponse;

    sCallbacks.addr[kAddRTPStreamCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_AddRTPStream;
    sCallbacks.addr[kPlayCallback] =                (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Play;
    sCallbacks.addr[kPauseCallback] =               (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Pause;
    sCallbacks.addr[kTeardownCallback] =            (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Teardown;
    sCallbacks.addr[kRefreshTimeOutCallback] =      (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_RefreshTimeOut;

    sCallbacks.addr[kRequestEventCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_RequestEvent;
    sCallbacks.addr[kSetIdleTimerCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_SetIdleTimer;
    sCallbacks.addr[kSignalStreamCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_SignalStream;

    sCallbacks.addr[kOpenFileObjectCallback] =      (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_OpenFileObject;
    sCallbacks.addr[kCloseFileObjectCallback] =     (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_CloseFileObject;

    sCallbacks.addr[kCreateSocketStreamCallback] =  (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_CreateStreamFromSocket;
    sCallbacks.addr[kDestroySocketStreamCallback] = (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_DestroySocketStream;

    sCallbacks.addr[kAddStaticAttributeCallback] =          (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_AddStaticAttribute;
    sCallbacks.addr[kAddInstanceAttributeCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_AddInstanceAttribute;
    sCallbacks.addr[kRemoveInstanceAttributeCallback] =     (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_RemoveInstanceAttribute;

    sCallbacks.addr[kGetAttrInfoByIndexCallback] =          (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_GetAttrInfoByIndex;
    sCallbacks.addr[kGetAttrInfoByNameCallback] =           (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_GetAttrInfoByName;
    sCallbacks.addr[kGetAttrInfoByIDCallback] =             (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_GetAttrInfoByID;
    sCallbacks.addr[kGetNumAttributesCallback] =            (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_GetNumAttributes;


    sCallbacks.addr[kGetValueAsStringCallback] =            (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_GetValueAsString;
    sCallbacks.addr[kTypeToTypeStringCallback] =            (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_TypeToTypeString;
    sCallbacks.addr[kTypeStringToTypeCallback] =            (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_TypeStringToType;
    sCallbacks.addr[kStringToValueCallback] =               (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_StringToValue;
    sCallbacks.addr[kValueToStringCallback] =               (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_ValueToString;

    sCallbacks.addr[kRemoveValueCallback] =                 (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_RemoveValue;

    sCallbacks.addr[kRequestGlobalLockCallback] =           (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_RequestLockedCallback;
    sCallbacks.addr[kIsGlobalLockedCallback] =              (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_IsGlobalLocked;
    sCallbacks.addr[kUnlockGlobalLock] =                    (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_UnlockGlobalLock;

    sCallbacks.addr[kAuthenticateCallback] =                (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Authenticate;
    sCallbacks.addr[kAuthorizeCallback] =                   (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_Authorize;
    
    sCallbacks.addr[kLockObjectCallback] =                  (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_LockObject;
    sCallbacks.addr[kUnlockObjectCallback] =                (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_UnlockObject;
    sCallbacks.addr[kSetAttributePtrCallback] =             (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_SetValuePtr;
    
    sCallbacks.addr[kSetIntervalRoleTimerCallback] =        (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_SetIdleRoleTimer;
    
    sCallbacks.addr[kLockStdLibCallback] =                  (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_LockStdLib;
    sCallbacks.addr[kUnlockStdLibCallback] =                (QTSS_CallbackProcPtr)QTSSCallbacks::QTSS_UnlockStdLib;
}
//该函数有一个参数类型为服务器字典的参数，从字典读出模块名及其路径，并加载模块
//函数的实现过程如下
//  1. "	获得模块目录
//  2. "	在目录中查找该模块   ::FindFirstFile(theLargeModDirName.GetObject(), &theFindData);
//  3. "	如果找到模块，则创建模块  CreateModule(theModDirName.GetObject(), theFindData.cFileName); 
void QTSServer::LoadModules(QTSServerPrefs* inPrefs)
{
    // Fetch the name of the module directory and open it.
    OSCharArrayDeleter theModDirName(inPrefs->GetModuleDirectory());
    
#ifdef __Win32__
    // NT doesn't seem to have support for the POSIX directory parsing APIs.
    OSCharArrayDeleter theLargeModDirName(NEW char[::strlen(theModDirName.GetObject()) + 3]);
    ::strcpy(theLargeModDirName.GetObject(), theModDirName.GetObject());
    ::strcat(theLargeModDirName.GetObject(), "\\*");
    
    WIN32_FIND_DATA theFindData;
    HANDLE theSearchHandle = ::FindFirstFile(theLargeModDirName.GetObject(), &theFindData);
    
    if (theSearchHandle == INVALID_HANDLE_VALUE)
    {
        QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssMsgNoModuleFolder, 0);  
        return;
    }
    
    while (theSearchHandle != INVALID_HANDLE_VALUE)	//此处为每一个文件建立一个模块对象
    {
        this->CreateModule(theModDirName.GetObject(), theFindData.cFileName);
        
        if (!::FindNextFile(theSearchHandle, &theFindData))
        {
            ::FindClose(theSearchHandle);
            theSearchHandle = INVALID_HANDLE_VALUE;
        }
    }
#else       

    // POSIX version
	// opendir mallocs memory for DIR* so call closedir to free the allocated memory
    DIR* theDir = ::opendir(theModDirName.GetObject());
    if (theDir == NULL)
    {
        QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssMsgNoModuleFolder, 0);  
        return;
    }
    
    while (true)
    {
        // Iterate over each file in the directory, attempting to construct
        // a module object from that file.
        
        struct dirent* theFile = ::readdir(theDir);
        if (theFile == NULL)
            break;
        
        this->CreateModule(theModDirName.GetObject(), theFile->d_name);
    }
	
	(void)::closedir(theDir);
	
#endif
}


/**判断str1是否以str2结尾
 * 如果是返回1
 * 不是返回0
 * 出错返回-1
 * */
int wzd_is_end_with(const char* str1, const char* str2)
{
    if (str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if ((len1 < len2) || (len1 == 0 || len2 == 0))
        return -1;
    while (len2 >= 1)
    {
        if (str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}

//该函数从模块文件创建模块
//  1.	新建模块对象
//  2.	设置模块回调函数, 并调用模块主函数，调用主模块函数，并由主模块函数获入分发函数
//  3.	把模块加入服务器，包括调用模块主函数，建立参数字典，把模块加入模块数组、把并加入队列  
void    QTSServer::CreateModule(char* inModuleFolderPath, char* inModuleName)
{
    // Ignore these silly directory names
        
    if (::strcmp(inModuleName, ".") == 0)
        return;
    if (::strcmp(inModuleName, "..") == 0)
        return;
    if (::strlen(inModuleName) == 0)
        return;
    if (*inModuleName == '.')
        return; // Fix 2572248. Do not attempt to load '.' files as modules at all 
	if (!::wzd_is_end_with(inModuleName, "dll"))
		return;
    //
    // Construct a full path to this module
    UInt32 totPathLen = ::strlen(inModuleFolderPath) + ::strlen(inModuleName);
    OSCharArrayDeleter theModPath(NEW char[totPathLen + 4]);
    ::strcpy(theModPath.GetObject(), inModuleFolderPath);
    ::strcat(theModPath.GetObject(), kPathDelimiterString);
    ::strcat(theModPath.GetObject(), inModuleName);
            
    //
    // Construct a QTSSModule object, and attempt to initialize the module
    QTSSModule* theNewModule = NEW QTSSModule(inModuleName, theModPath.GetObject());	//为模块新建立一个对象
    QTSS_Error theErr = theNewModule->SetupModule(&sCallbacks);
    
    if (theErr != QTSS_NoErr)
    {
        QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssMsgBadModule, theErr,
                                        inModuleName);
        delete theNewModule;
    }
    //
    // If the module was successfully initialized, add it to our module queue
    else if (!this->AddModule(theNewModule))
    {
        QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssMsgRegFailed, theErr,
                                        inModuleName);
        delete theNewModule;
    }
}
//该函数的主要工作就是把模块加入到服务器数据结构中
// 1.	设置主线程数据
// 2.	调用inModule->CallDispatch(QTSS_Register_Role, &theRegParams)。按不同的模块及其角色调用相应的函数. 对于QTSSErrorLogModuleDispatch的QTSS_Register_Role来说，调用QTSSErrorLogModuleDispatch以后，再调用::Register(&inParamBlock->regParams)
// 4.	为模块建立参数字典，允许模块的实例属性
// 5.	把模块加入模块数组，通过this->SetValue(…)
// 6.	把模块加入队列，队列由链表维护
Bool16 QTSServer::AddModule(QTSSModule* inModule)
{
    Assert(inModule->IsInitialized());
    
    // Prepare to invoke the module's Register role. Setup the Register param block
    QTSS_ModuleState theModuleState;

    theModuleState.curModule = inModule;
    theModuleState.curRole = QTSS_Register_Role;
    theModuleState.curTask = NULL;
    OSThread::SetMainThreadData(&theModuleState);
    
    // Currently we do nothing with the module name
    QTSS_RoleParams theRegParams;
    theRegParams.regParams.outModuleName[0] = 0;
    
    // If the module returns an error from the QTSS_Register role, don't put it anywhere
	// 在该分发函数中,就会采用主线程数据, 作为当前模块的数据
    if (inModule->CallDispatch(QTSS_Register_Role, &theRegParams) != QTSS_NoErr)
        return false;
        
    OSThread::SetMainThreadData(NULL);
    
    //
    // Update the module name to reflect what was returned from the register role
    theRegParams.regParams.outModuleName[QTSS_MAX_MODULE_NAME_LENGTH - 1] = 0;
    if (theRegParams.regParams.outModuleName[0] != 0)
        inModule->SetValue(qtssModName, 0, theRegParams.regParams.outModuleName, ::strlen(theRegParams.regParams.outModuleName), false);

    //
    // Give the module object a prefs dictionary. Instance attributes are allowed for these objects.
    QTSSPrefs* thePrefs = NEW QTSSPrefs( sPrefsSource, inModule->GetValue(qtssModName), QTSSDictionaryMap::GetMap(QTSSDictionaryMap::kModulePrefsDictIndex), true);
    thePrefs->RereadPreferences();
    inModule->SetPrefsDict(thePrefs);
        
    //
    // Add this module to the array of module (dictionaries)
    UInt32 theNumModules = this->GetNumValues(qtssSvrModuleObjects);//qtssSvrModuleObjects 模块对象，代表每个模块。
    QTSS_Error theErr = this->SetValue(qtssSvrModuleObjects, theNumModules, &inModule, sizeof(QTSSModule*), QTSSDictionary::kDontObeyReadOnly);//qtssSvrModuleObjects 模块对象，代表每个模块。
    Assert(theErr == QTSS_NoErr);
    
    //
    // Add this module to the module queue
    sModuleQueue.EnQueue(inModule->GetQueueElem());
    
    return true;
}


//由模块队列sModuleQueue构造theIter(类型为)
//1.	调用DestroyModuleRoleArrays();//即删除QTSServer::sModuleArray[x]所指向的内容，见[6.1.7.1.5]
//2.	然后对每种角色遍历(以下x为某种角色，y为该角色的模块下标(即第几个模块))。
//a)	对模块队列遍历，统计该角色对应的模块数存入sNumModulesInRole[x]
//b)	如果模块数>0，则重新对模块队列遍历，
//i.	则所有该角色的模块存入sModuleArray[x][y++]
//实际上这个过程是对模块按注册的角色进行分类的过程。
void QTSServer::BuildModuleRoleArrays()
{
    OSQueueIter theIter(&sModuleQueue);
    QTSSModule* theModule = NULL;
        
    // Make sure these variables are cleaned out in case they've already been inited.
    
    DestroyModuleRoleArrays();

    // Loop through all the roles of all the modules, recording the number of
    // modules in each role, and also recording which modules are doing what.
    
    for (UInt32 x = 0; x < QTSSModule::kNumRoles; x++)
    {
        sNumModulesInRole[x] = 0;
        for (theIter.Reset(); !theIter.IsDone(); theIter.Next())
        {
            theModule = (QTSSModule*)theIter.GetCurrent()->GetEnclosingObject();
            if (theModule->RunsInRole(x))
                sNumModulesInRole[x] += 1;
        }
    
        if (sNumModulesInRole[x] > 0)
        {
            UInt32 moduleIndex = 0;
            sModuleArray[x] = new QTSSModule*[sNumModulesInRole[x] + 1];
            for (theIter.Reset(); !theIter.IsDone(); theIter.Next())
            {
                theModule = (QTSSModule*)theIter.GetCurrent()->GetEnclosingObject();
                if (theModule->RunsInRole(x))
                {
                    sModuleArray[x][moduleIndex] = theModule;
                    moduleIndex++;
                }
            }
        }
    }
}

void QTSServer::DestroyModuleRoleArrays()
{
    for (UInt32 x = 0; x < QTSSModule::kNumRoles; x++)
    {
        sNumModulesInRole[x] = 0;
        if (sModuleArray[x] != NULL)
            delete [] sModuleArray[x];
        sModuleArray[x] = NULL; 
    }
}

// 该函数主要是以注册角色调用模块
void QTSServer::DoInitRole()
{
// 1.	设置theInitParams.initParams
    QTSS_RoleParams theInitParams;
    theInitParams.initParams.inServer =         this;
    theInitParams.initParams.inPrefs =          fSrvrPrefs;
    theInitParams.initParams.inMessages =       fSrvrMessages;
    theInitParams.initParams.inErrorLogStream = &sErrorLogStream;
//2.	设置theModuleState，并调用OSThread::SetMainThreadData(&theModuleState);        
    QTSS_ModuleState theModuleState;
    theModuleState.curRole = QTSS_Initialize_Role;
    theModuleState.curTask = NULL;
    OSThread::SetMainThreadData(&theModuleState);

    //
    // Add the OPTIONS method as the one method the server handles by default (it handles
    // it internally). Modules that handle other RTSP methods will add 
//3.	增加服务器(服务器进行内部处理)默认处理的OPTIONS方法。处理RTSP的模块也将增加
    QTSS_RTSPMethod theOptionsMethod = qtssOptionsMethod;
    (void)this->SetValue(qtssSvrHandledMethods, 0, &theOptionsMethod, sizeof(theOptionsMethod));//qtssSvrHandledMethods 服务器支持的方法。模块应该在QTSS_Initialize_Role角色中将自己支持的方法附加到这个属性上。

	//为了和real兼容，禁止SetParameter。对于SetParameter实现有问题的客户端(如Real播放器)，
	//该代码可删除。目前不必要增加该选项。
	// For now just disable the SetParameter to be compatible with Real.  It should really be removed only for clients that have problems with their SetParameter implementations like (Real Players).
	// At the moment it isn't necesary to add the option.
	//   QTSS_RTSPMethod	theSetParameterMethod = qtssSetParameterMethod;
	//    (void)this->SetValue(qtssSvrHandledMethods, 0, &theSetParameterMethod, sizeof(theSetParameterMethod));//qtssSvrHandledMethods 服务器支持的方法。模块应该在QTSS_Initialize_Role角色中将自己支持的方法附加到这个属性上。

//4.	对所有以初始化角色注册的模块，做以下循环，x = 0 ~ 模块数	
    for (UInt32 x = 0; x < QTSServerInterface::GetNumModulesInRole(QTSSModule::kInitializeRole); x++)
    {
        QTSSModule* theModule = QTSServerInterface::GetModule(QTSSModule::kInitializeRole, x);
        theInitParams.initParams.inModule = theModule;
        theModuleState.curModule = theModule;
        QTSS_Error theErr = theModule->CallDispatch(QTSS_Initialize_Role, &theInitParams);  //调用模块的分发流程，QTSSFileModule

        if (theErr != QTSS_NoErr)
        {
            // If the module reports an error when initializing itself,
            // delete the module and pretend it was never there.
			// 如果初始化模块时出错，则删除模块，就象模块从没有存在过一样
            QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssMsgInitFailed, theErr,
                                            theModule->GetValue(qtssModName)->Ptr);
                                            
            sModuleQueue.Remove(theModule->GetQueueElem());
            delete theModule;
        }
    }
//5.	设置公共头部分this->SetupPublicHeader();
    this->SetupPublicHeader();

    OSThread::SetMainThreadData(NULL);
}

void QTSServer::SetupPublicHeader()
{
    //
    // After the Init role, all the modules have reported the methods that they handle.
    // So, we can prune this attribute for duplicates, and construct a string to use in the
    // Public: header of the OPTIONS response
    QTSS_RTSPMethod* theMethod = NULL;
    UInt32 theLen = 0;

    Bool16 theUniqueMethods[qtssNumMethods + 1];
    ::memset(theUniqueMethods, 0, sizeof(theUniqueMethods));

    for (UInt32 y = 0; this->GetValuePtr(qtssSvrHandledMethods, y, (void**)&theMethod, &theLen) == QTSS_NoErr; y++)//qtssSvrHandledMethods 服务器支持的方法。模块应该在QTSS_Initialize_Role角色中将自己支持的方法附加到这个属性上。
        theUniqueMethods[*theMethod] = true;

    // Rewrite the qtssSvrHandledMethods, eliminating any duplicates that modules may have introduced//qtssSvrHandledMethods 服务器支持的方法。模块应该在QTSS_Initialize_Role角色中将自己支持的方法附加到这个属性上。
    UInt32 uniqueMethodCount = 0;
    for (QTSS_RTSPMethod z = 0; z < qtssNumMethods; z++)
    {
        if (theUniqueMethods[z])
            this->SetValue(qtssSvrHandledMethods, uniqueMethodCount++, &z, sizeof(QTSS_RTSPMethod));//qtssSvrHandledMethods 服务器支持的方法。模块应该在QTSS_Initialize_Role角色中将自己支持的方法附加到这个属性上。
    }
    this->SetNumValues(qtssSvrHandledMethods, uniqueMethodCount);//qtssSvrHandledMethods 服务器支持的方法。模块应该在QTSS_Initialize_Role角色中将自己支持的方法附加到这个属性上。
    
    // Format a text string for the Public: header
    ResizeableStringFormatter theFormatter(NULL, 0);

    for (UInt32 a = 0; this->GetValuePtr(qtssSvrHandledMethods, a, (void**)&theMethod, &theLen) == QTSS_NoErr; a++)//qtssSvrHandledMethods 服务器支持的方法。模块应该在QTSS_Initialize_Role角色中将自己支持的方法附加到这个属性上。
    {
        sPublicHeaderFormatter.Put(RTSPProtocol::GetMethodString(*theMethod));
        sPublicHeaderFormatter.Put(", ");
    }
    sPublicHeaderStr.Ptr = sPublicHeaderFormatter.GetBufPtr();
    sPublicHeaderStr.Len = sPublicHeaderFormatter.GetBytesWritten() - 2; //trunc the last ", "
}


Task*   RTSPListenerSocket::GetSessionTask(TCPSocket** outSocket)
{
    Assert(outSocket != NULL);
    
    // when the server is behing a round robin DNS, the client needs to knwo the IP address ot the server
    // so that it can direct the "POST" half of the connection to the same machine when tunnelling RTSP thru HTTP
    Bool16  doReportHTTPConnectionAddress = QTSServerInterface::GetServer()->GetPrefs()->GetDoReportHTTPConnectionAddress();
    
    RTSPSession* theTask = NEW RTSPSession(doReportHTTPConnectionAddress);
    *outSocket = theTask->GetSocket();  // out socket is not attached to a unix socket yet.

    if (this->OverMaxConnections(0))
        this->SlowDown();
    else
        this->RunNormal();
        
    return theTask;
}


Bool16 RTSPListenerSocket::OverMaxConnections(UInt32 buffer)
{
    QTSServerInterface* theServer = QTSServerInterface::GetServer();
    SInt32 maxConns = theServer->GetPrefs()->GetMaxConnections();
    Bool16 overLimit = false;
    
    if (maxConns > -1) // limit connections
    { 
        maxConns += buffer;
        if  ( (theServer->GetNumRTPSessions() > (UInt32) maxConns) 
              ||
              ( theServer->GetNumRTSPSessions() + theServer->GetNumRTSPHTTPSessions() > (UInt32) maxConns ) 
            )
        {
            overLimit = true;          
        }
    } 
    return overLimit;
     
}


UDPSocketPair*  RTPSocketPool::ConstructUDPSocketPair()
{
    Task* theTask = ((QTSServer*)QTSServerInterface::GetServer())->fRTCPTask;
    
    //construct a pair of UDP sockets, the lower one for RTP data (outgoing only, no demuxer necessary), and one for RTCP data (incoming, so definitely need a demuxer).
    //These are nonblocking sockets that DON'T receive events (we are going to poll for data) They do receive events - we don't poll from them anymore
    //构造一对UDP sockets, 小的这个用于RTP data (向外发数据, 不需要demuxer, 另一个用于发送用于RTCP data (包括incoming, 所以需要demuxer).
    //这些sockets是非阻塞的, 如果他们不接收事件, 我们将poll数据; 如果他们接收事件, 我们则不需要poll他们
    return NEW
        UDPSocketPair(  NEW UDPSocket(theTask, Socket::kNonBlockingSocketType),								//用于RTP
                        NEW UDPSocket(theTask, UDPSocket::kWantsDemuxer | Socket::kNonBlockingSocketType));	//用于RTCP
}

void RTPSocketPool::DestructUDPSocketPair(UDPSocketPair* inPair)
{
    delete inPair->GetSocketA();
    delete inPair->GetSocketB();
    delete inPair;
}

void RTPSocketPool::SetUDPSocketOptions(UDPSocketPair* inPair)
{
    // Apparently the socket buffer size matters even though this is UDP and being
    // used for sending... on UNIX typically the socket buffer size doesn't matter because the
    // packet goes right down to the driver. On Win32 and linux, unless this is really big, we get packet loss.
    inPair->GetSocketA()->SetSocketBufSize(256 * 1024);

    //
    // Always set the Rcv buf size for the RTCP sockets. This is important because the
    // server is going to be getting many many acks.
    UInt32 theRcvBufSize = QTSServerInterface::GetServer()->GetPrefs()->GetRTCPSocketRcvBufSizeinK();

    //
    // In case the rcv buf size is too big for the system, retry, dividing the requested size by 2.
    // Until it works, or until some minimum value is reached.
    OS_Error theErr = EINVAL;
    while ((theErr != OS_NoErr) && (theRcvBufSize > 32))
    {
        theErr = inPair->GetSocketB()->SetSocketRcvBufSize(theRcvBufSize * 1024);
        if (theErr != OS_NoErr)
            theRcvBufSize >>= 1;
    }

    //
    // Report an error if we couldn't set the socket buffer size the user requested
    if (theRcvBufSize != QTSServerInterface::GetServer()->GetPrefs()->GetRTCPSocketRcvBufSizeinK())
    {
        char theRcvBufSizeStr[20];
        qtss_sprintf(theRcvBufSizeStr, "%lu", theRcvBufSize);
        //
        // For now, do not log an error, though we should enable this in the future.
        //QTSSModuleUtils::LogError(qtssWarningVerbosity, qtssMsgSockBufSizesTooLarge, theRcvBufSizeStr);
    }
}


//读取服务器以及所有模块的配置, 设置默认IP, 创建侦听者, 调用所有模块的QTSS_RereadPrefs_Role角色
QTSS_Error QTSServer::RereadPrefsService(QTSS_ServiceFunctionArgsPtr /*inArgs*/)
{
    //
    // This function can only be called safely when the server is completely running.
    // Ensuring this is a bit complicated because of preemption. Here's how it's done...
    
    QTSServerInterface* theServer = QTSServerInterface::GetServer();
    
    // This is to make sure this function isn't being called before the server is
    // completely started up.
    if ((theServer == NULL) || (theServer->GetServerState() != qtssRunningState))
        return QTSS_OutOfState;

    // Because the server must have started up, and because this object always stays
    // around (until the process dies), we can now safely get this object.
    QTSServerPrefs* thePrefs = theServer->GetPrefs();
    
    // Grab the prefs mutex. We want to make sure that calls to RereadPrefsService
    // are serialized. This also prevents the server from shutting down while in
    // this function, because the QTSServer destructor grabs this mutex as well.
    OSMutexLocker locker(thePrefs->GetMutex());
    
    // Finally, check the server state again. The state may have changed
    // to qtssShuttingDownState or qtssFatalErrorState in this time, though
    // at this point we have the prefs mutex, so we are guarenteed that the
    // server can't actually shut down anymore
    if (theServer->GetServerState() != qtssRunningState)
        return QTSS_OutOfState;
    
    // Ok, we're ready to reread preferences now.
    
    //
    // Reread preferences
    sPrefsSource->Parse();
    thePrefs->RereadServerPreferences(true);
    
    {
        // Update listeners, ports, and IP addrs.
        OSMutexLocker locker(theServer->GetServerObjectMutex());
        (void)((QTSServer*)theServer)->SetDefaultIPAddr();
        (void)((QTSServer*)theServer)->CreateListeners(true, thePrefs, 0);
    }
    
    // Delete all the streams
    QTSSModule** theModule = NULL;
    UInt32 theLen = 0;
    
    for (int y = 0; QTSServerInterface::GetServer()->GetValuePtr(qtssSvrModuleObjects, y, (void**)&theModule, &theLen) == QTSS_NoErr; y++)//qtssSvrModuleObjects 模块对象，代表每个模块。
    {
        Assert(theModule != NULL);
        Assert(theLen == sizeof(QTSSModule*));
        
        (*theModule)->GetPrefsDict()->RereadPreferences();

#if DEBUG
        theModule = NULL;
        theLen = 0;
#endif
    }
    
    //
    // Go through each module's prefs object and have those reread as well

    //
    // Now that we are done rereading the prefs, invoke all modules in the RereadPrefs
    // role so they can update their internal prefs caches.
    for (UInt32 x = 0; x < QTSServerInterface::GetNumModulesInRole(QTSSModule::kRereadPrefsRole); x++)
    {
        QTSSModule* theModule = QTSServerInterface::GetModule(QTSSModule::kRereadPrefsRole, x);
        (void)theModule->CallDispatch(QTSS_RereadPrefs_Role, NULL);
    }
    return QTSS_NoErr;
}


