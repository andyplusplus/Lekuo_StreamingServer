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

/*  File:       RTSPRequestInterface.h

    Contains:   Provides a simple API for modules to access request information and
                manipulate (and possibly send) the client response.

                Implements the RTSP Request dictionary for QTSS API.

  Ϊģ���ṩ�ṩ����������Ϣ�ļ�API, �������ͻ���Ӧ, ΪQTSS APIʵ��RTSP��ʾ�ֵ�.   
 
   QTSServerInterface - �����ڲ����ݵĴ洢������API�б�ʶΪQTSS_ServerObject��
   ��API�е�ÿһ��QTSS_ServerAttributes���ڻ�����������ʵ�֡�


	����ΪqtssRTSPRequestObjectType�Ķ�����һЩ�����ض�RTSP�����������ɡ�
	RTSP�������QTSS_RTSPRequestObject���������ֶ������͵�ʵ������������
	�����Ǵӷ��������յ��ͻ���������RTSP����Ϊ�����󷢳���Ӧ����ת����һ
	������������ڼ䡣һ��RTSP����������͵���һ��RTSP�Ự����
	��QTSS_RTSPSessionObject�����������Ϊ�ض����������Ǵ�һ���ض��������Ϸ����ġ�

	RTSP Filter��ɫ�Ǹ����⡣ÿ�����Ե�ֵ���ڿ��Դӷ���������QTSS_RTSPRequestObject
	��������н�ɫ���ǿ��õġ� ����RTSP Filter��ɫ���յ�QTSS_RTSPRequestObject����ʱ��
	ֻ��qtssRTSPReqFullRequest���Ա���ֵ��

*/


#ifndef __RTSPREQUESTINTERFACE_H__
#define __RTSPREQUESTINTERFACE_H__

//INCLUDES:
#include "QTSS.h"
#include "QTSSDictionary.h"

#include "StrPtrLen.h"
#include "RTSPSessionInterface.h"
#include "RTSPResponseStream.h"
#include "RTSPProtocol.h"
#include "QTSSMessages.h"
#include "QTSSUserProfile.h"


class RTSPRequestInterface : public QTSSDictionary
{
    public:

        //Initialize
        //Call initialize before instantiating this class. For maximum performance, this class builds
        //any response header it can at startup time.
		//"	sPremadeHeaderPtr��ʼ��Ϊ"RTSP/1.0 200 OK..Server: DSS/5.5.1 (Build/489.8 Platform/Win32; Release/Darwin; )..Cseq:"
		//"	sPremadeNoHeaderPtr��ʼ��Ϊ"OK..Server: DSS/5.5.1 (Build/489.8 Platform/Win32; Release/Darwin; )..Cseq:"
		//"	��ʼ��kRTSPRequestDictIndex�ֵ����, �Լ���ʼ��kRTSPHeaderDictIndex�ֵ����, ��Ҫ�������ֵ�������Щ�ֶ�
        static void         Initialize();
        
        //CONSTRUCTOR:
		//���ֵ����ͷ������ı���֮�佨��һһ��Ӧ��ϵ
        RTSPRequestInterface(RTSPSessionInterface *session);
        virtual ~RTSPRequestInterface()
            { if (fMovieFolderPtr != &fMovieFolderPath[0]) delete [] fMovieFolderPtr; }
        
        //FUNCTIONS FOR SENDING OUTPUT:
        
        //Adds a new header to this object's list of headers to be sent out.
        //Note that this is only needed for "special purpose" headers. The Server,
        //CSeq, SessionID, and Connection headers are taken care of automatically
		//���������;��ͷ, ��ʽ XXX:value
        void    AppendHeader(QTSS_RTSPHeader inHeader, StrPtrLen* inValue);

        
        // The transport header constructed by this function mimics the one sent
        // by the client, with the addition of server port & interleaved sub headers
        void    AppendTransportHeader(StrPtrLen* serverPortA,
                                        StrPtrLen* serverPortB,
                                        StrPtrLen* channelA,
                                        StrPtrLen* channelB,
                                        StrPtrLen* serverIPAddr = NULL,
                                        StrPtrLen* ssrc = NULL);
        void    AppendContentBaseHeader(StrPtrLen* theURL);
        void    AppendRTPInfoHeader(QTSS_RTSPHeader inHeader,
                                    StrPtrLen* url, StrPtrLen* seqNumber,
                                    StrPtrLen* ssrc, StrPtrLen* rtpTime, Bool16 lastRTPInfo);

        void    AppendContentLength(UInt32 contentLength);
        void    AppendDateAndExpires();
        void    AppendSessionHeaderWithTimeout( StrPtrLen* inSessionID, StrPtrLen* inTimeout );
        void    AppendRetransmitHeader(UInt32 inAckTimeout);

        // MODIFIERS
        
