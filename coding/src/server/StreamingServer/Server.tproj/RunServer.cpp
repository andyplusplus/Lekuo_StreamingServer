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
    File:       main.cpp

    Contains:   main function to drive streaming server.
*/

#include <errno.h>

#include "RunServer.h"
#include "SafeStdLib.h"
#include "OS.h"
#include "OSMemory.h"
#include "OSThread.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "ev.h"
#include "OSArrayObjectDeleter.h"
#include "Task.h"
#include "IdleTask.h"
#include "TimeoutTask.h"
#include "DateTranslator.h"
#include "QTSSRollingLog.h"
//Wuzengde
#include "ServerControl.h"
//

#ifndef __Win32__
    #include <sys/types.h>
    #include <unistd.h>
#endif
#include "QTSServerInterface.h"
#include "QTSServer.h"

#include <stdlib.h>
#include <sys/stat.h>

QTSServer* sServer = NULL;
int sStatusUpdateInterval = 0;
Bool16 sHasPID = false;
UInt64 sLastStatusPackets = 0;
UInt64 sLastDebugPackets = 0;
SInt64 sLastDebugTotalQuality = 0;
#ifdef __sgi__ 
#include <sched.h>
#endif

//启动服务器
QTSS_ServerState StartServer(XMLPrefsParser* inPrefsSource, PrefsSource* inMessagesSource, UInt16 inPortOverride, int statsUpdateInterval, QTSS_ServerState inInitialState, Bool16 inDontFork, UInt32 debugLevel, UInt32 debugOptions)
{
    //Mark when we are done starting up. If auto-restart is enabled, we want to make sure
    //to always exit with a status of 0 if we encountered a problem WHILE STARTING UP. This
    //will prevent infinite-auto-restart-loop type problems
    Bool16 doneStartingUp = false;
    QTSS_ServerState theServerState = qtssStartingUpState;
    
    sStatusUpdateInterval = statsUpdateInterval;
//1. 初始化相关参数, 并创建QTSServer
    //Initialize utility classes
    OS::Initialize();			//该函数只调用一次，只是初始化一些时间变量
    OSThread::Initialize();		//分配Thread Local Storage
    Socket::Initialize();		//创建事件线程
    SocketUtils::Initialize(!inDontFork);	//该函数主要目的是把IP地址列表存入sIPAddrInfoArray数组
											//inDontFork = 0, 表示不查找DNS名字;

#if !MACOSXEVENTQUEUE
    ::select_startevents();//initialize the select() implementation of the event queue
#endif
    
	//
    //start the server
	//
	//首先, 构造所有静态字典对象Map, 并按照其属性数(每个Map的属性预定好)分配指针的存贮空间, 申请存贮空间, sDictionaryMaps指向这些字典对象Map
    QTSSDictionaryMap::Initialize();	
    QTSServerInterface::Initialize();	//定义ServerDict，QTSSConnectedUserDict字典对象，sServerHeaderPtr，
	        //该函数必须在构建服务器对象以前调用
    sServer = NEW QTSServer();			//新建QTSServer对象, 服务器对象直接使用前面建立的对象字典, 
			//并在服务器属性和字典元素之间建立关联
    sServer->SetDebugLevel(debugLevel);
    sServer->SetDebugOptions(debugOptions);
    inPrefsSource->Parse();				//重新解析XML配置文件???为什么要重新解析??? 解析streamingserver.xml

    Bool16 createListeners = true;
    if (qtssShuttingDownState == inInitialState) 
        createListeners = false;
//2. 初始化QTSServer, 主要包括以下几个方面的工作:
	//   1. 回调初始化, 字典初始化
	//   2. 创建QTSS消息对象, 参数对象, 并初始化QTSSModuleUtils
	//   3. 创建RTPSocketPool, 包括发送者端口,以及接收者端口
	//   4. 通过CreateListeners创建RTSP侦听端口, 至此, 已经可以接收RTSP请求了
    sServer->Initialize(inPrefsSource, inMessagesSource, inPortOverride,createListeners);

    if (inInitialState == qtssShuttingDownState)
    {  
        sServer->InitModules(inInitialState);
        return inInitialState;
    }
    
    OSCharArrayDeleter runGroupName(sServer->GetPrefs()->GetRunGroupName());
    OSCharArrayDeleter runUserName(sServer->GetPrefs()->GetRunUserName());
    OSThread::SetPersonality(runUserName.GetObject(), runGroupName.GetObject());
//3. 创建任务线程, 初始化超时任务
    if (sServer->GetServerState() != qtssFatalErrorState)
    {
        UInt32 numThreads = 0;
        
        if (OS::ThreadSafe())
        {
            numThreads = sServer->GetPrefs()->GetNumThreads(); // whatever the prefs say
            if (numThreads == 0)
                numThreads = OS::GetNumProcessors(); // 1 worker thread per processor
        }
        if (numThreads == 0)
            numThreads = 1;

        TaskThreadPool::AddThreads(numThreads);

    #if DEBUG
        qtss_printf("Number of task threads: %lu\n",numThreads);
    #endif
    
        // Start up the server's global tasks, and start listening
        TimeoutTask::Initialize();		//TimeoutTask基于任务机制，必须在增加任务线程以后, 
										//启动socket服务器之前，执行该操作
     }

/////////////////////////////////////////////////
//到此为止,服务器启动已经完成////////////////////
/////////////////////////////////////////////////
//4. 服务器启动完成后, 做以下方面的工作: 
//		1. 创建并启动sIdleThread, sIdleThread是IdleTask的静态变量
//		2. 创建并启动启动事件线程, sEventThread, sEventThread是socket类的成员变量

	/*确保最后执行以下操作。因为以下的这些线程是在服务器中工作的线程, 并确保在服务器启动期间不做具体工作*/
    if (sServer->GetServerState() != qtssFatalErrorState)
    {
        IdleTask::Initialize();
        Socket::StartThread();
        OSThread::Sleep(1000);
        
        // 在Win32平台上，为了调用modwatch，必须先建立Socket EventQueue线程。
        // 模块从其initializer调用modwatch，并且不希望阻止模块执行该操作，
        // 所以把模块初始化操作从其它初始化操作分离出来，因此我们先启动Socket EventQueue线程。由于还没有任何TaskThread， 
        // 所以还是可以阻止服务器执行这样的操作。

        sServer->InitModules(inInitialState);	//加载动态,静态模块, 构造模块数组,并以初始化角色调用该模块
		sServer->StartTasks();		//该函数主要实现:  1. 启动RTCPTask任务  2. 启动RTPStatsUpdaterTask任务  3. 启动RTSP事件通知机制, 通过fListeners[x]->RequestEvent(EV_RE);
        sServer->SetupUDPSockets(); //为每个本地IP建立UDP端口对(一个端口用于RTP, 另一个端口用于RTCP),并且在该端口上启动事件通知机制, udp sockets在RTCP任务初始化以后建立 are set up after the rtcp task is instantiated
        theServerState = sServer->GetServerState();
    }

    if (theServerState != qtssFatalErrorState)
    {
        CleanPid(true);
        WritePid(!inDontFork);

        doneStartingUp = true;
        qtss_printf("Streaming Server done starting up\n");
        OSMemory::SetMemoryError(ENOMEM);
    }


    // SWITCH TO RUN USER AND GROUP ID
    if (!sServer->SwitchPersonality())
        theServerState = qtssFatalErrorState;

   //
    // Tell the caller whether the server started up or not
    return theServerState;
}

