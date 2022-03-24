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
    File:       QTSServerInterface.h

    Contains:   This object defines an interface for getting and setting server-wide
                attributes, and storing global server resources.
                
                There can be only one of these objects per process, so there
                is a static accessor.

该对象定义了设置和读取服务器属性，并存贮全局服务器资源的接口
每个处理机只能有一个这样的对象，所以该对象是一个静态accessor。

*/


#ifndef __QTSSERVERINTERFACE_H__
#define __QTSSERVERINTERFACE_H__

#include "QTSS.h"
#include "QTSSDictionary.h"
#include "QTSServerPrefs.h"
#include "QTSSMessages.h"
#include "QTSSModule.h"
#include "atomic.h"

#include "OSMutex.h"
#include "Task.h"
#include "TCPListenerSocket.h"
#include "ResizeableStringFormatter.h"

// OSRefTable;
class UDPSocketPool;
class QTSServerPrefs;
class QTSSMessages;
//class RTPStatsUpdaterTask;
class RTPSessionInterface;

// This object also functions as our assert logger
// 作为错误日志管理
class QTSSErrorLogStream : public QTSSStream, public AssertLogger
{
    public:
    
        // This QTSSStream is used by modules to write to the error log
    
        QTSSErrorLogStream() {}
        virtual ~QTSSErrorLogStream() {}
        
        virtual QTSS_Error  Write(void* inBuffer, UInt32 inLen, UInt32* outLenWritten, UInt32 inFlags);
        virtual void        LogAssert(char* inMessage);
};

class QTSServerInterface : public QTSSDictionary
{
    public:
    
        //Initialize must be called right off the bat to initialize dictionary resources
		//初始化字典资源，设置sServerHeaderPtr，如:
		// "Server: DSS/5.5.1 (Build/4898; Platform/ Win32; Release/Darwin;)"
        static void     Initialize();

        //
        // CONSTRUCTOR / DESTRUCTOR
        
        QTSServerInterface();
        virtual ~QTSServerInterface() {}
        
        //
        //
        // STATISTICS MANIPULATION
        // These functions are how the server keeps its statistics current
        
        void                AlterCurrentRTSPSessionCount(SInt32 inDifference)
            { OSMutexLocker locker(&fMutex); fNumRTSPSessions += inDifference; }
        void                AlterCurrentRTSPHTTPSessionCount(SInt32 inDifference)
            { OSMutexLocker locker(&fMutex); fNumRTSPHTTPSessions += inDifference; }
        void                SwapFromRTSPToHTTP()
            { OSMutexLocker locker(&fMutex); fNumRTSPSessions--; fNumRTSPHTTPSessions++; }
            
        //total rtp bytes sent by the server
        void            IncrementTotalRTPBytes(UInt32 bytes)
                                        { (void)atomic_add(&fPeriodicRTPBytes, bytes); }
        //total rtp packets sent by the server
        void            IncrementTotalPackets()
                                        { (void)atomic_add(&fPeriodicRTPPackets, 1); }
        //total rtp bytes reported as lost by the clients
        void            IncrementTotalRTPPacketsLost(UInt32 packets)
                                        { (void)atomic_add(&fPeriodicRTPPacketsLost, packets); }
                                        
        // Also increments current RTP session count
        void            IncrementTotalRTPSessions()
            { OSMutexLocker locker(&fMutex); fNumRTPSessions++; fTotalRTPSessions++; }            
        void            AlterCurrentRTPSessionCount(SInt32 inDifference)
            { OSMutexLocker locker(&fMutex); fNumRTPSessions += inDifference; }

        //track how many sessions are playing
        void            AlterRTPPlayingSessions(SInt32 inDifference)
            { OSMutexLocker locker(&fMutex); fNumRTPPlayingSessions += inDifference; }
            
        
        void            IncrementTotalLate(SInt64 milliseconds)
           {    OSMutexLocker locker(&fMutex); 
                fTotalLate += milliseconds;
                if (milliseconds > fCurrentMaxLate) fCurrentMaxLate = milliseconds;
                if (milliseconds > fMaxLate) fMaxLate = milliseconds;
           }
           
