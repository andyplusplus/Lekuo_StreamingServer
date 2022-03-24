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
    File:       RTPStream.h

    Contains:   Represents a single client stream (audio, video, etc). Control API is similar to overall session API.
                
                Contains all stream-specific data & resources, used by Session when it
                wants to send out or receive data for this stream
                
                This is also the class that implements the RTP stream dictionary for QTSS API.

	代表单个客户端流, 控制API和其它会话API相同. 
  
	包含所有面向stream的数据和资源, 当会话想发送或接收数据时, 会使用这些数据和资源

	该类也实现了QTSS API的RTP流字典
*/

#ifndef __RTPSTREAM_H__
#define __RTPSTREAM_H__

#include "QTSS.h"
#include "QTSSDictionary.h"
#include "QTSS_Private.h"

#include "UDPDemuxer.h"
#include "UDPSocketPool.h"

#include "RTSPRequestInterface.h"
#include "RTPSessionInterface.h"

#include "RTPPacketResender.h"
#include "QTSServerInterface.h"

class RTPStream : public QTSSDictionary, public UDPDemuxerTask
{
public:
        // Initializes dictionary resources
        static void Initialize();

        //
        // CONSTRUCTOR / DESTRUCTOR
        
        RTPStream(UInt32 inSSRC, RTPSessionInterface* inSession);
        virtual ~RTPStream();
        
        //
        //ACCESS FUNCTIONS
        
        UInt32      GetSSRC()                   { return fSsrc; }
        UInt8       GetRTPChannelNum()          { return fRTPChannel; }
        UInt8       GetRTCPChannelNum()         { return fRTCPChannel; }
        RTPPacketResender* GetResender()        { return &fResender; }
        QTSS_RTPTransportType GetTransportType() { return fTransportType; }
        UInt32      GetStalePacketsDropped()    { return fStalePacketsDropped; }
        UInt32      GetTotalPacketsRecv()       { return fTotalPacketsRecv; }

        // Setup uses the info in the RTSPRequestInterface to associate
        // all the necessary resources, ports, sockets, etc, etc, with this stream.
		// Setup使用RTSPRequestInterface中的信息, 关该流和所有必须要资源, 端口, socket等建立关联
        QTSS_Error Setup(RTSPRequestInterface* request, QTSS_AddStreamFlags inFlags);
        
        // Write sends RTP data to the client. Caller must specify either qtssWriteFlagsIsRTP or qtssWriteFlagsIsRTCP
		// 把RTP数据发送到客户端, 调用该函数必须说明qtssWriteFlagsIsRTP or qtssWriteFlagsIsRTCP形式
        virtual QTSS_Error  Write(void* inBuffer, UInt32 inLen,
                                        UInt32* outLenWritten, QTSS_WriteFlags inFlags);
        
        
        //UTILITY FUNCTIONS:
        //These are not necessary to call and do not manipulate the state of the
        //stream. They may, however, be useful services exported by the server
        
        // 格式化标准的setup回应, 该回应的格式见本类的头文件的结尾. Formats a standard setup response.
        void            SendSetupResponse(RTSPRequestInterface* request);

        //Formats a transport header for this stream. 
        void            AppendTransport(RTSPRequestInterface* request);
        
        //Formats a RTP-Info header for this stream.
        //Isn't useful unless you've already called Play()
        void            AppendRTPInfo(QTSS_RTSPHeader inHeader,
                                        RTSPRequestInterface* request, UInt32 inFlags, Bool16 lastInfo);

        //
        // When we get an incoming Interleaved Packet for this stream, this
        // function should be called
        void ProcessIncomingInterleavedData(UInt8 inChannelNum, RTSPSessionInterface* inRTSPSession, StrPtrLen* inPacket);

        //When we get a new RTCP packet, we can directly invoke the RTP session and tell it
        //to process the packet right now!
        void ProcessIncomingRTCPPacket(StrPtrLen* inPacket);

        // Send a RTCP SR on this stream. Pass in true if this SR should also have a BYE
        void SendRTCPSR(const SInt64& inTime, Bool16 inAppendBye = false);
        
        //
        // Retransmits get sent when there is new data to be sent, but this function
        // should be called periodically even if there is no new packet data, as
        // the pipe should have a steady stream of data in it. 
        void SendRetransmits();

        //
        // Update the thinning parameters for this stream to match current prefs
        void SetThinningParams();
        
		//
		// Reset the delay parameters that are stored for the thinning calculations
		void ResetThinningDelayParams() { fLastCurrentPacketDelay = 0; }
		
		void SetLateTolerance(Float32 inLateToleranceInSec) { fLateToleranceInSec = inLateToleranceInSec; }
		