void WritePid(Bool16 forked)
{
#ifndef __Win32__
    // WRITE PID TO FILE
    OSCharArrayDeleter thePidFileName(sServer->GetPrefs()->GetPidFilePath());
    FILE *thePidFile = fopen(thePidFileName, "w");
    if(thePidFile)
    {
        if (!forked)
            fprintf(thePidFile,"%d\n",getpid());    // write own pid
        else
        {
            fprintf(thePidFile,"%d\n",getppid());    // write parent pid
            fprintf(thePidFile,"%d\n",getpid());    // and our own pid in the next line
        }                
        fclose(thePidFile);
        sHasPID = true;
    }
#endif
}

void CleanPid(Bool16 force)
{
#ifndef __Win32__
    if (sHasPID || force)
    {
        OSCharArrayDeleter thePidFileName(sServer->GetPrefs()->GetPidFilePath());
        unlink(thePidFileName);
    }
#endif
}
void LogStatus(QTSS_ServerState theServerState)
{
    static QTSS_ServerState lastServerState = 0;
    static char *sPLISTHeader[] =
    {     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>",
#if __MacOSX__
        "<!DOCTYPE plist SYSTEM \"file://localhost/System/Library/DTDs/PropertyList.dtd\">",
#else
        "<!ENTITY % plistObject \"(array | data | date | dict | real | integer | string | true | false )\">",
        "<!ELEMENT plist %plistObject;>",
        "<!ATTLIST plist version CDATA \"0.9\">",
        "",
        "<!-- Collections -->",
        "<!ELEMENT array (%plistObject;)*>",
        "<!ELEMENT dict (key, %plistObject;)*>",
        "<!ELEMENT key (#PCDATA)>",
        "",
        "<!--- Primitive types -->",
        "<!ELEMENT string (#PCDATA)>",
        "<!ELEMENT data (#PCDATA)> <!-- Contents interpreted as Base-64 encoded -->",
        "<!ELEMENT date (#PCDATA)> <!-- Contents should conform to a subset of ISO 8601 (in particular, YYYY '-' MM '-' DD 'T' HH ':' MM ':' SS 'Z'.  Smaller units may be omitted with a loss of precision) -->",
        "",
        "<!-- Numerical primitives -->",
        "<!ELEMENT true EMPTY>  <!-- Boolean constant true -->",
        "<!ELEMENT false EMPTY> <!-- Boolean constant false -->",
        "<!ELEMENT real (#PCDATA)> <!-- Contents should represent a floating point number matching (\"+\" | \"-\")? d+ (\".\"d*)? (\"E\" (\"+\" | \"-\") d+)? where d is a digit 0-9.  -->",
        "<!ELEMENT integer (#PCDATA)> <!-- Contents should represent a (possibly signed) integer number in base 10 -->",
        "]>",
#endif
    };

    static int numHeaderLines = sizeof(sPLISTHeader) / sizeof(char*);

    static char*    sPlistStart = "<plist version=\"0.9\">";
    static char*    sPlistEnd = "</plist>";
    static char*    sDictStart = "<dict>";
    static char*    sDictEnd = "</dict>";
    
    static char*    sKey    = "     <key>%s</key>\n";
    static char*    sValue  = "     <string>%s</string>\n";
    
    static char *sAttributes[] =
    {
        "qtssSvrServerName",
        "qtssSvrServerVersion",
        "qtssSvrServerBuild",
        "qtssSvrServerPlatform",
        "qtssSvrRTSPServerComment",
        "qtssSvrServerBuildDate",
        "qtssSvrStartupTime",
        "qtssSvrCurrentTimeMilliseconds",//qtssSvrCurrentTimeMilliseconds 服务器当前的时间，以毫秒计。获取这个属性的值等同于调用//QTSS_Milliseconds函数。
        "qtssSvrCPULoadPercent",
         "qtssSvrState",
        "qtssRTPSvrCurConn",
        "qtssRTSPCurrentSessionCount",
        "qtssRTSPHTTPCurrentSessionCount",
        "qtssRTPSvrCurBandwidth",
        "qtssRTPSvrCurPackets",
        "qtssRTPSvrTotalConn",
        "qtssRTPSvrTotalBytes",
        "qtssMP3SvrCurConn",
        "qtssMP3SvrTotalConn",
        "qtssMP3SvrCurBandwidth",
        "qtssMP3SvrTotalBytes"
    };
    static int numAttributes = sizeof(sAttributes) / sizeof(char*);
        
    static StrPtrLen statsFileNameStr("server_status");    
    
    if (false == sServer->GetPrefs()->ServerStatFileEnabled())
        return;
        
    UInt32 interval = sServer->GetPrefs()->GetStatFileIntervalSec();
    if (interval == 0 || (OS::UnixTime_Secs() % interval) > 0 ) 
        return;
    
    // If the total number of RTSP sessions is 0  then we 
    // might not need to update the "server_status" file.
    char* thePrefStr = NULL;
    // We start lastRTSPSessionCount off with an impossible value so that
    // we force the "server_status" file to be written at least once.
    static int lastRTSPSessionCount = -1; 
    // Get the RTSP session count from the server.
    (void)QTSS_GetValueAsString(sServer, qtssRTSPCurrentSessionCount, 0, &thePrefStr);
    int currentRTSPSessionCount = ::atoi(thePrefStr);
    delete [] thePrefStr; thePrefStr = NULL;
    if (currentRTSPSessionCount == 0 && currentRTSPSessionCount == lastRTSPSessionCount)
    {
        // we don't need to update the "server_status" file except the
        // first time we are in the idle state.
        if (theServerState == qtssIdleState && lastServerState == qtssIdleState)
        {
            lastRTSPSessionCount = currentRTSPSessionCount;
            lastServerState = theServerState;
            return;
        }
    }
    else
    {
        // save the RTSP session count for the next time we execute.
        lastRTSPSessionCount = currentRTSPSessionCount;
    }

    StrPtrLenDel pathStr(sServer->GetPrefs()->GetErrorLogDir());
    StrPtrLenDel fileNameStr(sServer->GetPrefs()->GetStatsMonitorFileName());
    ResizeableStringFormatter pathBuffer(NULL,0);
    pathBuffer.PutFilePath(&pathStr,&fileNameStr);
    pathBuffer.PutTerminator();
    
    char*   filePath = pathBuffer.GetBufPtr();    
    FILE*   statusFile = ::fopen(filePath, "w");
    char*   theAttributeValue = NULL;
    int     i;
    
    if (statusFile != NULL)
    {
        ::chmod(filePath, 0640);
        for ( i = 0; i < numHeaderLines; i++)
        {    
            qtss_fprintf(statusFile, "%s\n",sPLISTHeader[i]);    
        }

        qtss_fprintf(statusFile, "%s\n", sPlistStart);
        qtss_fprintf(statusFile, "%s\n", sDictStart);    

          // show each element value
         for ( i = 0; i < numAttributes; i++)
        {
            (void)QTSS_GetValueAsString(sServer, QTSSModuleUtils::GetAttrID(sServer,sAttributes[i]), 0, &theAttributeValue);
            if (theAttributeValue != NULL)
             {
                qtss_fprintf(statusFile, sKey, sAttributes[i]);    
               qtss_fprintf(statusFile, sValue, theAttributeValue);    
                delete [] theAttributeValue;
                theAttributeValue = NULL;
             }
         }
                  
        qtss_fprintf(statusFile, "%s\n", sDictEnd);
        qtss_fprintf(statusFile, "%s\n\n", sPlistEnd);    
         
          ::fclose(statusFile);
    }
    lastServerState = theServerState;
}