        void            IncrementTotalQuality(SInt32 level)
           { OSMutexLocker locker(&fMutex); fTotalQuality += level; }
           
           
        void            IncrementNumThinned(SInt32 inDifference)
           { OSMutexLocker locker(&fMutex); fNumThinned += inDifference; }

        void            ClearTotalLate()
           { OSMutexLocker locker(&fMutex); fTotalLate = 0;  }
        void            ClearCurrentMaxLate()
           { OSMutexLocker locker(&fMutex); fCurrentMaxLate = 0;  }
        void            ClearTotalQuality()
           { OSMutexLocker locker(&fMutex); fTotalQuality = 0;  }
     

        //
        // ACCESSORS
        
        QTSS_ServerState    GetServerState()        { return fServerState; }
        UInt32              GetNumRTPSessions()     { return fNumRTPSessions; }
        UInt32              GetNumRTSPSessions()    { return fNumRTSPSessions; }
        UInt32              GetNumRTSPHTTPSessions(){ return fNumRTSPHTTPSessions; }
        
        UInt32              GetTotalRTPSessions()   { return fTotalRTPSessions; }
        UInt32              GetNumRTPPlayingSessions()   { return fNumRTPPlayingSessions; }
        
        UInt32              GetCurBandwidthInBits() { return fCurrentRTPBandwidthInBits; }
        UInt32              GetAvgBandwidthInBits() { return fAvgRTPBandwidthInBits; }
        UInt32              GetRTPPacketsPerSec()   { return fRTPPacketsPerSecond; }
        UInt64              GetTotalRTPBytes()      { return fTotalRTPBytes; }
        UInt64              GetTotalRTPPacketsLost(){ return fTotalRTPPacketsLost; }
        UInt64              GetTotalRTPPackets()    { return fTotalRTPPackets; }
        Float32             GetCPUPercent()         { return fCPUPercent; }
        Bool16              SigIntSet()             { return fSigInt; }
        Bool16				SigTermSet()			{ return fSigTerm; }
		
        UInt32              GetNumMP3Sessions()     { return fNumMP3Sessions; }
        UInt32              GetTotalMP3Sessions()   { return fTotalMP3Sessions; }
        UInt64              GetTotalMP3Bytes()      { return fTotalMP3Bytes; }
        
        UInt32              GetDebugLevel()                     { return fDebugLevel; }
        UInt32              GetDebugOptions()                   { return fDebugOptions; }
        void                SetDebugLevel(UInt32 debugLevel)    { fDebugLevel = debugLevel; }
        void                SetDebugOptions(UInt32 debugOptions){ fDebugOptions = debugOptions; }
        
        SInt64          GetMaxLate()                { return fMaxLate; };
        SInt64          GetTotalLate()              { return fTotalLate; };
        SInt64          GetCurrentMaxLate()         { return fCurrentMaxLate; };
        SInt64          GetTotalQuality()           { return fTotalQuality; };
        SInt32          GetNumThinned()             { return fNumThinned; };

        //
        //
        // GLOBAL OBJECTS REPOSITORY
        // This object is in fact global, so there is an accessor for it as well.
        
        static QTSServerInterface*  GetServer()         { return sServer; }
        
        //Allows you to map RTP session IDs (strings) to actual RTP session objects
        OSRefTable*         GetRTPSessionMap()          { return fRTPMap; }		//服务器对象的fRTPMap,key设置为RTSP的客户端会话ID
    
        //Server provides a statically created & bound UDPSocket / Demuxer pair
        //for each IP address setup to serve RTP. You access those pairs through
        //this function. This returns a pair pre-bound to the IPAddr specified.
        UDPSocketPool*      GetSocketPool()             { return fSocketPool; }

        QTSServerPrefs*     GetPrefs()                  { return fSrvrPrefs; }
        QTSSMessages*       GetMessages()               { return fSrvrMessages; }
        
        //
        //
        // SERVER NAME & VERSION
        
