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

    //ɾ���ûỰ������RTP��
    for (int x = 0; this->GetValuePtr(qtssCliSesStreamObjects, x, (void**)&theStream, &theLen) == QTSS_NoErr; x++)
    {//qtssCliSesStreamObjects�������ԸûỰ����RTP������(QTSS_RTPStreamObject)�ĵ�������
        Assert(theStream != NULL);
        Assert(theLen == sizeof(RTPStream*));
        
        if (*theStream != NULL)
            delete *theStream;
    }
    //
	//�ӷ�����ɾ��RTPSession����
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
            QTSS_Error theErr = theServer->GetValuePtr(qtssSvrClientSessions, y, (void**)&theSession, &theLen, true);	//qtssSvrClientSessions һ�����󣬰����洢����������QTSS_ClientSessionObject�����е����пͻ��Ự��
            Assert(theErr == QTSS_NoErr);
            
            if (*theSession == this)
            {
                theErr = theServer->RemoveValue(qtssSvrClientSessions, y, QTSSDictionary::kDontObeyReadOnly);		//qtssSvrClientSessions һ�����󣬰����洢����������QTSS_ClientSessionObject�����е����пͻ��Ự��
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

//�ú����ѸûỰ����Session map(�ɷ�����,��������ģ�����). ����ú���ʧ��, ����map���Ѿ��иûỰID�ĻỰ
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
	//����ûỰ,����RTPSession Map
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
    Assert(theServer->GetNumValues(qtssSvrClientSessions) == theServer->GetNumRTPSessions());		//qtssSvrClientSessions һ�����󣬰����洢����������QTSS_ClientSessionObject�����е����пͻ��Ự��
#endif
    RTPSession* theSession = this;
	//qtssSvrClientSessions �������пͻ��˻Ự��QTSS_Object�Ի����ͻ��˻Ự�������˵�QTSS_ClientSessionObject����
	//qtssSvrClientSessions һ�����󣬰����洢����������QTSS_ClientSessionObject�����е����пͻ��Ự��
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
    UInt32 theSSRC = 0;	//SSRC��RTPͷЯ����������ʶ����RTP���ݰ��ķ����ߣ��Ӷ����������ײ������ַʵ�����ݰ���ʶ��
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

//��ʾ���ú���������TaskThread������
SInt64 RTPSession::Run()
{ 
#if DEBUG
    Assert(fActivateCalled);
#endif
    EventFlags events = this->GetEvents();	//ȡ���¼�
    QTSS_RoleParams theParams;
    theParams.clientSessionClosingParams.inClientSession = this;    //�ṩ������Module���еĲ�������һ����Ա�Ƕ�����
						//every single role being invoked now has this as the first parameter
    
#if RTPSESSION_DEBUGGING
    qtss_printf("RTPSession %ld: In Run. Events %ld\n",(SInt32)this, (SInt32)events);
#endif
    // ��Щģ������̶߳����в��Ҹ�ģ�� Some callbacks look for this struct in the thread object
    OSThreadDataSetter theSetter(&fModuleState, NULL);

    //if we have been instructed to go away, then let's delete ourselves
	/*����¼���֪ͨRTPSession������������׼����ɱ�����ܵ�������������������¼�����ɱkKillEvent����ʱkTimeoutEvent*/
    /////////////Start RTPSession��ɱ����/////////////////
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
			//ɾ�����̽ϸ���. һ����, ɾ��������Run������ʵ��, ��ȷ����������ɱʱû�н��յ��¼�. ���ǻ���ȷ�����ǲ��ڽ���RTSP����
			//(���߸������˵, �����󲻻ᱻ�����߳�ʹ��). Ϊ����ɸò���, �������ȴӻỰmap��ɾ���Ự.
        
			#if RTPSESSION_DEBUGGING
					qtss_printf("RTPSession %ld: about to be killed. Eventmask = %ld\n",(SInt32)this, (SInt32)events);
			#endif
			// We cannot block waiting to UnRegister, because we have to give the RTSPSessionTask a chance to release the RTPSession.
			//���ǲ��������ȴ�UnRegister, ��Ϊ���Ǳ����RTSPSessionTaskɾ����RTPSession�Ự�Ļ���.
			OSRefTable* sessionTable = QTSServerInterface::GetServer()->GetRTPSessionMap();	//�����������fRTPMap, key����ΪRTSP�Ŀͻ��˻ỰID
			Assert(sessionTable != NULL);
			if (!sessionTable->TryUnRegister(&fRTPMapElem))		
			{
				this->Signal(Task::kKillEvent);// So that we get back to this place in the code
				return kCantGetMutexIdleTime;
			}

			#if RTPSESSION_DEBUGGING
					qtss_printf("RTPSession %ld: is UnRegisted. Eventmask = %ld\n",(SInt32)this, (SInt32)events);
			#endif

				//ֻ�������TryUnRegister(&fRTPMapElem)�Ժ�Ż�ִ�����²���
				// The ClientSessionClosing role is allowed to do async stuff
				fModuleState.curTask = this;
				fModuleDoingAsyncStuff = true;  //�Ա��ڴ����������ʵ�λ��So that we know to jump back to the right place in the code
				fCurrentModule = 0;             // �´�ִ��ʱֱ������50�д����Ժ�, if ((fState == qtssPausedState) || (fModule == NULL))
        
				// Set the reason parameter 
				theParams.clientSessionClosingParams.inReason = fClosingReason;
            
				// If RTCP packets are being generated internally for this stream, 
				// Send a BYE now.
				// ���´��뷢��RTCP SR
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
		//��ʱ����ִ�лỰclearup����������Session�Ѿ�unregistered��û������ģ����Է��ʻỰ����˲���Ҫmutex��

        UInt32 numModules = QTSServerInterface::GetNumModulesInRole(QTSSModule::kClientSessionClosingRole);
        {
            for (; fCurrentModule < numModules; fCurrentModule++)
            {
                fModuleState.eventRequested = false;
                fModuleState.idleTime = 0;
                QTSSModule* theModule = QTSServerInterface::GetModule(QTSSModule::kClientSessionClosingRole, fCurrentModule);
                (void)theModule->CallDispatch(QTSS_ClientSessionClosing_Role, &theParams);		//����clientSessionClosingParams.inClientSession�Լ�, clientSessionClosingParams.inReason = fClosingReason;

                // If this module has requested an event, return and wait for the event to transpire
				// ���ģ���������¼�,��ȴ��¼�����
                if (fModuleState.eventRequested)
                    return fModuleState.idleTime; // If the module has requested idle time...
            }
        }
        
        return -1;//doing this will cause the destructor to get called.
    }//if ((events & Task::kKillEvent) || (events & Task::kTimeoutEvent) || (fModuleDoingAsyncStuff))
	/////////////End RTPSession��ɱ����/////////////////
    
    //if the stream is currently paused, just return without doing anything.
    //We'll get woken up again when a play is issued
	//�����������ͣ��PAUSE��״̬��ʲô�������ͷ��أ��ȴ�PLAY����
    if ((fState == qtssPausedState) || (fModule == NULL))
        return 0;
        
	//������븺��������
    //Make sure to grab the session mutex here, to protect the module against RTSP requests coming in while it's sending packets
    {//��Session��������������ֹ�������ݹ�����RTSP������
        OSMutexLocker locker(&fSessionMutex);

        //just make sure we haven't been scheduled before our scheduled play
        //time. If so, reschedule ourselves for the proper time. (if client
        //sends a play while we are already playing, this may occur)
        //�趨���ݰ�����ʱ�䣬��ֹ����ǰ����
        theParams.rtpSendPacketsParams.inCurrentTime = OS::Milliseconds();
        if (fNextSendPacketsTime > theParams.rtpSendPacketsParams.inCurrentTime)		//δ������ʱ��
        {
            RTPStream** retransStream = NULL;
            UInt32 retransStreamLen = 0;

            //
            // Send retransmits if we need to
            for (int streamIter = 0; this->GetValuePtr(qtssCliSesStreamObjects, streamIter, (void**)&retransStream, &retransStreamLen) == QTSS_NoErr; streamIter++)
                if (retransStream && *retransStream)
                    (*retransStream)->SendRetransmits();
            
            theParams.rtpSendPacketsParams.outNextPacketTime = fNextSendPacketsTime - theParams.rtpSendPacketsParams.inCurrentTime;	//���㻹��೤ʱ��ſ�����
        }
        else
        {//�´�����ʱ���ȱȱʡֵΪ0
			#if RTPSESSION_DEBUGGING
					qtss_printf("RTPSession %ld: about to call SendPackets\n",(SInt32)this);
			#endif
            if ((theParams.rtpSendPacketsParams.inCurrentTime - fLastBandwidthTrackerStatsUpdate) > 1000)
                this->GetBandwidthTracker()->UpdateStats();
                
            theParams.rtpSendPacketsParams.outNextPacketTime = 0;
            // Async event registration is definitely allowed from this role.             // ����Module״̬
            fModuleState.eventRequested = false;
            Assert(fModule != NULL);
            //����QTSS_RTPSendPackets_Role�ڵĺ����������ݣ���FileModule.cpp
            (void)fModule->CallDispatch(QTSS_RTPSendPackets_Role, &theParams);
			#if RTPSESSION_DEBUGGING
					qtss_printf("RTPSession %ld: back from sendPackets, nextPacketTime = %" _64BITARG_ "d\n",(SInt32)this, theParams.rtpSendPacketsParams.outNextPacketTime);
			#endif
            //make sure not to get deleted accidently!
            //������ֵ�Ӹ�����Ϊ0�������������ͻᱻTaskThreadɾ��
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
    //������һ��ϣ�������е�ʱ�䣻����ֵ�����ǰ�ĵķ���
    return theParams.rtpSendPacketsParams.outNextPacketTime;
}
//����������ɼ�������״̬��Run�����ķ���ֵ�����֣��������ֵΪ������������һ�η������ݰ���ʱ�䣬�涨ʱ�䵽����ʱ��
//TaskThread�̻߳��Զ�����Run�������������ֵ����0�����´��κ��¼�����ʱ��Run�����ͻᱻ���ã����������������������
//���ݶ��Ѿ�������ɻ��߸�RTPSession����Ҫ��ɱ����ʱ��
//Run����������QTSSFileModule�е�QTSS_RTPSendPackets_Role�������ݡ���QTSSFileModule.cpp�ļ���QTSSFileModule_Main�����ڣ�
//ϵͳ�ֵ�����SendPackets�������������������RTP���ݰ��ĺ��������Ƕ�������������