void print_status(FILE* file, FILE* console, char* format, char* theStr)
{
    if (file) qtss_fprintf(file, format, theStr);
    if (console) qtss_fprintf(console, format, theStr);

}

void DebugLevel_1(FILE*   statusFile, FILE*   stdOut,  Bool16 printHeader )
{
    char*  thePrefStr = NULL;
    static char numStr[12] ="";
    static char dateStr[25] ="";
    UInt32 theLen = 0;

    if ( printHeader )
    {                   
   
        print_status(statusFile,stdOut,"%s", "     RTP-Conns RTSP-Conns HTTP-Conns  kBits/Sec   Pkts/Sec   RTP-Playing   AvgDelay CurMaxDelay  MaxDelay  AvgQuality  NumThinned      Time\n");

    }
    
    (void)QTSS_GetValueAsString(sServer, qtssRTPSvrCurConn, 0, &thePrefStr);
    print_status(statusFile, stdOut,"%11s", thePrefStr);
    
    delete [] thePrefStr; thePrefStr = NULL;
    
    (void)QTSS_GetValueAsString(sServer, qtssRTSPCurrentSessionCount, 0, &thePrefStr);
    print_status(statusFile, stdOut,"%11s", thePrefStr);
    delete [] thePrefStr; thePrefStr = NULL;
    
    (void)QTSS_GetValueAsString(sServer, qtssRTSPHTTPCurrentSessionCount, 0, &thePrefStr);
    print_status(statusFile, stdOut,"%11s", thePrefStr);
    delete [] thePrefStr; thePrefStr = NULL;
    
    UInt32 curBandwidth = 0;
    theLen = sizeof(curBandwidth);
    (void)QTSS_GetValue(sServer, qtssRTPSvrCurBandwidth, 0, &curBandwidth, &theLen);
    qtss_snprintf(numStr, 11, "%lu", curBandwidth/1024);
    print_status(statusFile, stdOut,"%11s", numStr);

    (void)QTSS_GetValueAsString(sServer, qtssRTPSvrCurPackets, 0, &thePrefStr);
    print_status(statusFile, stdOut,"%11s", thePrefStr);
    delete [] thePrefStr; thePrefStr = NULL;


    UInt32 currentPlaying = sServer->GetNumRTPPlayingSessions();
    qtss_snprintf( numStr, sizeof(numStr) -1, "%lu", currentPlaying);
    print_status(statusFile, stdOut,"%14s", numStr);

   
    //is the server keeping up with the streams?
    //what quality are the streams?
    SInt64 totalRTPPaackets = sServer->GetTotalRTPPackets();
    SInt64 deltaPackets = totalRTPPaackets - sLastDebugPackets;
    sLastDebugPackets = totalRTPPaackets;

    SInt64 totalQuality = sServer->GetTotalQuality();
    SInt64 deltaQuality = totalQuality - sLastDebugTotalQuality;
    sLastDebugTotalQuality = totalQuality;

    SInt64 currentMaxLate =  sServer->GetCurrentMaxLate();
    SInt64 totalLate =  sServer->GetTotalLate();

    sServer->ClearTotalLate();
    sServer->ClearCurrentMaxLate();
    sServer->ClearTotalQuality();
    
    ::qtss_snprintf(numStr, sizeof(numStr) -1, "%s", "0");
    if (deltaPackets > 0)
        qtss_snprintf(numStr, sizeof(numStr) -1, "%ld", (SInt32) ((SInt64)totalLate /  (SInt64) deltaPackets ));
    print_status(statusFile, stdOut,"%11s", numStr);

    qtss_snprintf(numStr,sizeof(numStr) -1, "%ld", (SInt32) currentMaxLate);
    print_status(statusFile, stdOut,"%11s", numStr);
    
    qtss_snprintf(numStr,sizeof(numStr) -1, "%ld", (SInt32)  sServer->GetMaxLate() );
    print_status(statusFile, stdOut,"%11s", numStr);

    ::qtss_snprintf(numStr, sizeof(numStr) -1, "%s", "0");
    if (deltaPackets > 0)
        qtss_snprintf(numStr, sizeof(numStr) -1, "%ld", (SInt32) ((SInt64) deltaQuality /  (SInt64) deltaPackets));
    print_status(statusFile, stdOut,"%11s", numStr);

    qtss_snprintf(numStr,sizeof(numStr) -1, "%ld", (SInt32) sServer->GetNumThinned() );
    print_status(statusFile, stdOut,"%11s", numStr);

    
    
    char theDateBuffer[QTSSRollingLog::kMaxDateBufferSizeInBytes];
    (void) QTSSRollingLog::FormatDate(theDateBuffer, false);
    
    qtss_snprintf(dateStr,sizeof(dateStr) -1, "%s", theDateBuffer );
    print_status(statusFile, stdOut,"%24s\n", dateStr);
}