        static StrPtrLen&   GetServerName()             { return sServerNameStr; }
        static StrPtrLen&   GetServerVersion()          { return sServerVersionStr; }
        static StrPtrLen&   GetServerPlatform()         { return sServerPlatformStr; }
        static StrPtrLen&   GetServerBuildDate()        { return sServerBuildDateStr; }
        static StrPtrLen&   GetServerHeader()           { return sServerHeaderPtr; }
        static StrPtrLen&   GetServerBuild()            { return sServerBuildStr; }
        static StrPtrLen&   GetServerComment()          { return sServerCommentStr; }
        
        //
        // PUBLIC HEADER
        static StrPtrLen*   GetPublicHeader()           { return &sPublicHeaderStr; }
        
        // KILL所有会话
        // KILL ALL
        void                KillAllRTPSessions();
        
        //
        // SIGINT - to interrupt the server, set this flag and the server will shut down
        void                SetSigInt()                 { fSigInt = true; }

       // SIGTERM - to kill the server, set this flag and the server will shut down
        void                SetSigTerm()                 { fSigTerm = true; }
        
        //
        // MODULE STORAGE
        
        // All module objects are stored here, and are accessable through
        // these routines.
        
        // Returns the number of modules that act in a given role
        static UInt32       GetNumModulesInRole(QTSSModule::RoleIndex inRole)
                { Assert(inRole < QTSSModule::kNumRoles); return sNumModulesInRole[inRole]; }
        
        // Allows the caller to iterate over all modules that act in a given role           
        static QTSSModule*  GetModule(QTSSModule::RoleIndex inRole, UInt32 inIndex)
                                {   Assert(inRole < QTSSModule::kNumRoles);
                                    Assert(inIndex < sNumModulesInRole[inRole]);
                                    return sModuleArray[inRole][inIndex];
                                }

        //
        // We need to override this. This is how we implement the QTSS_StateChange_Role
        virtual void    SetValueComplete(UInt32 inAttrIndex, QTSSDictionaryMap* inMap,
									UInt32 inValueIndex, void* inNewValue, UInt32 inNewValueLen);
        
        //
        // ERROR LOGGING
        
        // Invokes the error logging modules with some data
		//
        static void     LogError(QTSS_ErrorVerbosity inVerbosity, char* inBuffer);
        
        // Returns the error log stream
        static QTSSErrorLogStream* GetErrorLogStream() { return &sErrorLogStream; }
        
        //
        // LOCKING DOWN THE SERVER OBJECT
        OSMutex*        GetServerObjectMutex() { return &fMutex; }
        

        
    protected:

        // Setup by the derived RTSPServer object

        //Sockets are allocated global to the server, and arbitrated through this pool here.
        //RTCP data is processed completely within the following task.
        UDPSocketPool*              fSocketPool;	//在QTSServer::Initialize(...)中创建, 指向RTPSocketPool
        
        // All RTP sessions are put into this map
        OSRefTable*                 fRTPMap;		//服务器对象的fRTPMap, key设置为RTSP的客户端会话ID
        
        QTSServerPrefs*             fSrvrPrefs;		////qtssSvrPreferences 代表服务器预置的对象。服务器参数
        QTSSMessages*               fSrvrMessages;	//qtssSvrMessages 包含服务器错误信息的对象

        QTSServerPrefs*				fStubSrvrPrefs;
        QTSSMessages*				fStubSrvrMessages;

        QTSS_ServerState            fServerState;   //qtssSvrState 服务器的当前状态。可能的值有
														//qtssStartingUpState，
														//qtssRunningState，　
														//qtssRefusingConnectionsState，  会导致服务器拒绝新的连接；
														//qtssFatalErrorState，状态表示已经发生致命错误，但是服务器尚未退出。会导致服务器退出。
														//qtssShuttingDownState，则会导致服务器退出。
														//qtssIdleState。模块可以设置服务器的状态。如果模块进行设置，则服务器会进行相应的响应

        UInt32                      fDefaultIPAddr;		//qtssSvrDefaultIPAddr The "缺省的"服务器IP地址。
        
        // Array of pointers to TCPListenerSockets.
        TCPListenerSocket**         fListeners;		//RTSP侦听数据, 在CreateListeners(...)函数中建立, 包括四个RTSP侦听端口, 554, 7070, 8000, 以及8001。
        UInt32                      fNumListeners;	//RTSP侦听数据的长度 Number of elements in the array
        
