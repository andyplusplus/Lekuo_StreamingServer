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
    File:       RTPSession.cpp

    Contains:   Implementation of RTPSession class. 
                    
    Change History (most recent first):
*/

#include "RTPSession.h"

#include "RTSPProtocol.h" 
#include "QTSServerInterface.h"
#include "QTSS.h"

#include "OS.h"
#include "OSMemory.h"

#include <errno.h>

#define RTPSESSION_DEBUGGING 0

RTPSession::RTPSession() :
    RTPSessionInterface(),
    fModule(NULL),
    fHasAnRTPStream(false),
    fCurrentModuleIndex(0),
    fCurrentState(kStart),
    fClosingReason(qtssCliSesCloseClientTeardown),
    fCurrentModule(0),
    fModuleDoingAsyncStuff(false),
    fLastBandwidthTrackerStatsUpdate(0)
{
#if DEBUG
    fActivateCalled = false;
#endif

    this->SetTaskName("RTPSession");
    fModuleState.curModule = NULL;
    fModuleState.curTask = this;
    fModuleState.curRole = 0;
}

RTPSession::~RTPSession()
{
    // Delete all the streams
    RTPStream** theStream = NULL;
    UInt32 theLen = 0;
    
    if (QTSServerInterface::GetServer()->GetPrefs()->GetReliableUDPPrintfsEnabled())
    {
        SInt32 theNumLatePacketsDropped = 0;
        SInt32 theNumResends = 0;
        
        for (int x = 0; this->GetValuePtr(qtssCliSesStreamObjects, x, (void**)&theStream, &theLen) == QTSS_NoErr; x++)
        {
            Assert(theStream != NULL);
            Assert(theLen == sizeof(RTPStream*));
            if (*theStream != NULL)
            {
                theNumLatePacketsDropped += (*theStream)->GetStalePacketsDropped();
                theNumResends += (*theStream)->GetResender()->GetNumResends();
            }
        }
        
        char* theURL = NULL;
        (void)this->GetValueAsString(qtssCliSesFullURL, 0, &theURL);
        Assert(theURL != NULL);
        
        RTPBandwidthTracker* tracker = this->GetBandwidthTracker(); 
    
        qtss_printf("Client complete. URL: %s.\n",theURL);
        qtss_printf("Max congestion window: %ld. Min congestion window: %ld. Avg congestion window: %ld\n", tracker->GetMaxCongestionWindowSize(), tracker->GetMinCongestionWindowSize(), tracker->GetAvgCongestionWindowSize());
        qtss_printf("Max RTT: %ld. Min RTT: %ld. Avg RTT: %ld\n", tracker->GetMaxRTO(), tracker->GetMinRTO(), tracker->GetAvgRTO());
        qtss_printf("Num resends: %ld. Num skipped frames: %ld. Num late packets dropped: %ld\n", theNumResends, this->GetFramesSkipped(), theNumLatePacketsDropped);
        
        delete [] theURL;
    }

    //删除该会话的所有RTP流
    for (int x = 0; this->GetValuePtr(qtssCliSesStreamObjects, x, (void**)&theStream, &theLen) == QTSS_NoErr; x++)
    {//qtssCliSesStreamObjects包含属性该会话所有RTP流引用(QTSS_RTPStreamObject)的迭代属性
        Assert(theStream != NULL);
        Assert(theLen == sizeof(RTPStream*));
        
        if (*theStream != NULL)
            delete *theStream;
    }
    //
	//从服务器删除RTPSession对象
	//
    QTSServerInterface* theServer = QTSServerInterface::GetServer();
    
    {
        OSMutexLocker theLocker(theServer->GetMutex());
        
        RTPSession** theSession = NULL;
        //
        // Remove this session from the qtssSvrClientSessions attribute
        UInt32 y = 0;
        for ( ; y < theServer->GetNumRTPSessions(); y++)
        {
            QTSS_Error theErr = theServer->GetValuePtr(qtssSvrClientSessions, y, (void**)&theSession, &theLen, true);	//qtssSvrClientSessions 一个对象，包含存储在索引过的QTSS_ClientSessionObject对象中的所有客户会话。
            Assert(theErr == QTSS_NoErr);
            
            if (*theSession == this)
            {
                theErr = theServer->RemoveValue(qtssSvrClientSessions, y, QTSSDictionary::kDontObeyReadOnly);		//qtssSvrClientSessions 一个对象，包含存储在索引过的QTSS_ClientSessionObject对象中的所有客户会话。
                break;
            }
        }

        Assert(y < theServer->GetNumRTPSessions());
        theServer->AlterCurrentRTPSessionCount(-1);
        if (!fIsFirstPlay) // The session was started playing (the counter ignores additional pause-play changes while session is active)
            theServer->AlterRTPPlayingSessions(-1);
        
    }

    //we better not be in the RTPSessionMap anymore!
#if DEBUG
    Assert(!fRTPMapElem.IsInTable());
    OSRef* theRef = QTSServerInterface::GetServer()->GetRTPSessionMap()->Resolve(&fRTSPSessionID);
    Assert(theRef == NULL);
#endif
}