FILE* LogDebugEnabled()
{

    if (DebugLogOn(sServer))
    {
        static StrPtrLen statsFileNameStr("server_debug_status");    
    
        StrPtrLenDel pathStr(sServer->GetPrefs()->GetErrorLogDir());
        ResizeableStringFormatter pathBuffer(NULL,0);
        pathBuffer.PutFilePath(&pathStr,&statsFileNameStr);
        pathBuffer.PutTerminator();
        
        char*   filePath = pathBuffer.GetBufPtr();    
        return ::fopen(filePath, "a");
    }
    
    return NULL;
}


FILE* DisplayDebugEnabled()
{        
    return ( DebugDisplayOn(sServer) ) ? stdout   : NULL ;
}


void DebugStatus(UInt32 debugLevel, Bool16 printHeader)
{
        
    FILE*   statusFile = LogDebugEnabled();
    FILE*   stdOut = DisplayDebugEnabled();
    
    if (debugLevel > 0)
        DebugLevel_1(statusFile, stdOut, printHeader);

    if (statusFile) 
        ::fclose(statusFile);
}

void FormattedTotalBytesBuffer(char *outBuffer, int outBufferLen, UInt64 totalBytes)
{
    Float32 displayBytes = 0.0;
    char  sizeStr[] = "B";
    char* format = NULL;
        
    if (totalBytes > 1073741824 ) //GBytes
    {   displayBytes = (Float32) ( (Float64) (SInt64) totalBytes /  (Float64) (SInt64) 1073741824 );
        sizeStr[0] = 'G';
        format = "%.4f%s ";
     }
    else if (totalBytes > 1048576 ) //MBytes
    {   displayBytes = (Float32) (SInt32) totalBytes /  (Float32) (SInt32) 1048576;
        sizeStr[0] = 'M';
        format = "%.3f%s ";
     }
    else if (totalBytes > 1024 ) //KBytes
    {    displayBytes = (Float32) (SInt32) totalBytes /  (Float32) (SInt32) 1024;
         sizeStr[0] = 'K';
         format = "%.2f%s ";
    }
    else
    {    displayBytes = (Float32) (SInt32) totalBytes;  //Bytes
         sizeStr[0] = 'B';
         format = "%4.0f%s ";
    }
    
    outBuffer[outBufferLen -1] = 0;
    qtss_snprintf(outBuffer, outBufferLen -1,  format , displayBytes, sizeStr);
}

