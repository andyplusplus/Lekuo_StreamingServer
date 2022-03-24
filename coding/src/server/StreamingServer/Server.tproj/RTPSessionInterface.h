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
	File: RTPSessionInterface.h
    Contains:   API interface for objects to use to get access to attributes,
                data items, whatever, specific to RTP sessions (see RTPSession.h
                for more details on what that object is). This object
                implements the RTP Session Dictionary.
	对象用该接口访问属性, 数据项, 任何和RTP会话相关的参数, 该对象实现的RTP会话字典//

*/


#ifndef _RTPSESSIONINTERFACE_H_
#define _RTPSESSIONINTERFACE_H_

#include "QTSSDictionary.h"

#include "RTCPSRPacket.h"
#include "RTSPSessionInterface.h"
#include "TimeoutTask.h"
#include "Task.h"
#include "RTPBandwidthTracker.h"
#include "RTPOverbufferWindow.h"
#include "QTSServerInterface.h"
#include "OSMutex.h"
#include "atomic.h"
//_WZD_ADD_Start
#include "ChannelElem_4_Channel_S_map.h"
//_WZD_ADD_End
//qtssClientSessionObjectType
class RTPSessionInterface : public QTSSDictionary, public Task
{
	public:
	//_WZD_ADD_Start
	LP_S_ChannelElem m_LP_S_ChannelElem;		//2008.05.30 Add this to be used in ModuleAdaptor.cpp
	LP_S_ConnectionElem m_LP_S_ConnectionElem;
	//_WZD_ADD_End
    public:
    
        // Initializes dictionary resources
        static void Initialize();	//初始化字典资源
        
        //
        // CONSTRUCTOR / DESTRUCTOR
        
        RTPSessionInterface();			//构造函数, 把相关属性值存入字典

        virtual ~RTPSessionInterface()  //析构函数
            {   
                if (GetQualityLevel() != 0)
                    QTSServerInterface::GetServer()->IncrementNumThinned(-1);
                if (fRTSPSession != NULL)
                    fRTSPSession->DecrementObjectHolderCount();
                delete [] fSRBuffer.Ptr;
                delete [] fAuthNonce.Ptr; 
                delete [] fAuthOpaque.Ptr;

			}

		//_WZD_ADD_Start 
		//Since RTPSession is inherited from RTPSessionInterface, 
		//  RTPSession::~RTPSession will delete RTPStreaming, 
		//  so we should detach RTPStreaming from m_LP_S_ConnectionElem
		//void WZD_S_ConnectionElem_Assert()
		//{
		//	if(m_LP_S_ConnectionElem.get())
		//	{
		//		int i =1;
		//	}
		//	assert(m_LP_S_ConnectionElem.get());
		//}

		void WZD_Delete_RTPStream_From_ConnectionElem()
		{
			if(m_LP_S_ConnectionElem.get())
			{
				m_LP_S_ConnectionElem->clearRTPStreamObject();
			}
		};
		//_WZD_ADD_End


        virtual void SetValueComplete(UInt32 inAttrIndex, QTSSDictionaryMap* inMap,
							UInt32 inValueIndex, void* inNewValue, UInt32 inNewValueLen);	//???

        //Timeouts. This allows clients to refresh the timeout on this session
        void    RefreshTimeout()        { fTimeoutTask.RefreshTimeout(); }
        void    RefreshRTSPTimeout()    { if (fRTSPSession != NULL) fRTSPSession->RefreshTimeout(); }
        void    RefreshTimeouts()       { RefreshTimeout(); RefreshRTSPTimeout();}
        
        //
        // ACCESSORS
        
        Bool16  IsFirstPlay()           { return fIsFirstPlay; }
        SInt64  GetFirstPlayTime()      { return fFirstPlayTime; }
        //Time (msec) most recent play was issued
        SInt64  GetPlayTime()           { return fPlayTime; }
        SInt64  GetNTPPlayTime()        { return fNTPPlayTime; }
        SInt64  GetSessionCreateTime()  { return fSessionCreateTime; }
        //Time (msec) most recent play, adjusted for start time of the movie
        //ex: PlayTime() == 20,000. Client said start 10 sec into the movie,
        //so AdjustedPlayTime() == 10,000
        QTSS_PlayFlags GetPlayFlags()   { return fPlayFlags; }
        OSMutex*        GetSessionMutex()   { return &fSessionMutex; }
        UInt32          GetPacketsSent()    { return fPacketsSent; }
        UInt32          GetBytesSent()  { return fBytesSent; }		//返回到目前为止已经发送的字节
        OSRef*      GetRef()            { return &fRTPMapElem; }
        RTSPSessionInterface* GetRTSPSession() { return fRTSPSession; }
        UInt32      GetMovieAvgBitrate(){ return fMovieAverageBitRate; }
        QTSS_CliSesTeardownReason GetTeardownReason() { return fTeardownReason; }
        QTSS_RTPSessionState    GetSessionState() { return fState; }
        void    SetUniqueID(UInt32 theID)   {fUniqueID = theID;}
        UInt32  GetUniqueID()           { return fUniqueID; }
        RTPBandwidthTracker* GetBandwidthTracker() { return &fTracker; }
        RTPOverbufferWindow* GetOverbufferWindow() { return &fOverbufferWindow; }
        UInt32  GetFramesSkipped() { return fFramesSkipped; }
        