        // startup time
        SInt64              fStartupTime_UnixMilli;		//qtssSvrStartupTime 服务器启动的时间。
        SInt32              fGMTOffset;					//qtssSvrGMTOffsetInHrs 服务器运行所在的时区（与GMT的差别，按小时计）。

        static ResizeableStringFormatter    sPublicHeaderFormatter;
        static StrPtrLen                    sPublicHeaderStr;

        //
        //  MODULE DATA
        //  ...是否定义了多个模块，每个模块对应一个数组
        static QTSSModule**             sModuleArray[QTSSModule::kNumRoles];			//模块数组??初始化以后是否把模块放在该数组中
        static UInt32                   sNumModulesInRole[QTSSModule::kNumRoles];		//角色数据??
        static OSQueue                  sModuleQueue;									//已增加的模块队列
        static QTSSErrorLogStream       sErrorLogStream;								//Error Log Stream

    private:
    
        enum
        {
            kMaxServerHeaderLen = 1000
        };

        static void* TimeConnected(QTSSDictionary* inConnection, UInt32* outLen);

        static UInt32       sServerAPIVersion;			//qtssServerAPIVersion 当前服务器支持的API版本。这个值的格式为0xMMMMmmmm，其中M是大版本号，m是小版本号。
        static StrPtrLen    sServerNameStr;				//qtssSvrServerName 服务器名称。
        static StrPtrLen    sServerVersionStr;			//qtssSvrServerVersion 服务器版本。
        static StrPtrLen    sServerBuildStr;			//build of the server
        static StrPtrLen    sServerCommentStr;			//RTSP comment for the server header    
        static StrPtrLen    sServerPlatformStr;			//Platform (OS) of the server
        static StrPtrLen    sServerBuildDateStr;		//qtssSvrServerBuildDate 服务器连编的日期。
        static char         sServerHeader[kMaxServerHeaderLen];
        static StrPtrLen    sServerHeaderPtr;			//qtssSvrRTSPServerHeader 服务器响应RTSP客户时使用的报头。

        OSMutex             fMutex;

        UInt32              fNumRTSPSessions;			//qtssRTSPCurrentSessionCount 当前基于标准的RTSP与服务器连接的客户数
        UInt32              fNumRTSPHTTPSessions;		//qtssRTSPHTTPCurrentSessionCount 当前基于标准的RTSP/HTTP与服务器连接的客户数。
        UInt32              fNumRTPSessions;			//qtssRTPSvrCurConn 当前与服务器连接的客户数。

        //stores the current number of playing connections.
        UInt32              fNumRTPPlayingSessions;

        //stores the total number of connections since startup.
        UInt32              fTotalRTPSessions;			//qtssRTPSvrTotalConn 自服务器启动以来一直连接着的客户总数。
        //stores the total number of bytes served since startup
        UInt64              fTotalRTPBytes;				//qtssRTPSvrTotalBytes服务器启动以后发出的字节总数
        //total number of rtp packets sent since startup
        UInt64              fTotalRTPPackets;			//qtssRTPSvrTotalPackets 自服务器启动以来输出的总数据包数。
        //stores the total number of bytes lost (as reported by clients) since startup
        UInt64              fTotalRTPPacketsLost;

        //because there is no 64 bit atomic add (for obvious reasons), we efficiently
        //implement total byte counting by atomic adding to this variable, then every
        //once in awhile updating the sTotalBytes.
        unsigned int        fPeriodicRTPBytes;				//周期性发生的字节
        unsigned int        fPeriodicRTPPacketsLost;
        unsigned int        fPeriodicRTPPackets;
        
        //stores the current served bandwidth in BITS per second
        UInt32              fCurrentRTPBandwidthInBits;		//qtssRTPSvrCurBandwidth 当前服务器输出的带宽，以每秒的位数计。
        UInt32              fAvgRTPBandwidthInBits;			//qtssRTPSvrAvgBandwidth 服务器输出的平均带宽，以每秒的位数计。
        UInt32              fRTPPacketsPerSecond;			//qtssRTPSvrCurPackets 当前服务器每秒输出的数据包数。
        