        void SetKeepAlive(Bool16 newVal)                { fResponseKeepAlive = newVal; }
        
        //SendHeader:
        //Sends the RTSP headers, in their current state, to the client.
        void SendHeader();
        
        // QTSS STREAM FUNCTIONS
        
        // THE FIRST ENTRY OF THE IOVEC MUST BE BLANK!!!
        virtual QTSS_Error WriteV(iovec* inVec, UInt32 inNumVectors, UInt32 inTotalLength, UInt32* outLenWritten);
        
        //Write A "buffered send" that can be used for sending small chunks of data at a time.
        virtual QTSS_Error Write(void* inBuffer, UInt32 inLength, UInt32* outLenWritten, UInt32 inFlags);
        
        // Flushes all currently buffered data to the network. This either returns
        // QTSS_NoErr or EWOULDBLOCK. If it returns EWOULDBLOCK, you should wait for
        // a EV_WR on the socket, and call flush again.
        virtual QTSS_Error  Flush() { return fOutputStream->Flush(); }

        // Reads data off the stream. Same behavior as calling RTSPSessionInterface::Read
        virtual QTSS_Error Read(void* ioBuffer, UInt32 inLength, UInt32* outLenRead)
            { return fSession->Read(ioBuffer, inLength, outLenRead); }
            
        // Requests an event. Same behavior as calling RTSPSessionInterface::RequestEvent
        virtual QTSS_Error RequestEvent(QTSS_EventType inEventMask)
            { return fSession->RequestEvent(inEventMask); }
        
        
        //ACCESS FUNCTIONS:
        
        // These functions are shortcuts that objects internal to the server
        // use to get access to RTSP request information. Pretty much all
        // of this stuff is also available as QTSS API attributes.
        
        QTSS_RTSPMethod             GetMethod() const       { return fMethod; }
        QTSS_RTSPStatusCode         GetStatus() const       { return fStatus; }
        Bool16                      GetResponseKeepAlive() const { return fResponseKeepAlive; }
        void                        SetResponseKeepAlive(Bool16 keepAlive)  { fResponseKeepAlive = keepAlive; }
        
        //will be -1 unless there was a Range header. May have one or two values
        Float64                     GetStartTime()      { return fStartTime; }
        Float64                     GetStopTime()       { return fStopTime; }
        
        //
        // Value of Speed: header in request
        Float32                     GetSpeed()          { return fSpeed; }
        
        //
        // Value of late-tolerance field of x-RTP-Options header
        Float32                     GetLateToleranceInSec(){ return fLateTolerance; }
        StrPtrLen*                  GetLateToleranceStr(){ return &fLateToleranceStr; }
        
        // these get set if there is a transport header
        UInt16                      GetClientPortA()    { return fClientPortA; }
        UInt16                      GetClientPortB()    { return fClientPortB; }
        UInt32                      GetDestAddr()       { return fDestinationAddr; }
        UInt32                      GetSourceAddr()     { return fSourceAddr; }
        UInt16                      GetTtl()            { return fTtl; }
        QTSS_RTPTransportType       GetTransportType()  { return fTransportType; }
        QTSS_RTPNetworkMode         GetNetworkMode()    { return fNetworkMode; }
        UInt32                      GetWindowSize()     { return fWindowSize; }
        
            
        Bool16                      HasResponseBeenSent()
                                        { return fOutputStream->GetBytesWritten() > 0; }
            
        RTSPSessionInterface*       GetSession()         { return fSession; }
        QTSSDictionary*             GetHeaderDictionary(){ return &fHeaderDictionary; }
        
        Bool16                      GetAllowed()                { return fAllowed; }
        void                        SetAllowed(Bool16 allowed)  { fAllowed = allowed;}
        
        QTSS_ActionFlags            GetAction()             { return fAction; }
        void                        SetAction(QTSS_ActionFlags action)  { fAction = action;}

		Bool16						IsPushRequest()				{ return (fTransportMode == qtssRTPTransportModeRecord) ? true : false; }
        UInt16                      GetSetUpServerPort()        { return fSetUpServerPort;}
        QTSS_RTPTransportMode       GetTransportMode()          { return fTransportMode; }
        
        QTSS_AuthScheme             GetAuthScheme()             {  return fAuthScheme; }
        void                        SetAuthScheme(QTSS_AuthScheme scheme)   { fAuthScheme = scheme;}
        StrPtrLen*                  GetAuthRealm()              { return &fAuthRealm; }
        StrPtrLen*                  GetAuthNonce()              { return &fAuthNonce; }
        StrPtrLen*                  GetAuthUri()                { return &fAuthUri; }
        UInt32                      GetAuthQop()                { return fAuthQop; }
        StrPtrLen*                  GetAuthNonceCount()         { return &fAuthNonceCount; }
        StrPtrLen*                  GetAuthCNonce()             { return &fAuthCNonce; }
        StrPtrLen*                  GetAuthResponse()           { return &fAuthResponse; }                          
        StrPtrLen*                  GetAuthOpaque()             { return &fAuthOpaque; }
        QTSSUserProfile*            GetUserProfile()            { return fUserProfilePtr; }
        