void PrintStatus(Bool16 printHeader)
{
    char* thePrefStr = NULL;
    UInt32 theLen = 0;
    
    if ( printHeader )
    {                       
        qtss_printf("     RTP-Conns RTSP-Conns HTTP-Conns  kBits/Sec   Pkts/Sec    TotConn     TotBytes   TotPktsLost          Time\n");   
    }

    (void)QTSS_GetValueAsString(sServer, qtssRTPSvrCurConn, 0, &thePrefStr);
    qtss_printf( "%11s", thePrefStr);
    delete [] thePrefStr; thePrefStr = NULL;
    
    (void)QTSS_GetValueAsString(sServer, qtssRTSPCurrentSessionCount, 0, &thePrefStr);
    qtss_printf( "%11s", thePrefStr);
    delete [] thePrefStr; thePrefStr = NULL;
    
    (void)QTSS_GetValueAsString(sServer, qtssRTSPHTTPCurrentSessionCount, 0, &thePrefStr);
    qtss_printf( "%11s", thePrefStr);
    delete [] thePrefStr; thePrefStr = NULL;
    
    UInt32 curBandwidth = 0;
    theLen = sizeof(curBandwidth);
    (void)QTSS_GetValue(sServer, qtssRTPSvrCurBandwidth, 0, &curBandwidth, &theLen);
    qtss_printf( "%11lu", curBandwidth/1024);
    
    (void)QTSS_GetValueAsString(sServer, qtssRTPSvrCurPackets, 0, &thePrefStr);
    qtss_printf( "%11s", thePrefStr);
    delete [] thePrefStr; thePrefStr = NULL;
    
    (void)QTSS_GetValueAsString(sServer, qtssRTPSvrTotalConn, 0, &thePrefStr);
    qtss_printf( "%11s", thePrefStr);
    delete [] thePrefStr; thePrefStr = NULL;
    
    UInt64 totalBytes = sServer->GetTotalRTPBytes();
    char  displayBuff[32] = "";
    FormattedTotalBytesBuffer(displayBuff, sizeof(displayBuff),totalBytes);
    qtss_printf( "%17s", displayBuff);
    
    qtss_printf( "%11" _64BITARG_ "u", sServer->GetTotalRTPPacketsLost());
                    
    char theDateBuffer[QTSSRollingLog::kMaxDateBufferSizeInBytes];
    (void) QTSSRollingLog::FormatDate(theDateBuffer, false);
    qtss_printf( "%25s",theDateBuffer);
    
    qtss_printf( "\n");
    
}

