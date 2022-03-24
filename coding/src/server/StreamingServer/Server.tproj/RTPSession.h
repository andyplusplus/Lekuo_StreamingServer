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
    File:       RTPSession.h

    Contains:   RTPSession represents an, well, an RTP session. The server creates
                one of these when a new client connects, and it lives for the duration
                of an RTP presentation. It contains all the resources associated with
                that presentation, such as RTPStream objects. It also provides an
                API for manipulating a session (playing it, pausing it, stopping it, etc) 
                
                It is also the active element, ie. it is the object that gets scheduled
                to send out & receive RTP & RTCP packets
RTPSession代表一个RTP会话, 服务器在客户连接时建立一个Session, 并在整个RTP表示期间存在. RTPSession包含与表示相关的所有资源, 如RTPStream对象, RTPStream还提供了操作的API(播放, 停止, 暂停等)

RTP标准是实时传输协议（Real-Time Transfer Protocol）的简称，它被客户和流式媒体服务器用来处理流式媒体数据的传输。
在介绍RTSP的运行流程时，我们发现RTSPSession对象通过调用SetupRequest函数为客户建立RTPSession对象。RTPSession类是
Task类的子类，因此它重载了Task类的Run函数，该函数通过调用FileModule.cpp文件中的SendPacket()函数向客户发送RTP协议
打包的流式媒体数据。当客户通过利用RTSP向RTSPSession对象发出PLAY命令后，RTSPSession对象将调用RTPSession对象的Play()函数。
Play函数准备好需要打包发送的数据后，利用Task类的Signal函数传信RTPSession对象，使其被加入某个TaskThread的任务队列，
从而运行其Run函数。

另外，对同一个节目中的每一个独立的RTP流（如音频流或视频流等），DSS都定义了一个RTPStream类与之对应；显然一个RTPSession
  对象可能包含多个RTPStream对象。  

    Change History (most recent first):
*/


#ifndef _RTPSESSION_H_
#define _RTPSESSION_H_

#include "RTPSessionInterface.h"
#include "RTSPRequestInterface.h"
#include "RTPStream.h"
#include "QTSSModule.h"


class RTPSession : public RTPSessionInterface
{
    public:
    
        RTPSession();				//构造函数:设置fModuleState数据结构
        virtual ~RTPSession();		//析构函数:删除该Session的所有RTP流,从服务器删除RTPSession对象
        
        //
        //ACCESS FUNCTIONS
        
        QTSSModule* GetPacketSendingModule()                        { return fModule; }
        Bool16      HasAnRTPStream()                                { return fHasAnRTPStream; }
        
        RTPStream*  FindRTPStreamForChannelNum(UInt8 inChannelNum);		//找到特定通道号的RTPStream
        
        //
        // MODIFIERS
        
        //This puts this session into the session map (called by the server! not the module!)
        //If this function fails (it can return QTSS_DupName), it means that there is already
        //a session with this session ID in the map.
		//该函数把该会话放入Session map(由服务器,而不是由模块调用). 如果该函数失败, 表明map中已经有该会话ID的会话
        QTSS_Error  Activate(const StrPtrLen& inSessionID);
                
        // The way this object is implemented currently, only one module can send the
        // packets for a session.
        void        SetPacketSendingModule(QTSSModule* inModule)    { fModule = inModule; }

        //Once the session is bound, a module can add streams to it.
        //It must pass in a trackID that uniquely identifies this stream.
        //This call can only be made during an RTSP Setup request, and the
        //RTSPRequestInterface must be provided.
        //You may also opt to attach a codec name and type to this stream.
		//一旦会话已被绑定, 模块可以增加流到该会话.模块必须传递trackID(唯一标识该流).
		//该调用只在RTSP Setup请求期间建立, 并且必须提供RTSPRequestInterface.
		//你也可选择性地在该流上附加codec name以及codec类型
        QTSS_Error  AddStream(  RTSPRequestInterface* request, RTPStream** outStream,
                                    QTSS_AddStreamFlags inFlags);
        
		//Reset the thinning params for all streams using the late tolerance value
		void SetStreamThinningParams(Float32 inLateTolerance);
		
        //Begins playing all streams. Currently must be associated with an RTSP Play
        //request, and the request interface must be provided.
		//开始播放所有流,当前必须与RTSP Play request相关联,并且必须提供request interface
		//函数实现包括两个方面: 1. 设置Buffer, 2. 传信this->Signal(Task::kStartEvent)
        QTSS_Error  Play(RTSPRequestInterface* request, QTSS_PlayFlags inFlags);
        
        //Pauses all streams.
        void            Pause() { fState = qtssPausedState; }
        
        // Tears down the session. This will cause QTSS_SessionClosing_Role to run
		// fRTSPSession->DecrementObjectHolderCount(); Signal(Task::kKillEvent)
        void            Teardown();

        //Utility functions. Modules aren't required to use these, but can be useful
        void            SendDescribeResponse(RTSPRequestInterface* request);
        void            SendAnnounceResponse(RTSPRequestInterface* request);
        void            SendPlayResponse(RTSPRequestInterface* request, UInt32 inFlags);
        void            SendPauseResponse(RTSPRequestInterface* request)
                                                                { request->SendHeader(); }
        void            SendTeardownResponse(RTSPRequestInterface* request)
                                    { request->SetKeepAlive(false); request->SendHeader(); }
                                    
        SInt32          GetQualityLevel();
        void            SetQualityLevel(SInt32 level);

    private:
    
        //where timeouts, deletion conditions get processed
        virtual SInt64  Run();
        
        // Utility function used by Play
        UInt32 PowerOf2Floor(UInt32 inNumToFloor);
    
		//overbuffer logging function
		void LogOverbufferStats();
	
		enum
		{
            kRTPStreamArraySize     = 20,
            kCantGetMutexIdleTime   = 10			//用于Mutex的切换时间
        };

        QTSSModule*         fModule;				//该Session的发包模块
        Bool16              fHasAnRTPStream;		//是否有RTP流
        SInt32              fSessionQualityLevel;	//
        
        char        fRTPStreamArray[kRTPStreamArraySize];	//该变量没有被使用
        
        // Module invocation and module state.
        // This info keeps track of our current state so that
        // the state machine works properly.
        enum
        {
            kStart                  = 0,
            kSendingPackets         = 1
        };
        
        UInt32 fCurrentModuleIndex;		//没有使用
        UInt32 fCurrentState;			//没有使用
        
        QTSS_ModuleState    fModuleState;			//模块状态, 包括
        QTSS_CliSesClosingReason fClosingReason;	//退出系统的原因, 如超时或killself
        
        UInt32              fCurrentModule;			//没有系统使用

        // This is here to give the ability to the ClientSessionClosing role to do asynchronous I/O
        Bool16              fModuleDoingAsyncStuff;
        
#if DEBUG
        Bool16 fActivateCalled;
#endif
        SInt64              fLastBandwidthTrackerStatsUpdate;

};

#endif //_RTPSESSION_H_