		void EnableSSRC() { fEnableSSRC = true; }
		void DisableSSRC() { fEnableSSRC = false; }
		
    private:
        
        enum
        {
            kMaxSsrcSizeInBytes         = 12,
            kMaxStreamURLSizeInBytes    = 32,
            kDefaultPayloadBufSize      = 32,
            kSenderReportIntervalInSecs = 7,
            kNumPrebuiltChNums          = 10,
        };
    
        SInt64 fLastQualityChange;
        SInt32 fQualityInterval;

        //either pointers to the statically allocated sockets (maintained by the server)
        //or fresh ones (only fresh in extreme special cases)
        UDPSocketPair*          fSockets;
        RTPSessionInterface*    fSession;	

        // info for kinda reliable UDP
        //DssDurationTimer      fInfoDisplayTimer;
        SInt32                  fBytesSentThisInterval;
        SInt32                  fDisplayCount;
        Bool16                  fSawFirstPacket;
        SInt64                  fStreamCumDuration;
        // manages UDP retransmits
        RTPPacketResender       fResender;
        RTPBandwidthTracker*    fTracker;

        
        //who am i sending to? 客户端的地址, 端口, 以及RTCP端口, 从RTSPRequestInterface获得
        UInt32      fRemoteAddr;	//
        UInt16      fRemoteRTPPort;
        UInt16      fRemoteRTCPPort;
        UInt16      fLocalRTPPort;
        
        //RTCP stuff 
        SInt64      fLastSenderReportTime;
        UInt32      fPacketCount;
        UInt32      fLastPacketCount;
        UInt32      fPacketCountInRTCPInterval;
        UInt32      fByteCount;
        
        // DICTIONARY ATTRIBUTES
        
        //Module assigns a streamID to this object
        UInt32      fTrackID;
        
        //low-level RTP stuff 
        UInt32      fSsrc;
        char        fSsrcString[kMaxSsrcSizeInBytes];
        StrPtrLen   fSsrcStringPtr;
        Bool16      fEnableSSRC;
        
        //Payload name and codec type.
        char                fPayloadNameBuf[kDefaultPayloadBufSize];
        QTSS_RTPPayloadType fPayloadType;

        //Media information.
        UInt16      fFirstSeqNumber;//used in sending the play response
        UInt32      fFirstTimeStamp;//RTP time
        UInt32      fTimescale;
        
        //what is the URL for this stream?
        char        fStreamURL[kMaxStreamURLSizeInBytes];
        StrPtrLen   fStreamURLPtr;						//trackID=3
        
        SInt32      fQualityLevel;
        UInt32      fNumQualityLevels;
			
        UInt32      fLastRTPTimestamp;
        
        // RTCP data, 以下为RTCP数据
        UInt32      fFractionLostPackets;
        UInt32      fTotalLostPackets;
        UInt32      fJitter;
        UInt32      fReceiverBitRate;
        UInt16      fAvgLateMsec;
        UInt16      fPercentPacketsLost;
        UInt16      fAvgBufDelayMsec;
        UInt16      fIsGettingBetter;
        UInt16      fIsGettingWorse;
        UInt32      fNumEyes;
        UInt32      fNumEyesActive;
        UInt32      fNumEyesPaused;
        UInt32      fTotalPacketsRecv;
        UInt32      fPriorTotalPacketsRecv;
        UInt16      fTotalPacketsDropped;
        UInt16      fTotalPacketsLost;
        UInt32      fCurPacketsLostInRTCPInterval;
        UInt16      fClientBufferFill;
        UInt16      fFrameRate;
        UInt16      fExpectedFrameRate;
        UInt16      fAudioDryCount;
        UInt32      fClientSSRC;
        
        Bool16      fIsTCP;							//是否在TCP上传输数据
        QTSS_RTPTransportType   fTransportType;		//qtssRTPTransportTypeUDP = 0, qtssRTPTransportTypeReliableUDP = 1, qtssRTPTransportTypeTCP         = 2
        