        Bool16                      GetStale()                  { return fStale; }
        void                        SetStale(Bool16 stale)      { fStale = stale; }
        
        Bool16                      SkipAuthorization()         {  return fSkipAuthorization; }

		SInt32                      GetDynamicRateState()       { return fEnableDynamicRateState; }
        
		// DJM PROTOTYPE
		UInt32						GetRandomDataSize()			{ return fRandomDataSize; }
        
    protected:

        //ALL THIS STUFF HERE IS SETUP BY RTSPREQUEST object (derived)
        
        //REQUEST HEADER DATA
        enum
        {
            kMovieFolderBufSizeInBytes = 256,   //Uint32
            kMaxFilePathSizeInBytes = 256       //Uint32
        };
        
        QTSS_RTSPMethod             fMethod;            //Method of this request������ķ���  		//qtssRTSPReqMethodStr ��ǰ�����RTSP������
        QTSS_RTSPStatusCode         fStatus;            //qtssRTSPReqStatusCode RTSP����ĵ�ǰ״̬�룬������QTSS_RTSPStatusCode��
														//		ȱʡ����£���ֵ��qtssSuccessOK��
														//		���ģ���������Խ������ã�������QTSS_SendRTSPHeaders������������������ı�ͷ�е�״̬�����������Ե�ֵ��

        
		//qtssRTSPReqRealStatusCode ��	qtssRTSPReqStatusCode������ͬ��ֻ�ǽ�QTSS_RTSPStatusCode���͵�ֵ����Ϊʵ�ʵ�RTSP״̬�롣		
		UInt32                      fRealStatusCode;    //Current RTSP status num of this request
        Bool16                      fRequestKeepAlive;  //Does the client want keep-alive?
        Bool16                      fResponseKeepAlive; //Are we going to keep-alive?����
														//  qtssRTSPReqRespKeepAlive �����ϣ����������������ɺ���Ȼ�������Ӵ򿪣��������������Ϊ�档�����ϣ����������ɺ���ֹ���ӣ��������������Ϊ�١�
        RTSPProtocol::RTSPVersion   fVersion;			// ...

        Float64                     fStartTime;         //Range header info: start time  
														//qtssRTSPReqStartTime PLAY�����Range����Χ����ͷ��ָ������ʼʱ�䡣
        Float64                     fStopTime;          //Range header info: stop time
														//qtssRTSPReqStopTime PLAY�����Range��ͷ��ָ����ֹͣʱ�䡣

        UInt16                      fClientPortA;       //This is all info that comes out
        UInt16                      fClientPortB;       //of the Transport: header
        UInt16                      fTtl;				// ...
        UInt32                      fDestinationAddr;   // ....
        UInt32                      fSourceAddr;        // ... 
        QTSS_RTPTransportType       fTransportType;		//qtssRTSPReqTransportType
        QTSS_RTPNetworkMode         fNetworkMode;		// ... 
				//    qtssRTPNetworkModeDefault       = 0, // not declared
				//    qtssRTPNetworkModeMulticast     = 1,
				//    qtssRTPNetworkModeUnicast       = 2

		
        UInt32                      fContentLength;			// qtssRTSPReqContentLen ���յ���RTSP����������ݳ��ȡ�
        SInt64                      fIfModSinceDate;		// qtssRTSPReqIfModSinceDate ���RTSP�����а���һ��If-Modified-Since��ͷ����������Ծ���if-modified�����ڣ�ֻ��ת��ΪQTSS_TimeVal���͵�ֵ��
        Float32                     fSpeed;					// qtssRTSPReqSpeed Speed���ٶȣ���ͷ��ֵ��
        Float32                     fLateTolerance;			// qtssRTSPReqLateTolerance x-RTP-Options��ͷ��late-tolerance���ֵ�������ֵ�����ڣ����������ֵΪ-1��
        StrPtrLen                   fLateToleranceStr;		// ??
        Float32                     fPrebufferAmt;			// The maxtime field of the x-Prebuffer RTSP header
        
        StrPtrLen                   fFirstTransport;		// ...
        
        QTSS_StreamRef              fStreamRef;		//qtssRTSPReqStreamRef �������ָ��һ��QTSS_StreamRef���͵�ֵ�����ڽ����ݷ��͸�RTSP�ͻ���
										//�����������QTSS_RTSPSessionObject���������ṩ������ͬ������ӦQTSS_Write��QTSS_WriteV����ʱ�Ӳ�����QTSS_WouldBlock��
        