//该函数把该会话放入Session map(由服务器,而不是由模块调用). 如果该函数失败, 表明map中已经有该会话ID的会话
QTSS_Error  RTPSession::Activate(const StrPtrLen& inSessionID)
{
    //Set the session ID for this session
    Assert(inSessionID.Len <= QTSS_MAX_SESSION_ID_LENGTH);
    ::memcpy(fRTSPSessionIDBuf, inSessionID.Ptr, inSessionID.Len);
    fRTSPSessionIDBuf[inSessionID.Len] = '\0';
    this->SetVal(qtssCliSesRTSPSessionID, &fRTSPSessionIDBuf[0], inSessionID.Len);
    
    fRTPMapElem.Set(*this->GetValue(qtssCliSesRTSPSessionID), this);
    
    QTSServerInterface* theServer = QTSServerInterface::GetServer();
    
    //Activate puts the session into the RTPSession Map
	//激活该会话,放入RTPSession Map
    QTSS_Error err = theServer->GetRTPSessionMap()->Register(&fRTPMapElem);
    if (err == EPERM)
        return err;
    Assert(err == QTSS_NoErr);
    
    //
    // Adding this session into the qtssSvrClientSessions attr and incrementing the number of sessions must be atomic
    OSMutexLocker locker(theServer->GetMutex()); 

    //
    // Put this session into the qtssSvrClientSessions attribute of the server
#if DEBUG
    Assert(theServer->GetNumValues(qtssSvrClientSessions) == theServer->GetNumRTPSessions());		//qtssSvrClientSessions 一个对象，包含存储在索引过的QTSS_ClientSessionObject对象中的所有客户会话。
#endif
    RTPSession* theSession = this;
	//qtssSvrClientSessions 包含所有客户端会话的QTSS_Object对话，客户端会话是索引了的QTSS_ClientSessionObject对象
	//qtssSvrClientSessions 一个对象，包含存储在索引过的QTSS_ClientSessionObject对象中的所有客户会话。
    err = theServer->SetValue(qtssSvrClientSessions, theServer->GetNumRTPSessions(), &theSession, sizeof(theSession), QTSSDictionary::kDontObeyReadOnly);
    Assert(err == QTSS_NoErr);
    
#if DEBUG
    fActivateCalled = true;
#endif
    QTSServerInterface::GetServer()->IncrementTotalRTPSessions();
    return QTSS_NoErr;
}

RTPStream*  RTPSession::FindRTPStreamForChannelNum(UInt8 inChannelNum)
{
    RTPStream** theStream = NULL;
    UInt32 theLen = 0;

    for (int x = 0; this->GetValuePtr(qtssCliSesStreamObjects, x, (void**)&theStream, &theLen) == QTSS_NoErr; x++)
    {
        Assert(theStream != NULL);
        Assert(theLen == sizeof(RTPStream*));
        
        if (*theStream != NULL)
            if (((*theStream)->GetRTPChannelNum() == inChannelNum) || ((*theStream)->GetRTCPChannelNum() == inChannelNum))
                return *theStream;
    }
    return NULL; // Couldn't find a matching stream
}