        // HTTP params
        // Each stream has a set of thinning related tolerances,
        // that are dependent on prefs and parameters in the SETUP.
        // These params, as well as the current packet delay determine
        // whether a packet gets dropped.
        SInt32      fTurnThinningOffDelay_TCP;
        SInt32      fIncreaseThinningDelay_TCP;
        SInt32      fDropAllPacketsForThisStreamDelay_TCP;
        UInt32      fStalePacketsDropped_TCP;
        SInt64      fTimeStreamCaughtUp_TCP;
        SInt64      fLastQualityLevelIncreaseTime_TCP;
        //
        // Each stream has a set of thinning related tolerances,
        // that are dependent on prefs and parameters in the SETUP.
        // These params, as well as the current packet delay determine
        // whether a packet gets dropped.
        SInt32      fThinAllTheWayDelay;
        SInt32      fAlwaysThinDelay;
        SInt32      fStartThinningDelay;
        SInt32      fStartThickingDelay;
        SInt32      fThickAllTheWayDelay;
        SInt32      fQualityCheckInterval;
        SInt32      fDropAllPacketsForThisStreamDelay;
        UInt32      fStalePacketsDropped;
        SInt64      fLastCurrentPacketDelay;
        Bool16      fWaitOnLevelAdjustment;
        
        Float32     fBufferDelay; // from the sdp
        Float32     fLateToleranceInSec;	//late-tolerance value that came out of hte x-RTP-Options header,
                
        // Pointer to the stream ref (this is just a this pointer)
        QTSS_StreamRef  fStreamRef;
        
        UInt32      fCurrentAckTimeout;
        SInt32      fMaxSendAheadTimeMSec;
        
#if DEBUG
        UInt32      fNumPacketsDroppedOnTCPFlowControl;
        SInt64      fFlowControlStartedMsec;
        SInt64      fFlowControlDurationMsec;
#endif
        
        // If we are interleaving RTP data over the TCP connection, these are channel numbers to use for RTP & RTCP
		// RTP以及RTCP通道
        UInt8   fRTPChannel;
        UInt8   fRTCPChannel;
        
        QTSS_RTPNetworkMode     fNetworkMode;
        
        SInt64  fStreamStartTimeOSms;
        // acutally write the data out that way
        QTSS_Error  InterleavedWrite(void* inBuffer, UInt32 inLen, UInt32* outLenWritten, unsigned char channel );

        // implements the ReliableRTP protocol
        QTSS_Error  ReliableRTPWrite(void* inBuffer, UInt32 inLen, const SInt64& curPacketDelay);

        void        SetTCPThinningParams();
        QTSS_Error  TCPWrite(void* inBuffer, UInt32 inLen, UInt32* outLenWritten, UInt32 inFlags);

        static QTSSAttrInfoDict::AttrInfo   sAttributes[];
        static StrPtrLen                    sChannelNums[];
        static QTSS_ModuleState             sRTCPProcessModuleState;

        static char *noType;
        static char *UDP;
        static char *RUDP;
        static char *TCP;
        
        Bool16 UpdateQualityLevel(const SInt64& inTransmitTime, const SInt64& inCurrentPacketDelay,
                                        const SInt64& inCurrentTime, UInt32 inPacketSize);
        
        SInt32          GetQualityLevel();
        void            SetQualityLevel(SInt32 level);
        
        char *GetStreamTypeStr();
        enum { rtp = 0, rtcpSR = 1, rtcpRR = 2, rtcpACK = 3, rtcpAPP = 4 };
        Float32 GetStreamStartTimeSecs() { return (Float32) ((OS::Milliseconds() - this->fSession->GetSessionCreateTime())/1000.0); }
		//
		//以下函数打印Debug信息
		//
        void PrintPacket(char *inBuffer, UInt32 inLen, SInt32 inType); 
        void PrintRTP(char* packetBuff, UInt32 inLen);
        void PrintRTCPSenderReport(char* packetBuff, UInt32 inLen);
inline  void PrintPacketPrefEnabled(char *inBuffer,UInt32 inLen, SInt32 inType) { if (QTSServerInterface::GetServer()->GetPrefs()->PacketHeaderPrintfsEnabled() ) this->PrintPacket(inBuffer,inLen, inType); }

        void SetOverBufferState(RTSPRequestInterface* request);

};



/*SendSetupResponse(...)函数生成并发送标准回应, 该回应的格式如下: 

 1		RTSP/1.0 200 OK
 2		Server: DSS/5.5.1 (Build/489.8; Platform/Win32; Release/Darwin; )
 3		Cseq: 3
 4		Session: 130382322222193
 5		Last-Modified: Tue, 01 Mar 2005 01:39:22 GMT
 6		Cache-Control: must-revalidate
 7		Date: Fri, 07 Apr 2006 02:58:13 GMT
 8		Expires: Fri, 07 Apr 2006 02:58:13 GMT
 9		Transport: RTP/AVP;unicast;source=159.99.242.108;client_port=6972-6973;server_port=6970-6971;ssrc=00007029
10		x-Transport-Options: late-tolerance=2.384000
11		x-Retransmit: our-retransmit
12		x-Dynamic-Rate: 1
*/



#endif // __RTPSTREAM_H__
