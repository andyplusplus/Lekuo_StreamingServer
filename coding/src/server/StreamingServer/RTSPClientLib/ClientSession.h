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
    File:       ClientSession.h
    该类封装了TCPClientSocket以及RTSPClient, 这两个对象采用同一个socket
*/

#ifndef __CLIENT_SESSION__
#define __CLIENT_SESSION__

#include "Task.h"
#include "TimeoutTask.h"
#include "RTSPClient.h"
#include "ClientSocket.h"
#include "SDPSourceInfo.h"
#include "UDPSocket.h"


// _WZD_Start
#include "LeKuo.h"
#include "lkRTPPacketFormat.h"
#include "ILkTokenParser.h"
#include "IMsgReceiver.h"
#include "IDataReceiver.h"
#include "StdRsType.h"
#include "LkStrParser.h"
#include "LOG.h"
class RelayDVR;

class ClientSession : public Task
#ifdef _WZD_CODE_ENABLED_
	, public IChannel
#endif //_WZD_CODE_ENABLED_
{
private:
	bool m_isFirstPacket;
	enum
	{
		en_Print_Received_DataTrace = 0,
		en_Print_DataLen_Range = 10000,
		en_ReceiveData = 1,
	};

public:
	friend RelayDVR;

	static const SInt64 kIdleTimeoutInMsec = 20000;
	typedef UInt32 ClientType;	//在构造函数中使用,该参数的不同fSocket将指向不同的对象

	ClientSession(rsString inStr, Tag inTag, IMutex* inMutex, LPIDVR inDVR);
	
	virtual LK_Error Attach_DataReceiver(LPIDataReceiver inPlayer);
	virtual LPIDataReceiver Detach_DataReceiver();

	virtual LK_Error Play(const char* inPlayOption );
	virtual BOOL IsPlaying();
	virtual LK_Error Stop();		//只是DetachReceiver
	virtual LK_Error Destroy();

	virtual MetadataLen getMetadataLen();//{return 0;};
	virtual MetadataLen getMetadata(unsigned char* outDataBuffer, MetadataLen inBufferLength);// {return 0;};

private:
	void ClientSession_Play(UInt32 inAddr, UInt16 inPort, char* inURL,
		ClientType inClientType, UInt32 inHTTPCookie, 
		Float32 inLateTolerance, char* inMetaInfoFields, Bool16 inVerbosePrinting);

	void ClientSession_Play(const char* inpClientSessionStr);

	rsString m_CreateStr;
	rsString m_PlayStr;

	LPIDVR m_DVR;
	IMutex* m_Mutex;
	Tag m_Tag;
	LPIDataReceiver m_IDataReceiver;
	BOOL m_PingServerRequest;       //是否该请求只是PingServer的状态，如果是，则不做后续请求
	
	unsigned char m_MetaDataBuffer[RS_MAX_PAYLOAD_SIZE];
	MetadataLen   m_MetaDataLen;


	void TearDown()
	{
		fState = kSendingTeardown;	
		Signal(ClientSession::kTeardownEvent); 

		#ifdef _WZD_CODE_ENABLED_
			m_DVR = NULL;		//the task thread will delete this object, and this objec will not related with the DVR, refer to ClientSession::~ClientSession()
		#endif
	}

//_WZD_CODE_End_

    public:

        enum
        {
            LK_CLIENT_TYPE_UDP          = 0,
            LK_CLIENT_TYPE_TCP          = 1,
            LK_CLIENT_TYPE_HTTP         = 2,
            LK_CLIENT_TYPE_HTTPDropPost = 3,
            LK_CLIENT_TYPE_ReliableUDP  = 4
        };
        typedef UInt32 ClientType;	//在构造函数中使用,该参数的不同fSocket将指向不同的对象

		//初始化变量, 构造socket以及相关变量
        ClientSession(  UInt32 inAddr, UInt16 inPort, char* inURL,			//服务器地址； 接听客户端端口，如554； 连接URL:RTSP://.../...
                        ClientType inClientType,							//客户端类型：LK_CLIENT_TYPE_UDP，LK_CLIENT_TYPE_TCP，LK_CLIENT_TYPE_HTTP，LK_CLIENT_TYPE_HTTPDropPost，LK_CLIENT_TYPE_ReliableUDP
                        UInt32 inDurationInSec, UInt32 inStartPlayTimeInSec,
                        UInt32 inRTCPIntervalInSec, UInt32 inOptionsIntervalInSec,

                        UInt32 inHTTPCookie, 
						Bool16 inAppendJunkData,		//是否在发送的Describe请求后加200字节填充数据，这些数据为'd'
						UInt32 inReadInterval,

                        UInt32 inSockRcvBufSize,		//接收Buffer的大小，如32768
						Float32 inLateTolerance, 
						char* inMetaInfoFields,

                        Float32 inSpeed, 
						Bool16 inVerbosePrinting, 
						char* inPacketRangePlayHeader, 
						UInt32 inOverbufferWindowSizeInK,

                        Bool16 sendOptions,					//是否要发送Option请求，如TRUE
						Bool16 requestRandomData, 
						SInt32 randomDataSize 

						#ifdef _WZD_CODE_ENABLED_
						, LPIDVR inDVR
						, LPIDataReceiver inDataReceiver
						, Tag inTag
						#endif //_WZD_CODE_ENABLED_						
						);

private:
	//从SDP文件找出所有流, 并删除
	virtual ~ClientSession();


public:

		//
        // Signals.
        //
        // Send a kKillEvent to delete this object.
        // Send a kTeardownEvent to tell the object to send a TEARDOWN and abort
        
        enum
        {
            kTeardownEvent = 0x00000100
        };
        
		//1. 维护客户端RTSP的状态机
		//2. 实现RTP的读取过程
		//3. 实现RTCP过程
        virtual SInt64 Run();
        
        //
        // States. Find out what the object is currently doing
        enum	//定义当前客户端所进行的操作
        {
            kSendingOptions     = 0,
            kSendingDescribe    = 1,
            kSendingSetup       = 2,
            kSendingPlay        = 3,
            kPlaying            = 4,
            kSendingTeardown    = 5,
            kDone               = 6
        };
        //
        // Why did this session die?
        enum
        {
            kDiedNormally       = 0,    // Session went fine
            kTeardownFailed     = 1,    // Teardown failed, but session stats are all valid
            kRequestFailed      = 2,    // Session couldn't be setup because the server returned an error
            kBadSDP             = 3,    // Server sent back some bad SDP，当Server返回的SDP中包含流数为0时，设置为此值
            kSessionTimedout    = 4,    // Server not responding
            kConnectionFailed   = 5,    // Couldn't connect at all.
            kDiedWhilePlaying   = 6,    // Connection was forceably closed while playing the movie
			kWZDResponseTestOK  = 7,  //  Added by WZD, to the testing of send options
        };
        
        //
        // Once this client session is completely done with the TEARDOWN and ready to be
        // destructed, this will return true. Until it returns true, this object should not
        // be deleted. When it does return true, this object should be deleted.
        Bool16  IsDone()        { return fState == kDone; }
        
        //
        // ACCESSORS
    
        RTSPClient*             GetClient()         { return fClient; }
        ClientSocket*           GetSocket()         { return fSocket; }
        SDPSourceInfo*          GetSDPInfo()        { return &fSDPParser; }
        UInt32                  GetState()          { return fState; }
        
        // When this object is in the kDone state, this will tell you why the session died.
        UInt32                  GetReasonForDying() { return fDeathReason; }
        UInt32                  GetRequestStatus()  { return fClient->GetStatus(); }
        
        // Tells you the total time we were receiving packets. You can use this for computing bit rate
        SInt64                  GetTotalPlayTimeInMsec() { return fTotalPlayTime; }
        
        QTSS_RTPPayloadType     GetTrackType(UInt32 inTrackIndex)
                                    { return fSDPParser.GetStreamInfo(inTrackIndex)->fPayloadType; }
        UInt32                  GetNumPacketsReceived(UInt32 inTrackIndex)
                                    { return fStats[inTrackIndex].fNumPacketsReceived; }
        UInt32                  GetNumBytesReceived(UInt32 inTrackIndex)
                                    { return fStats[inTrackIndex].fNumBytesReceived; }
        UInt32                  GetNumPacketsLost(UInt32 inTrackIndex)
                                    { return fStats[inTrackIndex].fNumLostPackets; }
        UInt32                  GetNumPacketsOutOfOrder(UInt32 inTrackIndex)
                                    { return fStats[inTrackIndex].fNumOutOfOrderPackets; }
        UInt32                  GetNumDuplicates(UInt32 inTrackIndex)
                                    { return fStats[inTrackIndex].fNumDuplicates; }
        UInt32                  GetNumAcks(UInt32 inTrackIndex)
                                    { return fStats[inTrackIndex].fNumAcks; }
                
        UInt32   GetSessionPacketsReceived()  { UInt32 result = fNumPacketsReceived; fNumPacketsReceived = 0; return result; }
        //
        // Global stats
        static UInt32   GetActiveConnections()          { return sActiveConnections; }
        static UInt32   GetPlayingConnections()         { return sPlayingConnections; }
        static UInt32   GetConnectionAttempts()         { return sTotalConnectionAttempts; }
        static UInt32   GetConnectionBytesReceived()    { UInt32 result = sBytesReceived; sBytesReceived = 0; return result; }
        static UInt32   GetConnectionPacketsReceived()  { UInt32 result = sPacketsReceived; sPacketsReceived = 0; return result; }

      
    private:
    
        enum
        {
            kRawRTSPControlType         = 0,
            kRTSPHTTPControlType        = 1,
            kRTSPHTTPDropPostControlType= 2
        };
        typedef UInt32 ControlType;
        
        enum
        {
            kUDPTransportType           = 0,
            kReliableUDPTransportType   = 1,
            kTCPTransportType           = 2
        };
        typedef UInt32 TransportType;
        
        ClientSocket*   fSocket;		// Connection object
        RTSPClient*     fClient;		// RTSP Client对象，Manages the client connection
        SDPSourceInfo   fSDPParser;		// Parses the SDP in the DESCRIBE response
        TimeoutTask     fTimeoutTask;	// Kills this connection in the event the server isn't responding
        
        ControlType     fControlType;	//流量控制类型，见ControlType的定义
        TransportType   fTransportType;	//数据传输方式，见TransportType的定义，UDP或TCP，或可靠的UDP

        UInt32          fDurationInSec;	//
        UInt32          fStartPlayTimeInSec;
        UInt32          fRTCPIntervalInSec;
        UInt32          fOptionsIntervalInSec;
        
        Bool16          fOptions;
        Bool16          fOptionsRequestRandomData;
        SInt32          fOptionsRandomDataSize;
        SInt64          fTransactionStartTimeMilli;

        UInt32          fState;			// For managing the state machine，状态机的当前状态
        UInt32          fDeathReason;
        UInt32          fNumSetups;
        UDPSocket**     fUDPSocketArray;
        
        SInt64          fPlayTime;
        SInt64          fTotalPlayTime;
        SInt64          fLastRTCPTime;
        
        Bool16          fTeardownImmediately;
        Bool16          fAppendJunk;
        UInt32          fReadInterval;
        UInt32          fSockRcvBufSize;
        
        Float32         fSpeed;
        char*           fPacketRangePlayHeader;
        
        //
        // Client stats
        struct TrackStats
        {
            enum
            {
                kSeqNumMapSize = 100,
                kHalfSeqNumMap = 50
            };
        
            UInt16          fDestRTCPPort;
            UInt32          fNumPacketsReceived;
            UInt32          fNumBytesReceived;
            UInt32          fNumLostPackets;
            UInt32          fNumOutOfOrderPackets;
            UInt32          fNumThrownAwayPackets;
            UInt8           fSequenceNumberMap[kSeqNumMapSize];
            UInt16          fWrapSeqNum;
            UInt16          fLastSeqNum;
            UInt32          fSSRC;
            Bool16          fIsSSRCValid;
            
            UInt16          fHighestSeqNum;
            UInt16          fLastAckedSeqNum;
            Bool16          fHighestSeqNumValid;
            
            UInt32          fNumAcks;
            UInt32          fNumDuplicates;
            
        };
        TrackStats*         fStats;						//为每一个流定义一个数据该数据结构，用于跟踪数据传输
        UInt32              fOverbufferWindowSizeInK;
        UInt32              fCurRTCPTrack;
        UInt32              fNumPacketsReceived;
        //
        // Global stats
        static UInt32           sActiveConnections;
        static UInt32           sPlayingConnections;
        static UInt32           sTotalConnectionAttempts;
        static UInt32           sBytesReceived;
        static UInt32           sPacketsReceived;
		//
		//建立UDP端口
        void    SetupUDPSockets();
		//处理媒体数据
        void    ProcessMediaPacket(char* inPacket, UInt32 inLength, UInt32 inTrackID, Bool16 isRTCP);
		//
		//从Socket读取媒体数据, 并在RTCP端口上返回RTCP数据
        OS_Error    ReadMediaData();
		//
		//通过RTCP端口发送接收者报告
        OS_Error    SendReceiverReport(UInt32 inTrackID);
		//
		//通过RTCP端口返回确认包
        void    AckPackets(UInt32 inTrackIndex, UInt16 inCurSeqNum, Bool16 inCurSeqNumValid);

		//_WZD_ADD_Start      refer to UDPSocketPool.h  for kLowestUDPPort, this two value is related
		enum	
		{
			kLowestUDPPort_Client = 36970,  //UInt16
			kHighestUDPPort_Client = 65533 //UInt16
		};
		//_WZD_ADD_End

};

#endif //__CLIENT_SESSION__