QTSS_Error RTPSession::AddStream(RTSPRequestInterface* request, RTPStream** outStream,
                                        QTSS_AddStreamFlags inFlags)
{
    Assert(outStream != NULL);

    // Create a new SSRC for this stream. This should just be a random number unique to all the streams in the session
    UInt32 theSSRC = 0;	//SSRC由RTP头携带，用来标识发出RTP数据包的发起者，从而不需依赖底层网络地址实现数据包的识别。
    while (theSSRC == 0)
    {
        theSSRC = (SInt32)::rand();

        RTPStream** theStream = NULL;
        UInt32 theLen = 0;
    
        for (int x = 0; this->GetValuePtr(qtssCliSesStreamObjects, x, (void**)&theStream, &theLen) == QTSS_NoErr; x++)
        {
            Assert(theStream != NULL);
            Assert(theLen == sizeof(RTPStream*));
            
            if (*theStream != NULL)
                if ((*theStream)->GetSSRC() == theSSRC)
                    theSSRC = 0;
        }
    }

    *outStream = NEW RTPStream(theSSRC, this);
    
    QTSS_Error theErr = (*outStream)->Setup(request, inFlags);
    if (theErr != QTSS_NoErr)
        // If we couldn't setup the stream, make sure not to leak memory!
        delete *outStream;
    else
    {
        // If the stream init succeeded, then put it into the array of setup streams
        theErr = this->SetValue(qtssCliSesStreamObjects, this->GetNumValues(qtssCliSesStreamObjects),
                                                    outStream, sizeof(RTPStream*), QTSSDictionary::kDontObeyReadOnly);
        Assert(theErr == QTSS_NoErr);
        fHasAnRTPStream = true;
    }
    return theErr;
}

void RTPSession::SetStreamThinningParams(Float32 inLateTolerance)
{
	// Set the thinning params in all the RTPStreams of the RTPSession
	// Go through all the streams, setting their thinning params
	RTPStream** theStream = NULL;
	UInt32 theLen = 0;
	
	for (int x = 0; this->GetValuePtr(qtssCliSesStreamObjects, x, (void**)&theStream, &theLen) == QTSS_NoErr; x++)
	{
		Assert(theStream != NULL);
		Assert(theLen == sizeof(RTPStream*));
		if (*theStream != NULL)
		{
			(*theStream)->SetLateTolerance(inLateTolerance);
			(*theStream)->SetThinningParams();
		}
	}
}