        //
        // MEMORY FOR RTCP PACKETS
        
        //
        // Class for easily building a standard RTCP SR
        RTCPSRPacket*   GetSRPacket()       { return &fRTCPSRPacket; }

        //
        // Memory if you want to build your own
        char*           GetSRBuffer(UInt32 inSRLen);		//获得指定长度的发送者报告
        
        //
        // STATISTICS UPDATING
        
        //The session tracks the total number of bytes sent during the session.
        //Streams can update that value by calling this function
        void            UpdateBytesSent(UInt32 bytesSent) { fBytesSent += bytesSent; }
                        
        //The session tracks the total number of packets sent during the session.
        //Streams can update that value by calling this function                
        void            UpdatePacketsSent(UInt32 packetsSent) { fPacketsSent += packetsSent; }
                                        
        void            UpdateCurrentBitRate(const SInt64& curTime)		//每10秒更新一次,计算当前的比特率
            { if (curTime > (fLastBitRateUpdateTime + 10000)) this->UpdateBitRateInternal(curTime); }

        void            SetAllTracksInterleaved(Bool16 newValue) { fAllTracksInterleaved = newValue; }      
        //
        // RTSP RESPONSES

        // This function appends a session header to the SETUP response, and
        // checks to see if it is a 304 Not Modified. If it is, it sends the entire
        // response and returns an error
		// 该函数在SETUP回应上增加一个会话头, 并检测是否是未修改的头qtssRedirectNotModified,如果是, 发送完整回应,并返回错误
        QTSS_Error DoSessionSetupResponse(RTSPRequestInterface* inRequest);
        
        //
        // RTSP SESSION
                                
        // This object has a current RTSP session. This may change over the
        // life of the RTSPSession, so update it. It keeps an RTSP session in
        // case interleaved data or commands need to be sent back to the client. 
        void            UpdateRTSPSession(RTSPSessionInterface* inNewRTSPSession);
                
        // let's RTSP Session pass along it's query string
        void            SetQueryString( StrPtrLen* queryString );		//函数没用有
        
        // SETTERS and ACCESSORS for auth information
        // Authentication information that needs to be kept around for the duration of the session      
        QTSS_AuthScheme GetAuthScheme() { return fAuthScheme; }
        StrPtrLen*      GetAuthNonce() { return &fAuthNonce; }
        UInt32          GetAuthQop() { return fAuthQop; }
        UInt32          GetAuthNonceCount() { return fAuthNonceCount; }
        StrPtrLen*      GetAuthOpaque() { return &fAuthOpaque; }
        void            SetAuthScheme(QTSS_AuthScheme scheme) { fAuthScheme = scheme; }
        // Use this if the auth scheme or the qop has to be changed from the defaults 
        // of scheme = Digest, and qop = auth
        void            SetChallengeParams(QTSS_AuthScheme scheme, UInt32 qop, Bool16 newNonce, Bool16 createOpaque);
        // Use this otherwise...if newNonce == true, it will create a new nonce
        // and reset nonce count. If newNonce == false but nonce was never created earlier
        // a nonce will be created. If newNonce == false, and there is an existing nonce,
        // the nounce count will be incremented.
        void            UpdateDigestAuthChallengeParams(Bool16 newNonce, Bool16 createOpaque, UInt32 qop);
    
        Float32* GetPacketLossPercent() { UInt32 outLen;return  (Float32*) this->PacketLossPercent(this, &outLen);}

        SInt32          GetQualityLevel()   { return fSessionQualityLevel; }
        SInt32*         GetQualityLevelPtr()    { return &fSessionQualityLevel; }
        void            SetQualityLevel(SInt32 level)   { if (fSessionQualityLevel == 0 && level != 0)
                                                                QTSServerInterface::GetServer()->IncrementNumThinned(1);
                                                            else if (fSessionQualityLevel != 0 && level == 0)
                                                                QTSServerInterface::GetServer()->IncrementNumThinned(-1);
                                                            fSessionQualityLevel = level; 
                                                         }
        SInt64          fLastQualityCheckTime;
		SInt64			fLastQualityCheckMediaTime;
		Bool16			fStartedThinning;
		