Bool16 PrintHeader(UInt32 loopCount)
{
     return ( (loopCount % (sStatusUpdateInterval * 10) ) == 0 ) ? true : false;
}

Bool16 PrintLine(UInt32 loopCount)
{
     return ( (loopCount % sStatusUpdateInterval) == 0 ) ? true : false;
}


void RunServer()
{   
    Bool16 restartServer = false;
    UInt32 loopCount = 0;
    UInt32 debugLevel = 0;
    Bool16 printHeader = false;
    Bool16 printStatus = false;


    //just wait until someone stops the server or a fatal error occurs.
    QTSS_ServerState theServerState = sServer->GetServerState();
    while ((theServerState != qtssShuttingDownState) &&
            (theServerState != qtssFatalErrorState))
    {
		//Start WUZENGDE 2007.12.08 为了增加程序的可测试性，增加了停止视频服务器的功能
		if (CServerControl::getServerControlAndReset() == SERVER_CONTROL_STATUS_STOP)	//通过读初始化文件，获知是否需要停止服务器
		{
			theServerState = qtssShuttingDownState;
			QTSServerInterface::GetServer()->SetSigInt();
			QTSServerInterface::GetServer()->SetSigTerm();
		}
		//End

#ifdef __sgi__
        OSThread::Sleep(999);
#else
        OSThread::Sleep(1000);
#endif
        LogStatus(theServerState);

        if (sStatusUpdateInterval)
        {
            debugLevel = sServer->GetDebugLevel();             
            printHeader = PrintHeader(loopCount);
            printStatus = PrintLine(loopCount);
                
            if (printStatus)
            {
                if  (DebugOn(sServer) ) // debug level display or logging is on
                    DebugStatus(debugLevel, printHeader);
                
                if (!DebugDisplayOn(sServer))
                    PrintStatus(printHeader); // default status output
            }

            loopCount++;

        }
        
        if ((sServer->SigIntSet()) || (sServer->SigTermSet()))
        {
            // start the shutdown process
            theServerState = qtssShuttingDownState;
            (void)QTSS_SetValue(QTSServerInterface::GetServer(), qtssSvrState, 0, &theServerState, sizeof(theServerState));

            if (sServer->SigIntSet())
                restartServer = true;
        }
        
        theServerState = sServer->GetServerState();
        if (theServerState == qtssIdleState)
            sServer->KillAllRTPSessions();
    }
    
    //
    // Kill all the sessions and wait for them to die,
    // but don't wait more than 5 seconds
    sServer->KillAllRTPSessions();
    for (UInt32 shutdownWaitCount = 0; (sServer->GetNumRTPSessions() > 0) && (shutdownWaitCount < 5); shutdownWaitCount++)
        OSThread::Sleep(1000);
        
    //Now, make sure that the server can't do any work
    TaskThreadPool::RemoveThreads();
    
    //now that the server is definitely stopped, it is safe to initate
    //the shutdown process
    delete sServer;
    
    CleanPid(false);
    //ok, we're ready to exit. If we're quitting because of some fatal error
    //while running the server, make sure to let the parent process know by
    //exiting with a nonzero status. Otherwise, exit with a 0 status
    if (theServerState == qtssFatalErrorState || restartServer)
        ::exit (-2);//-2 signals parent process to restart server
}