QTSS_Error  RTPSession::Play(RTSPRequestInterface* request, QTSS_PlayFlags inFlags)
{
    //first setup the play associated session interface variables
    Assert(request != NULL);
    if (fModule == NULL)
        return QTSS_RequestFailed;//Can't play if there are no associated streams
    
    //what time is this play being issued at?
	fLastBitRateUpdateTime = fNextSendPacketsTime = fPlayTime = OS::Milliseconds();
    if (fIsFirstPlay)
		fFirstPlayTime = fPlayTime;
    fAdjustedPlayTime = fPlayTime - ((SInt64)(request->GetStartTime() * 1000));

    //for RTCP SRs, we also need to store the play time in NTP
    fNTPPlayTime = OS::TimeMilli_To_1900Fixed64Secs(fPlayTime);
    
    //we are definitely playing now, so schedule the object!
    fState = qtssPlayingState;
    fIsFirstPlay = false;
    fPlayFlags = inFlags;
    QTSServerInterface::GetServer()-> AlterRTPPlayingSessions(1);
    
    UInt32 theWindowSize;
    UInt32 bitRate = this->GetMovieAvgBitrate();
	if ((bitRate == 0) || (bitRate > QTSServerInterface::GetServer()->GetPrefs()->GetWindowSizeMaxThreshold() * 1024))
        theWindowSize = 1024 * QTSServerInterface::GetServer()->GetPrefs()->GetLargeWindowSizeInK();
	else if (bitRate > QTSServerInterface::GetServer()->GetPrefs()->GetWindowSizeThreshold() * 1024)
		theWindowSize = 1024 * QTSServerInterface::GetServer()->GetPrefs()->GetMediumWindowSizeInK();		
    else
        theWindowSize = 1024 * QTSServerInterface::GetServer()->GetPrefs()->GetSmallWindowSizeInK();

//  qtss_printf("bitrate = %d, window size = %d\n", bitRate, theWindowSize);
    this->GetBandwidthTracker()->SetWindowSize(theWindowSize);
	this->GetOverbufferWindow()->ResetOverBufferWindow();

    //
    // Go through all the streams, setting their thinning params
    RTPStream** theStream = NULL;
    UInt32 theLen = 0;
    
    for (int x = 0; this->GetValuePtr(qtssCliSesStreamObjects, x, (void**)&theStream, &theLen) == QTSS_NoErr; x++)
    {
        Assert(theStream != NULL);
        Assert(theLen == sizeof(RTPStream*));
        if (*theStream != NULL)
        {
            (*theStream)->SetThinningParams();
 			(*theStream)->ResetThinningDelayParams();
            //
            // If we are using reliable UDP, then make sure to clear all the packets
            // from the previous play spurt out of the resender
            (*theStream)->GetResender()->ClearOutstandingPackets();
        }
    }

//  qtss_printf("movie bitrate = %d, window size = %d\n", this->GetMovieAvgBitrate(), theWindowSize);
    Assert(this->GetBandwidthTracker()->BytesInList() == 0);
    
    // Set the size of the RTSPSession's send buffer to an appropriate max size
    // based on the bitrate of the movie. This has 2 benefits:
    // 1) Each socket normally defaults to 32 K. A smaller buffer prevents the
    // system from getting buffer starved if lots of clients get flow-controlled
    //
    // 2) We may need to scale up buffer sizes for high-bandwidth movies in order
    // to maximize thruput, and we may need to scale down buffer sizes for low-bandwidth
    // movies to prevent us from buffering lots of data that the client can't use
    
    // If we don't know any better, assume maximum buffer size.

    QTSServerPrefs* thePrefs = QTSServerInterface::GetServer()->GetPrefs();
    UInt32 theBufferSize = thePrefs->GetMaxTCPBufferSizeInBytes();
    
#if RTPSESSION_DEBUGGING
    qtss_printf("RTPSession GetMovieAvgBitrate %li\n",(SInt32)this->GetMovieAvgBitrate() );
#endif

    if (this->GetMovieAvgBitrate() > 0)
    {
        // We have a bit rate... use it.
        Float32 realBufferSize = (Float32)this->GetMovieAvgBitrate() * thePrefs->GetTCPSecondsToBuffer();
        theBufferSize = (UInt32)realBufferSize;
        theBufferSize >>= 3; // Divide by 8 to convert from bits to bytes
        
        // Round down to the next lowest power of 2.
        theBufferSize = this->PowerOf2Floor(theBufferSize);
        
        // This is how much data we should buffer based on the scaling factor... if it is
        // lower than the min, raise to min
        if (theBufferSize < thePrefs->GetMinTCPBufferSizeInBytes())
            theBufferSize = thePrefs->GetMinTCPBufferSizeInBytes();
            
        // Same deal for max buffer size
        if (theBufferSize > thePrefs->GetMaxTCPBufferSizeInBytes())
            theBufferSize = thePrefs->GetMaxTCPBufferSizeInBytes();
    }
    
    Assert(fRTSPSession != NULL); // can this ever happen?
    if (fRTSPSession != NULL)
        fRTSPSession->GetSocket()->SetSocketBufSize(theBufferSize);
        
    
#if RTPSESSION_DEBUGGING
    qtss_printf("RTPSession %ld: In Play, about to call Signal\n",(SInt32)this);
#endif
    this->Signal(Task::kStartEvent);
    
    return QTSS_NoErr;
}

UInt32 RTPSession::PowerOf2Floor(UInt32 inNumToFloor)
{
    UInt32 retVal = 0x10000000;
    while (retVal > 0)
    {
        if (retVal & inNumToFloor)
            return retVal;
        else
            retVal >>= 1;
    }
    return retVal;
}

void RTPSession::Teardown()
{
    // To proffer a quick death of the RTSP session, let's disassociate
    // ourselves with it right now.
    
    // Note that this function relies on the session mutex being grabbed, because
    // this fRTSPSession pointer could otherwise be being used simultaneously by
    // an RTP stream.
    if (fRTSPSession != NULL)
        fRTSPSession->DecrementObjectHolderCount();
    fRTSPSession = NULL;
    fState = qtssPausedState;
    this->Signal(Task::kKillEvent);
}

