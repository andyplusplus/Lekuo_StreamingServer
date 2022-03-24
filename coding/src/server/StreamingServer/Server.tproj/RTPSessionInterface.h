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
	�����øýӿڷ�������, ������, �κκ�RTP�Ự��صĲ���, �ö���ʵ�ֵ�RTP�Ự�ֵ�//

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
        static void Initialize();	//��ʼ���ֵ���Դ
        
        //
        // CONSTRUCTOR / DESTRUCTOR
        
        RTPSessionInterface();			//���캯��, ���������ֵ�����ֵ�

        virtual ~RTPSessionInterface()  //��������
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
        UInt32          GetBytesSent()  { return fBytesSent; }		//���ص�ĿǰΪֹ�Ѿ����͵��ֽ�
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
        char*           GetSRBuffer(UInt32 inSRLen);		//���ָ�����ȵķ����߱���
        
        //
        // STATISTICS UPDATING
        
        //The session tracks the total number of bytes sent during the session.
        //Streams can update that value by calling this function
        void            UpdateBytesSent(UInt32 bytesSent) { fBytesSent += bytesSent; }
                        
        //The session tracks the total number of packets sent during the session.
        //Streams can update that value by calling this function                
        void            UpdatePacketsSent(UInt32 packetsSent) { fPacketsSent += packetsSent; }
                                        
        void            UpdateCurrentBitRate(const SInt64& curTime)		//ÿ10�����һ��,���㵱ǰ�ı�����
            { if (curTime > (fLastBitRateUpdateTime + 10000)) this->UpdateBitRateInternal(curTime); }

        void            SetAllTracksInterleaved(Bool16 newValue) { fAllTracksInterleaved = newValue; }      
        //
        // RTSP RESPONSES

        // This function appends a session header to the SETUP response, and
        // checks to see if it is a 304 Not Modified. If it is, it sends the entire
        // response and returns an error
		// �ú�����SETUP��Ӧ������һ���Ựͷ, ������Ƿ���δ�޸ĵ�ͷqtssRedirectNotModified,�����, ����������Ӧ,�����ش���
        QTSS_Error DoSessionSetupResponse(RTSPRequestInterface* inRequest);
        
        //
        // RTSP SESSION
                                
        // This object has a current RTSP session. This may change over the
        // life of the RTSPSession, so update it. It keeps an RTSP session in
        // case interleaved data or commands need to be sent back to the client. 
        void            UpdateRTSPSession(RTSPSessionInterface* inNewRTSPSession);
                
        // let's RTSP Session pass along it's query string
        void            SetQueryString( StrPtrLen* queryString );		//����û����
        
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
        SInt64      fFirstPlayTime;//in milliseconds  		//qtssCliSesFirstPlayTimeInMsec �״ε���QTSS_Play��ʱ�䣬�Ժ���ơ�
        SInt64      fPlayTime;				//qtssCliSesPlayTimeInMsec ���һ�ε���QTSS_Play��ʱ�䣬�Ժ���ơ�
        SInt64      fAdjustedPlayTime;		//qtssCliSesAdjustedPlayTimeInMsec ��ǰ�����������һ�β�������ķ���ʱ�䣬����Ϊ���Ƴٷ������ݰ���ֱ�������������Ӧ�����������ʱ���Ժ���ơ�
        SInt64      fNTPPlayTime;
        SInt64      fNextSendPacketsTime;	//��һ�����ķ���ʱ��
        
        SInt32      fSessionQualityLevel;
        
        //keeps track of whether we are playing or not
        QTSS_RTPSessionState fState;	//qtssCliSesState ��ǰ�Ự��״̬�����ܵ�ֵ��	qtssPausedState, qtssPlayingState
        
        // If we are playing, this are the play flags that were set on play
        QTSS_PlayFlags  fPlayFlags;
			//qtssPlayFlagsSendRTCP           = 0x00000001,   // have the server generate RTCP Sender Reports 
			//qtssPlayFlagsAppendServerInfo   = 0x00000002    // have the server append the server info APP packet to your RTCP Sender Reports

        //Session mutex. This mutex should be grabbed before invoking the module
        //responsible for managing this session. This allows the module to be
        //non-preemptive-safe with respect to a session
        OSMutex     fSessionMutex;

        //Stores the session ID
        OSRef               fRTPMapElem;										//��RTP�Ự�����ñ���, ���ڼ�������������fRTPMap,key����ΪRTSP�Ŀͻ��˻ỰID
        char                fRTSPSessionIDBuf[QTSS_MAX_SESSION_ID_LENGTH + 4];	//RTSP�ĻỰID
        
		//���²������ڵ�������
        UInt32      fLastBitRateBytes;			//��һ�θ���ʱ����ǰ���͵��ֽ���
        SInt64      fLastBitRateUpdateTime;		//��һ�θ��±����ʵ�ʱ��
        UInt32      fMovieCurrentBitRate;		//����ز�����õ�ǰ�ı�����  		//qtssCliSesCurrentBitRate ��Ӱ��λ�ʡ�
        
        // In order to facilitate sending data over the RTSP channel from
        // an RTP session, we store a pointer to the RTSP session used in
        // the last RTSP request.
        RTSPSessionInterface* fRTSPSession;		//�����RTSP�Ự


	private:
    
        //
        // Utility function for calculating current bit rate
        void UpdateBitRateInternal(const SInt64& curTime);

        static void* PacketLossPercent(QTSSDictionary* inSession, UInt32* outLen);	//���㶪����
        static void* TimeConnected(QTSSDictionary* inSession, UInt32* outLen);		//�Ѿ����ӵ�ʱ��
        static void* CurrentBitRate(QTSSDictionary* inSession, UInt32* outLen);		//�ȸ��±�����,Ȼ�󷵻ر�����
        
        // Create nonce
        void CreateDigestAuthenticationNonce();										//������֤��, skipfirst

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
        
        void*       fStreamBuffer[kStreamBufSize];				//qtssCliSesStreamObjects ���ظ����ʵ����ԣ��������ڵ�ǰ�Ự������RTP�������ã�QTSS_RTPStreamObject����

        
        // theses are dictionary items picked up by the RTSPSession
        // but we need to store copies of them for logging purposes.
        char        fUserAgentBuffer[kUserAgentBufSize];			//qtssCliSesFirstUserAgent
        char        fPresentationURL[kPresentationURLSize];         //qtssCliSesPresentationURL ��ǰ�Ự�ı���URL�����URL�ǻỰ��"����" URL��
		                                                     //        ����������֣�RL��RTSP���󶼱���Ϊ��Ӱ��Ự���������� eg /foo/bar.mov
        char        fFullRequestURL[kFullRequestURLBufferSize];     // eg rtsp://yourdomain.com/foo/bar.mov
									//qtssCliSesFullURL ��ǰ�Ự�����ı���URL����qtssCliSesPresentationURL������ͬ��ֻ�ǰ���rtsp://domain_name��ǰ׺��

        char        fRequestHostName[kRequestHostNameBufferSize];   // eg yourdomain.com   		//qtssCliSesHostName ��ǰ�Ự���������ơ�ͬ���ģ�Ҳ��qtssCliSesFullURL�����е��������֡�
    
        char        fRTSPSessRemoteAddrStr[kIPAddrStrBufSize];		//qtssCliRTSPSessRemoteAddrStr �ͻ��˵�IP��ַ���Դ����ʮ��������ʽ��ʾ��
        char        fRTSPSessLocalDNS[kLocalDNSBufSize];			//qtssCliRTSPSessLocalDNS ��ǰRTSP�����еı���IP��ַ��Ӧ��DNS���ơ�
        char        fRTSPSessLocalAddrStr[kIPAddrStrBufSize];		//qtssCliRTSPSessLocalAddrStr ��ǰRTSP�����еı���IP��ַ���Դ����ʮ��������ʽ��ʾ��
        
        char        fUserNameBuf[RTSPSessionInterface::kMaxUserNameLen];		//qtssCliRTSPSesUserName ��������������û�����
        char        fUserPasswordBuf[RTSPSessionInterface::kMaxUserPasswordLen];
        char        fUserRealmBuf[RTSPSessionInterface::kMaxUserRealmLen];		//qtssCliRTSPSesURLRealm ��������ķ�Χ��Ϣ��
        UInt32      fLastRTSPReqRealStatusCode;

        //for timing out this session
        TimeoutTask fTimeoutTask;		//ָ��ʱ����
        UInt32      fTimeout;			//RTP��ʱʱ��, ��ֵ�������ļ���ȡ
        
        // Time when this session got created
        SInt64      fSessionCreateTime;	//�Ự����ʱ��			//qtssCliSesCreateTimeInMsec �Ự������ʱ�䣬�Ժ���ơ�

        //Packet priority levels. Each stream has a current level, and
        //the module that owns this session sets what the number of levels is.
        UInt32      fNumQualityLevels;	//��ֵû��
        
        //Statistics ��ص�ͳ�Ʋ���
        UInt32 fBytesSent;				//��ĿǰΪֹ�Ѿ����͵��ֽ���		//qtssCliSesRTPBytesSent ��ǰ�Ự�Ѿ����͵�RTP�ֽ�����
        UInt32 fPacketsSent;			//qtssCliSesRTPPacketsSent ��ǰ�Ự�Ѿ����͵�RTP���ݰ�����Ŀ��
        Float32 fPacketLossPercent;
        SInt64 fTimeConnected;			//�Ѿ����ӵ�ʱ��
        UInt32 fTotalRTCPPacketsRecv;
        UInt32 fTotalRTCPBytesRecv;
        // Movie size & movie duration. It may not be so good to associate these
        // statistics with the movie, for a session MAY have multiple movies...
        // however, the 1 movie assumption is in too many subsystems at this point
        Float64     fMovieDuration;		//qtssCliSesMovieDurationInSecs ��ǰ�Ự�ĵ�Ӱʱ��������ơ����ģ��û�н����趨�����ֵΪ����
        UInt64      fMovieSizeInBytes;		//qtssCliSesMovieSizeInBytes ��Ӱ�ĳߴ磬���ֽڼơ����ģ��û�н����趨�����ֵΪ����
        UInt32      fMovieAverageBitRate;		//qtssCliSesMovieAverageBitRate ÿ���ӵ�ƽ��λ�ʣ��������ܹ���RTPλ���Ե�Ӱʱ���õ������ģ��û�н����趨�����ֵΪ����
        
        QTSS_CliSesTeardownReason fTeardownReason;		//qtssCliTeardownReason ���ӶϿ���ԭ��������ǿͻ���������ģ��������QTSS_Teardown�������������ӶϿ���ԭ��
        // So the streams can send sender reports
        UInt32      fUniqueID;
        
        RTCPSRPacket        fRTCPSRPacket;	//RTCP���ݰ�
        StrPtrLen           fSRBuffer;		//�����߱���
        
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