        // Used by RTPStream to increment the RTCP packet and byte counts.
        void            IncrTotalRTCPPacketsRecv()         { fTotalRTCPPacketsRecv++; }
        UInt32          GetTotalRTCPPacketsRecv()          { return fTotalRTCPPacketsRecv; }
        void            IncrTotalRTCPBytesRecv(UInt16 cnt) { fTotalRTCPBytesRecv += cnt; }
        UInt32          GetTotalRTCPBytesRecv()            { return fTotalRTCPBytesRecv; }

    protected:
    
        // These variables are setup by the derived RTPSession object when
        // Play and Pause get called

        //Some stream related information that is shared amongst all streams
        Bool16      fIsFirstPlay;
        Bool16      fAllTracksInterleaved;
        SInt64      fFirstPlayTime;//in milliseconds  		//qtssCliSesFirstPlayTimeInMsec 首次调用QTSS_Play的时间，以毫秒计。
        SInt64      fPlayTime;				//qtssCliSesPlayTimeInMsec 最近一次调用QTSS_Play的时间，以毫秒计。
        SInt64      fAdjustedPlayTime;		//qtssCliSesAdjustedPlayTimeInMsec 向前调整过的最近一次播放请求的发出时间，这是为了推迟发送数据包，直到播放请求的响应被发出。这个时间以毫秒计。
        SInt64      fNTPPlayTime;
        SInt64      fNextSendPacketsTime;	//下一个包的发包时间
        
        SInt32      fSessionQualityLevel;
        
        //keeps track of whether we are playing or not
        QTSS_RTPSessionState fState;	//qtssCliSesState 当前会话的状态。可能的值是	qtssPausedState, qtssPlayingState
        
        // If we are playing, this are the play flags that were set on play
        QTSS_PlayFlags  fPlayFlags;
			//qtssPlayFlagsSendRTCP           = 0x00000001,   // have the server generate RTCP Sender Reports 
			//qtssPlayFlagsAppendServerInfo   = 0x00000002    // have the server append the server info APP packet to your RTCP Sender Reports

        //Session mutex. This mutex should be grabbed before invoking the module
        //responsible for managing this session. This allows the module to be
        //non-preemptive-safe with respect to a session
        OSMutex     fSessionMutex;

        //Stores the session ID
        OSRef               fRTPMapElem;										//该RTP会话的引用表项, 用于加入服务器对象的fRTPMap,key设置为RTSP的客户端会话ID
        char                fRTSPSessionIDBuf[QTSS_MAX_SESSION_ID_LENGTH + 4];	//RTSP的会话ID
        
		//以下参数用于调整流量
        UInt32      fLastBitRateBytes;			//上一次更新时间以前发送的字节数
        SInt64      fLastBitRateUpdateTime;		//上一次更新比特率的时间
        UInt32      fMovieCurrentBitRate;		//由相关参数获得当前的比特率  		//qtssCliSesCurrentBitRate 电影的位率。
        
        // In order to facilitate sending data over the RTSP channel from
        // an RTP session, we store a pointer to the RTSP session used in
        // the last RTSP request.
        RTSPSessionInterface* fRTSPSession;		//对象的RTSP会话


	private:
    
        //
        // Utility function for calculating current bit rate
        void UpdateBitRateInternal(const SInt64& curTime);

        static void* PacketLossPercent(QTSSDictionary* inSession, UInt32* outLen);	//计算丢包率
        static void* TimeConnected(QTSSDictionary* inSession, UInt32* outLen);		//已经连接的时间
        static void* CurrentBitRate(QTSSDictionary* inSession, UInt32* outLen);		//先更新比特率,然后返回比特率
        
        // Create nonce
        void CreateDigestAuthenticationNonce();										//关于认证的, skipfirst

        // One of the RTP session attributes is an iterated list of all streams.
        // As an optimization, we preallocate a "large" buffer of stream pointers,
        // even though we don't know how many streams we need at first.
        enum
        {
            kStreamBufSize              = 4,
            kUserAgentBufSize           = 256,
            kPresentationURLSize        = 256,
            kFullRequestURLBufferSize   = 256,
            kRequestHostNameBufferSize  = 80,
            
            kIPAddrStrBufSize           = 20,
            kLocalDNSBufSize            = 80,
            
            kAuthNonceBufSize           = 32,
            kAuthOpaqueBufSize          = 32,
            
        };
        
        void*       fStreamBuffer[kStreamBufSize];				//qtssCliSesStreamObjects 可重复访问的属性，包含属于当前会话的所有RTP流的引用（QTSS_RTPStreamObject）。

        
        // theses are dictionary items picked up by the RTSPSession
        // but we need to store copies of them for logging purposes.
        char        fUserAgentBuffer[kUserAgentBufSize];			//qtssCliSesFirstUserAgent
        char        fPresentationURL[kPresentationURLSize];         //qtssCliSesPresentationURL 当前会话的表现URL。这个URL是会话的"基础" URL。
		                                                     //        对于这个表现ＵRL的RTSP请求都被认为会影响会话的所有流。 eg /foo/bar.mov
        char        fFullRequestURL[kFullRequestURLBufferSize];     // eg rtsp://yourdomain.com/foo/bar.mov
									//qtssCliSesFullURL 当前会话完整的表现URL，和qtssCliSesPresentationURL属性相同，只是包含rtsp://domain_name的前缀。