void RTPSession::SendPlayResponse(RTSPRequestInterface* request, UInt32 inFlags)
{
    QTSS_RTSPHeader theHeader = qtssRTPInfoHeader;
    
    RTPStream** theStream = NULL;
    UInt32 theLen = 0;
    UInt32 valueCount = this->GetNumValues(qtssCliSesStreamObjects);
    Bool16 lastValue = false;
    for (UInt32 x = 0; x < valueCount; x++)
    {
        this->GetValuePtr(qtssCliSesStreamObjects, x, (void**)&theStream, &theLen);
        Assert(theStream != NULL);
        Assert(theLen == sizeof(RTPStream*));
        
        if (*theStream != NULL)
        {   if (x == (valueCount -1))
                lastValue = true;
            (*theStream)->AppendRTPInfo(theHeader, request, inFlags,lastValue);
            theHeader = qtssSameAsLastHeader;
        }
    }
    request->SendHeader();
}

void    RTPSession::SendDescribeResponse(RTSPRequestInterface* inRequest)
{
    if (inRequest->GetStatus() == qtssRedirectNotModified)
    {
        (void)inRequest->SendHeader();
        return;
    }
    
    // write date and expires
    inRequest->AppendDateAndExpires();
    
    //write content type header
    static StrPtrLen sContentType("application/sdp");
    inRequest->AppendHeader(qtssContentTypeHeader, &sContentType);
    
    // write x-Accept-Retransmit header
    static StrPtrLen sRetransmitProtocolName("our-retransmit");
    inRequest->AppendHeader(qtssXAcceptRetransmitHeader, &sRetransmitProtocolName);
	
	// write x-Accept-Dynamic-Rate header
	static StrPtrLen dynamicRateEnabledStr("1");
	inRequest->AppendHeader(qtssXAcceptDynamicRateHeader, &dynamicRateEnabledStr);
    
    //write content base header
    
    inRequest->AppendContentBaseHeader(inRequest->GetValue(qtssRTSPReqAbsoluteURL));
    
    //I believe the only error that can happen is if the client has disconnected.
    //if that's the case, just ignore it, hopefully the calling module will detect
    //this and return control back to the server ASAP 
    (void)inRequest->SendHeader();
}

void    RTPSession::SendAnnounceResponse(RTSPRequestInterface* inRequest)
{
    //
    // Currently, no need to do anything special for an announce response
    (void)inRequest->SendHeader();
}