//   _WZD_CLIENT_DLL_Start         _WZD_MEM_Start

	//Only called by RelayClient DLL to Stop the Server, and clean the resources, 
void StopServer()
{
//	// 
//        LogStatus(theServerState);
//
//        if (sStatusUpdateInterval)
//        {
//
//			if ((sServer->SigIntSet()) || (sServer->SigTermSet()))
//			{
//				//
//				// start the shutdown process
//				theServerState = qtssShuttingDownState;
//				(void)QTSS_SetValue(QTSServerInterface::GetServer(), qtssSvrState, 0, &theServerState, sizeof(theServerState));
//
//				if (sServer->SigIntSet())
//					restartServer = true;
//			}
//        
//			theServerState = sServer->GetServerState();
//			if (theServerState == qtssIdleState)
//				sServer->KillAllRTPSessions();
//
//		}
//
//		// Kill all the sessions and wait for them to die,
//		// but don't wait more than 5 seconds
//		sServer->KillAllRTPSessions();
//		for (UInt32 shutdownWaitCount = 0; (sServer->GetNumRTPSessions() > 0) && (shutdownWaitCount < 5); shutdownWaitCount++)
//			OSThread::Sleep(1000);
//        
//		//Now, make sure that the server can't do any work
//		TaskThreadPool::RemoveThreads();
//    
//		//now that the server is definitely stopped, it is safe to initate
//		//the shutdown process
//		delete sServer;
//    
//		CleanPid(false);
//		//ok, we're ready to exit. If we're quitting because of some fatal error
//		//while running the server, make sure to let the parent process know by
//		//exiting with a nonzero status. Otherwise, exit with a 0 status
//		if (theServerState == qtssFatalErrorState || restartServer)
//			::exit (-2);//-2 signals parent process to restart server

	
};


//   _WZD_CLIENT_DLL_End           _WZD_MEM_End