        //
        // For reliable UDP
        UInt32                      fWindowSize;			
        StrPtrLen                   fWindowSizeStr;			

        //Because of URL decoding issues, we need to make a copy of the file path. Here is a buffer for it.
        char                        fFilePath[kMaxFilePathSizeInBytes];				//qtssRTSPReqFilePath ��ǰ�����URI���ѱ�ת��Ϊ�����ļ�ϵͳ·����
        char                        fMovieFolderPath[kMovieFolderBufSizeInBytes];
        char*                       fMovieFolderPtr;		//qtssRTSPReqRootDir ��ǰ����ĸ�Ŀ¼��������Ե�ȱʡֵΪ��������ý���ļ���·����
															//    ģ�������RTSP Route��ɫ�ж�������Խ����趨��
        
        QTSSDictionary              fHeaderDictionary;		// ??
        
        Bool16                      fAllowed;			//qtssRTSPReqUserAllowed
        QTSS_RTPTransportMode       fTransportMode;		//qtssRTSPReqTransportMode
        UInt16                      fSetUpServerPort;   //qtssRTSPReqSetUpServerPort ��send this back as the server_port if is SETUP request
    
        QTSS_ActionFlags            fAction;    //qtssRTSPReqAction , The action that will be performed for this request
                                                //                    Set to a combination of QTSS_ActionFlags 
        
        QTSS_AuthScheme             fAuthScheme;		//qtssRTSPReqAuthScheme,     qtssAuthNone        = 0,    qtssAuthBasic       = 1,    qtssAuthDigest      = 2
        StrPtrLen                   fAuthRealm;
        StrPtrLen                   fAuthNonce;
        StrPtrLen                   fAuthUri;
        UInt32                      fAuthQop;
        StrPtrLen                   fAuthNonceCount;
        StrPtrLen                   fAuthCNonce;
        StrPtrLen                   fAuthResponse;                          
        StrPtrLen                   fAuthOpaque;

		//Object's username is filled in by the server and its password and group memberships filled in by the authentication module.
        QTSSUserProfile             fUserProfile;
        QTSSUserProfile*            fUserProfilePtr;	//qtssRTSPReqUserProfile  
        Bool16                      fStale;
        
        Bool16                      fSkipAuthorization; //qtssRTSPReqSkipAuthorization ���ģ��ϣ����ǰ����ͨ��������֤ģ�����֤����Ӧ������������ԡ�

		SInt32                      fEnableDynamicRateState;	//qtssRTSPReqDynamicRateState
        
		// DJM PROTOTYPE
		UInt32						fRandomDataSize;		//??
        
    private:

        RTSPSessionInterface*   fSession;
        RTSPResponseStream*     fOutputStream;		//�����
        

        enum
        {
            kStaticHeaderSizeInBytes = 512  //UInt32
        };
        
        Bool16                  fStandardHeadersWritten;
        
        void                    PutTransportStripped(StrPtrLen &outFirstTransport, StrPtrLen &outResultStr);
        void                    WriteStandardHeaders();
        static void             PutStatusLine(  StringFormatter* putStream,
                                                QTSS_RTSPStatusCode status,
                                                RTSPProtocol::RTSPVersion version);

        //Individual param retrieval functions
        static void*        GetAbsTruncatedPath(QTSSDictionary* inRequest, UInt32* outLen);
        static void*        GetTruncatedPath(QTSSDictionary* inRequest, UInt32* outLen);
        static void*        GetFileName(QTSSDictionary* inRequest, UInt32* outLen);
        static void*        GetFileDigit(QTSSDictionary* inRequest, UInt32* outLen);
        static void*        GetRealStatusCode(QTSSDictionary* inRequest, UInt32* outLen);
		static void*		GetLocalPath(QTSSDictionary* inRequest, UInt32* outLen);

        //optimized preformatted response header strings
        static char             sPremadeHeader[kStaticHeaderSizeInBytes];
        static StrPtrLen        sPremadeHeaderPtr;		//��RTSPRequestInterface::Initialize()�г�ʼ��, ��ʼֵΪ
				//RTSP/1.0 200 OK "\r\n" Server: DSS/5.5.1 (Build/489.8; Platform/Win32; Release/Darwin; ) "\r\n"  Cseq: 
        
        static char             sPremadeNoHeader[kStaticHeaderSizeInBytes];
        static StrPtrLen        sPremadeNoHeaderPtr;//��RTSPRequestInterface::Initialize()�г�ʼ��, ��ʼֵΪ
				//             OK "\r\n" Server: DSS/5.5.1 (Build/489.8; Platform/Win32; Release/Darwin; ) "\r\n"  Cseq: 

        
        static StrPtrLen        sColonSpace;
        
        //Dictionary support
        static QTSSAttrInfoDict::AttrInfo   sAttributes[];
};
#endif // __RTSPREQUESTINTERFACE_H__