//提示：该函数代码在TaskThread内运行
SInt64 RTPSession::Run()
{ 
#if DEBUG
    Assert(fActivateCalled);
#endif
    EventFlags events = this->GetEvents();	//取出事件
    QTSS_RoleParams theParams;
    theParams.clientSessionClosingParams.inClientSession = this;    //提供给其他Module运行的参数，第一个成员是对象本身
						//every single role being invoked now has this as the first parameter
    
#if RTPSESSION_DEBUGGING
    qtss_printf("RTPSession %ld: In Run. Events %ld\n",(SInt32)this, (SInt32)events);
#endif
    // 有些模拟会在线程对象中查找该模块 Some callbacks look for this struct in the thread object
    OSThreadDataSetter theSetter(&fModuleState, NULL);

    //if we have been instructed to go away, then let's delete ourselves
	/*如果事件是通知RTPSession对象死亡，就准备自杀。可能导致这种情况的有两种事件：自杀kKillEvent；超时kTimeoutEvent*/
    /////////////Start RTPSession自杀过程/////////////////
	if ((events & Task::kKillEvent) || (events & Task::kTimeoutEvent) || (fModuleDoingAsyncStuff))
    {
        if (!fModuleDoingAsyncStuff)
		{
			if (events & Task::kTimeoutEvent)
			{
				WZD_Delete_RTPStream_From_ConnectionElem();		//_WZD_ADD_ ask the connection Element to detach with the RTPStream
				fClosingReason = qtssCliSesCloseTimeout;
			}
            
			//deletion is a bit complicated. For one thing, it must happen from within 
			//the Run function to ensure that we aren't getting events when we are deleting
			//ourselves. We also need to make sure that we aren't getting RTSP requests
			//(or, more accurately, that the stream object isn't being used by any other
			//threads). We do this by first removing the session from the session map.
			//删除过程较复杂. 一方面, 删除必须在Run函数内实现, 以确保我们在自杀时没有接收到事件. 我们还需确认我们不在接收RTSP请求
			//(或者更具体地说, 流对象不会被其它线程使用). 为了完成该操作, 我们首先从会话map中删除会话.
        
			#if RTPSESSION_DEBUGGING
					qtss_printf("RTPSession %ld: about to be killed. Eventmask = %ld\n",(SInt32)this, (SInt32)events);
			#endif
			// We cannot block waiting to UnRegister, because we have to give the RTSPSessionTask a chance to release the RTPSession.
			//我们不能阻塞等待UnRegister, 因为我们必须给RTSPSessionTask删除该RTPSession会话的机会.
			OSRefTable* sessionTable = QTSServerInterface::GetServer()->GetRTPSessionMap();	//服务器对象的fRTPMap, key设置为RTSP的客户端会话ID
			Assert(sessionTable != NULL);
			if (!sessionTable->TryUnRegister(&fRTPMapElem))		
			{
				this->Signal(Task::kKillEvent);// So that we get back to this place in the code
				return kCantGetMutexIdleTime;
			}

			#if RTPSESSION_DEBUGGING
					qtss_printf("RTPSession %ld: is UnRegisted. Eventmask = %ld\n",(SInt32)this, (SInt32)events);
			#endif

				//只有在完成TryUnRegister(&fRTPMapElem)以后才会执行以下操作
				// The ClientSessionClosing role is allowed to do async stuff
				fModuleState.curTask = this;
				fModuleDoingAsyncStuff = true;  //以便于代码跳到合适的位置So that we know to jump back to the right place in the code
				fCurrentModule = 0;             // 下次执行时直接跳到50行代码以后, if ((fState == qtssPausedState) || (fModule == NULL))
        
				// Set the reason parameter 
				theParams.clientSessionClosingParams.inReason = fClosingReason;
            
				// If RTCP packets are being generated internally for this stream, 
				// Send a BYE now.
				// 以下代码发送RTCP SR
				RTPStream** theStream = NULL;
				UInt32 theLen = 0;
            
				if (this->GetPlayFlags() & qtssPlayFlagsSendRTCP)
				{
					SInt64 byePacketTime = OS::Milliseconds();
					for (int x = 0; this->GetValuePtr(qtssCliSesStreamObjects, x, (void**)&theStream, &theLen) == QTSS_NoErr; x++)
						if (theStream && *theStream != NULL)
							(*theStream)->SendRTCPSR(byePacketTime, true);
				}
        }
        
        //at this point, we know no one is using this session, so invoke the session cleanup role. We don't need to grab the session mutex before
        //invoking modules here, because the session is unregistered and therefore there's no way another thread could get involved anyway
		//此时可以执行会话clearup工作，由于Session已经unregistered，没有其它模块可以访问会话，因此不需要mutex。

        UInt32 numModules = QTSServerInterface::GetNumModulesInRole(QTSSModule::kClientSessionClosingRole);
        {
            for (; fCurrentModule < numModules; fCurrentModule++)
            {
                fModuleState.eventRequested = false;
                fModuleState.idleTime = 0;
                QTSSModule* theModule = QTSServerInterface::GetModule(QTSSModule::kClientSessionClosingRole, fCurrentModule);
                (void)theModule->CallDispatch(QTSS_ClientSessionClosing_Role, &theParams);		//包含clientSessionClosingParams.inClientSession以及, clientSessionClosingParams.inReason = fClosingReason;

                // If this module has requested an event, return and wait for the event to transpire
				// 如果模块请求了事件,则等待事件过期
                if (fModuleState.eventRequested)
                    return fModuleState.idleTime; // If the module has requested idle time...
            }
        }
        
        return -1;//doing this will cause the destructor to get called.
    }//if ((events & Task::kKillEvent) || (events & Task::kTimeoutEvent) || (fModuleDoingAsyncStuff))
	/////////////End RTPSession自杀过程/////////////////
    
    //if the stream is currently paused, just return without doing anything.
    //We'll get woken up again when a play is issued
	//如果正处于暂停（PAUSE）状态，什么都不做就返回，等待PLAY命令
    if ((fState == qtssPausedState) || (fModule == NULL))
        return 0;
        
	//下面代码负责发送数据
    //Make sure to grab the session mutex here, to protect the module against RTSP requests coming in while it's sending packets
    {//对Session互斥量加锁，防止发送数据过程中RTSP请求到来
        OSMutexLocker locker(&fSessionMutex);

        //just make sure we haven't been scheduled before our scheduled play
        //time. If so, reschedule ourselves for the proper time. (if client
        //sends a play while we are already playing, this may occur)
        //设定数据包发送时间，防止被提前发送
        theParams.rtpSendPacketsParams.inCurrentTime = OS::Milliseconds();
        if (fNextSendPacketsTime > theParams.rtpSendPacketsParams.inCurrentTime)		//未到发送时间
        {
            RTPStream** retransStream = NULL;
            UInt32 retransStreamLen = 0;

            //
            // Send retransmits if we need to
            for (int streamIter = 0; this->GetValuePtr(qtssCliSesStreamObjects, streamIter, (void**)&retransStream, &retransStreamLen) == QTSS_NoErr; streamIter++)
                if (retransStream && *retransStream)
                    (*retransStream)->SendRetransmits();
            
            theParams.rtpSendPacketsParams.outNextPacketTime = fNextSendPacketsTime - theParams.rtpSendPacketsParams.inCurrentTime;	//计算还需多长时间才可运行
        }
        else
        {//下次运行时间的缺缺省值为0
			#if RTPSESSION_DEBUGGING
					qtss_printf("RTPSession %ld: about to call SendPackets\n",(SInt32)this);
			#endif
            if ((theParams.rtpSendPacketsParams.inCurrentTime - fLastBandwidthTrackerStatsUpdate) > 1000)
                this->GetBandwidthTracker()->UpdateStats();
                
            theParams.rtpSendPacketsParams.outNextPacketTime = 0;
            // Async event registration is definitely allowed from this role.             // 设置Module状态
            fModuleState.eventRequested = false;
            Assert(fModule != NULL);
            //调用QTSS_RTPSendPackets_Role内的函数发送数据，见FileModule.cpp
            (void)fModule->CallDispatch(QTSS_RTPSendPackets_Role, &theParams);
			#if RTPSESSION_DEBUGGING
					qtss_printf("RTPSession %ld: back from sendPackets, nextPacketTime = %" _64BITARG_ "d\n",(SInt32)this, theParams.rtpSendPacketsParams.outNextPacketTime);
			#endif
            //make sure not to get deleted accidently!
            //将返回值从负数改为0，否则任务对象就会被TaskThread删除
            if (theParams.rtpSendPacketsParams.outNextPacketTime < 0)
                theParams.rtpSendPacketsParams.outNextPacketTime = 0;
            fNextSendPacketsTime = theParams.rtpSendPacketsParams.inCurrentTime + theParams.rtpSendPacketsParams.outNextPacketTime;
        }
    }
    
    //
    // Make sure the duration between calls to Run() isn't greater than the
    // max retransmit delay interval.
    UInt32 theRetransDelayInMsec = QTSServerInterface::GetServer()->GetPrefs()->GetMaxRetransmitDelayInMsec();
    UInt32 theSendInterval = QTSServerInterface::GetServer()->GetPrefs()->GetSendIntervalInMsec();
    
    //
    // We want to avoid waking up to do retransmits, and then going back to sleep for like, 1 msec. So, 
    // only adjust the time to wake up if the next packet time is greater than the max retransmit delay +
    // the standard interval between wakeups.
    if (theParams.rtpSendPacketsParams.outNextPacketTime > (theRetransDelayInMsec + theSendInterval))
        theParams.rtpSendPacketsParams.outNextPacketTime = theRetransDelayInMsec;
    
    Assert(theParams.rtpSendPacketsParams.outNextPacketTime >= 0);//we'd better not get deleted accidently!
    //返回下一次希望被运行的时间；返回值含义见前文的分析
    return theParams.rtpSendPacketsParams.outNextPacketTime;
}
//从上面分析可见，正常状态下Run函数的返回值有两种：如果返回值为正数，代表下一次发送数据包的时间，规定时间到来的时候，
//TaskThread线程会自动调用Run函数；如果返回值等于0，在下次任何事件发生时，Run函数就会被调用，这种情况往往发生在所有
//数据都已经发送完成或者该RTPSession对象将要被杀死的时候。
//Run函数调用了QTSSFileModule中的QTSS_RTPSendPackets_Role发送数据。在QTSSFileModule.cpp文件的QTSSFileModule_Main函数内，
//系统又调用了SendPackets函数，这才是真正发送RTP数据包的函数，我们对其代码分析如下