        char        fRequestHostName[kRequestHostNameBufferSize];   // eg yourdomain.com   		//qtssCliSesHostName 当前会话的主机名称。同样的，也是qtssCliSesFullURL属性中的域名部分。
    
        char        fRTSPSessRemoteAddrStr[kIPAddrStrBufSize];		//qtssCliRTSPSessRemoteAddrStr 客户端的IP地址，以带点的十进制数格式表示。
        char        fRTSPSessLocalDNS[kLocalDNSBufSize];			//qtssCliRTSPSessLocalDNS 当前RTSP连接中的本地IP地址对应的DNS名称。
        char        fRTSPSessLocalAddrStr[kIPAddrStrBufSize];		//qtssCliRTSPSessLocalAddrStr 当前RTSP连接中的本地IP地址，以带点的十进制数格式表示。
        
        char        fUserNameBuf[RTSPSessionInterface::kMaxUserNameLen];		//qtssCliRTSPSesUserName 发起最新请求的用户名。
        char        fUserPasswordBuf[RTSPSessionInterface::kMaxUserPasswordLen];
        char        fUserRealmBuf[RTSPSessionInterface::kMaxUserRealmLen];		//qtssCliRTSPSesURLRealm 最新请求的范围信息。
        UInt32      fLastRTSPReqRealStatusCode;

        //for timing out this session
        TimeoutTask fTimeoutTask;		//指向超时任务
        UInt32      fTimeout;			//RTP超时时间, 其值从配置文件读取
        
        // Time when this session got created
        SInt64      fSessionCreateTime;	//会话创建时间			//qtssCliSesCreateTimeInMsec 会话创建的时间，以毫秒计。

        //Packet priority levels. Each stream has a current level, and
        //the module that owns this session sets what the number of levels is.
        UInt32      fNumQualityLevels;	//该值没有
        
        //Statistics 相关的统计参数
        UInt32 fBytesSent;				//到目前为止已经发送的字节数		//qtssCliSesRTPBytesSent 当前会话已经发送的RTP字节数。
        UInt32 fPacketsSent;			//qtssCliSesRTPPacketsSent 当前会话已经发送的RTP数据包的数目。
        Float32 fPacketLossPercent;
        SInt64 fTimeConnected;			//已经连接的时间
        UInt32 fTotalRTCPPacketsRecv;
        UInt32 fTotalRTCPBytesRecv;
        // Movie size & movie duration. It may not be so good to associate these
        // statistics with the movie, for a session MAY have multiple movies...
        // however, the 1 movie assumption is in too many subsystems at this point
        Float64     fMovieDuration;		//qtssCliSesMovieDurationInSecs 当前会话的电影时长，以秒计。如果模块没有进行设定，则该值为０。
        UInt64      fMovieSizeInBytes;		//qtssCliSesMovieSizeInBytes 电影的尺寸，以字节计。如果模块没有进行设定，则该值为０。
        UInt32      fMovieAverageBitRate;		//qtssCliSesMovieAverageBitRate 每秒钟的平均位率，这是由总共的RTP位除以电影时长得到。如果模块没有进行设定，则该值为０。
        
        QTSS_CliSesTeardownReason fTeardownReason;		//qtssCliTeardownReason 连接断开的原因。如果不是客户端请求，则模块必须调用QTSS_Teardown函数来设置连接断开的原因。
        // So the streams can send sender reports
        UInt32      fUniqueID;
        
        RTCPSRPacket        fRTCPSRPacket;	//RTCP数据包
        StrPtrLen           fSRBuffer;		//发送者报告
        
        RTPBandwidthTracker fTracker;		//
        RTPOverbufferWindow fOverbufferWindow;
        
        // Built in dictionary attributes
        static QTSSAttrInfoDict::AttrInfo   sAttributes[];
        static unsigned int sRTPSessionIDCounter;

        // Authentication information that needs to be kept around
        // for the duration of the session      
        QTSS_AuthScheme             fAuthScheme;
        StrPtrLen                   fAuthNonce;
        UInt32                      fAuthQop;
        UInt32                      fAuthNonceCount;                    
        StrPtrLen                   fAuthOpaque;
        UInt32                      fQualityUpdate;
        
        UInt32                      fFramesSkipped;
        Bool16                      fOverBufferEnabled;
};

#endif //_RTPSESSIONINTERFACE_H_