        Float32             fCPUPercent;					//qtssSvrCPULoadPercent 服务器当前使用的CPU时间的百分比。
        Float32             fCPUTimeUsedInSec;              
        
        // stores # of UDP sockets in the server currently (gets updated lazily via.
        // param retrieval function)
        UInt32              fTotalUDPSockets;
        
        // are we out of descriptors?
        Bool16              fIsOutOfDescriptors;
        
        // Storage for current time attribute
        SInt64              fCurrentTime_UnixMilli;

        // Stats for UDP retransmits
        UInt32              fUDPWastageInBytes;
        UInt32              fNumUDPBuffers;
        
        // MP3 Client Session params
        UInt32              fNumMP3Sessions;				//qtssMP3SvrCurConn 当前连接着的MP3客户会话数
        UInt32              fTotalMP3Sessions;				//qtssMP3TotalConn 自服务器启动以来MP3客户会话的总数。
        UInt32              fCurrentMP3BandwidthInBits;		//qtssMP3SvrCurBandwidth MP3带宽，以服务器当前正在发送的每秒位数计
        UInt64              fTotalMP3Bytes;					//qtssMP3SvrTotalBytes 自服务器启动以来发送的MP3字节总数。
        UInt32              fAvgMP3BandwidthInBits;			//qtssMP3SvrAvgBandwidth 平均MP3带宽，以服务器当前正在发送的每秒位数计。
        
        Bool16              fSigInt;
        Bool16              fSigTerm;

        UInt32              fDebugLevel;				//函数RunServer()运行时会按fDebugLevel，来确定是否输出信息到文件
        UInt32              fDebugOptions;              //void RunServer()，运行时会按此值，来确定是否输出信息到文件，其取值为        kRunServerDebug_Off = 0,  kRunServerDebugDisplay_On = 1 << 0,   kRunServerDebugLogging_On = 1 << 1 // not implemented
        

        SInt64          fMaxLate;
        SInt64          fTotalLate;
        SInt64          fCurrentMaxLate;
        SInt64          fTotalQuality;
        SInt32          fNumThinned;					//Number of thinned sessions

        // Param retrieval functions
        static void* CurrentUnixTimeMilli(QTSSDictionary* inServer, UInt32* outLen);
        static void* GetTotalUDPSockets(QTSSDictionary* inServer, UInt32* outLen);
        static void* IsOutOfDescriptors(QTSSDictionary* inServer, UInt32* outLen);
        static void* GetNumUDPBuffers(QTSSDictionary* inServer, UInt32* outLen);
        static void* GetNumWastedBytes(QTSSDictionary* inServer, UInt32* outLen);
        
        static QTSServerInterface*  sServer;				//指向QTSServerInterface的指针，构造函数里该指针指向本对象
        static QTSSAttrInfoDict::AttrInfo   sAttributes[];	//
        static QTSSAttrInfoDict::AttrInfo   sConnectedUserAttributes[];
        
        friend class RTPStatsUpdaterTask;
        friend class SessionTimeoutTask;
};


class RTPStatsUpdaterTask : public Task
{
    public:
		// 该类周期性地运行，以计算当前的总值以及平均值
        // This class runs periodically to compute current totals & averages
        
		//构造函数: 设置任务名称，并触发该任务的Task::kStartEvent事件
		RTPStatsUpdaterTask();
        virtual ~RTPStatsUpdaterTask() {}
    
    private:
    
        virtual SInt64 Run();
        //函数从inRTPSessionMap读出每个会话，取出创建时间最晚的会话(GetSessionCreateTime())，并返回该会话
		RTPSessionInterface* GetNewestSession(OSRefTable* inRTPSessionMap);
        // 该函数返回总秒数，用于统计处理机作为用户线程运行RTPStatsUpdaterTask()
		Float32 GetCPUTimeInSeconds();
        
        SInt64 fLastBandwidthTime;
        SInt64 fLastBandwidthAvg;
        SInt64 fLastBytesSent;
        SInt64 fLastTotalMP3Bytes;
};



#endif // __QTSSERVERINTERFACE_H__

