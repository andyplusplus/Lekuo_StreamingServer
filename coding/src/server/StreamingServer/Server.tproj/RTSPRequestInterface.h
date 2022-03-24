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

  为模块提供提供访问请求信息的简单API, 并操作客户回应, 为QTSS API实现RTSP请示字典.   
 
   QTSServerInterface - 这是内部数据的存储对象，在API中标识为QTSS_ServerObject。
   在API中的每一个QTSS_ServerAttributes都在基类中声明和实现。


	类型为qtssRTSPRequestObjectType的对象由一些描述特定RTSP请求的属性组成。
	RTSP请求对象（QTSS_RTSPRequestObject）就是这种对象类型的实例，它的生命
	周期是从服务器接收到客户端完整的RTSP请求到为该请求发出响应，并转向下一
	个请求的整个期间。一个RTSP请求对象必须和单独一个RTSP会话对象
	（QTSS_RTSPSessionObject）相关联，因为特定的请求总是从一个特定的连接上发出的。

	RTSP Filter角色是个例外。每个属性的值对于可以从服务器接收QTSS_RTSPRequestObject
	对象的所有角色都是可用的。 而当RTSP Filter角色接收到QTSS_RTSPRequestObject对象时，
	只有qtssRTSPReqFullRequest属性被赋值。

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
		//"	sPremadeHeaderPtr初始化为"RTSP/1.0 200 OK..Server: DSS/5.5.1 (Build/489.8 Platform/Win32; Release/Darwin; )..Cseq:"
		//"	sPremadeNoHeaderPtr初始化为"OK..Server: DSS/5.5.1 (Build/489.8 Platform/Win32; Release/Darwin; )..Cseq:"
		//"	初始化kRTSPRequestDictIndex字典对象, 以及初始化kRTSPHeaderDictIndex字典对象, 主要是设置字典中有哪些字段
        static void         Initialize();
        
        //CONSTRUCTOR:
		//在字典对象和服务器的变量之间建立一一对应关系
        RTSPRequestInterface(RTSPSessionInterface *session);
        virtual ~RTSPRequestInterface()
            { if (fMovieFolderPtr != &fMovieFolderPath[0]) delete [] fMovieFolderPtr; }
        
        //FUNCTIONS FOR SENDING OUTPUT:
        
        //Adds a new header to this object's list of headers to be sent out.
        //Note that this is only needed for "special purpose" headers. The Server,
        //CSeq, SessionID, and Connection headers are taken care of automatically
		//添加特殊用途的头, 格式 XXX:value
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
        
        QTSS_RTSPMethod             fMethod;            //Method of this request，请求的方法  		//qtssRTSPReqMethodStr 当前请求的RTSP方法。
        QTSS_RTSPStatusCode         fStatus;            //qtssRTSPReqStatusCode RTSP请求的当前状态码，类型是QTSS_RTSPStatusCode。
														//		缺省情况下，其值是qtssSuccessOK。
														//		如果模块对这个属性进行设置，并调用QTSS_SendRTSPHeaders函数，则服务器产生的报头中的状态码包含这个属性的值。

        
		//qtssRTSPReqRealStatusCode 和	qtssRTSPReqStatusCode属性相同，只是将QTSS_RTSPStatusCode类型的值翻译为实际的RTSP状态码。		
		UInt32                      fRealStatusCode;    //Current RTSP status num of this request
        Bool16                      fRequestKeepAlive;  //Does the client want keep-alive?
        Bool16                      fResponseKeepAlive; //Are we going to keep-alive?，　
														//  qtssRTSPReqRespKeepAlive 如果您希望服务器在请求完成后仍然保持连接打开，则将这个属性设置为真。如果您希望在请求完成后终止连接，则将这个属性设置为假。
        RTSPProtocol::RTSPVersion   fVersion;			// ...

        Float64                     fStartTime;         //Range header info: start time  
														//qtssRTSPReqStartTime PLAY请求的Range（范围）报头中指定的起始时间。
        Float64                     fStopTime;          //Range header info: stop time
														//qtssRTSPReqStopTime PLAY请求的Range报头中指定的停止时间。

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

		
        UInt32                      fContentLength;			// qtssRTSPReqContentLen 接收到的RTSP请求体的内容长度。
        SInt64                      fIfModSinceDate;		// qtssRTSPReqIfModSinceDate 如果RTSP请求中包含一个If-Modified-Since报头，则这个属性就是if-modified的日期，只是转换为QTSS_TimeVal类型的值。
        Float32                     fSpeed;					// qtssRTSPReqSpeed Speed（速度）报头的值。
        Float32                     fLateTolerance;			// qtssRTSPReqLateTolerance x-RTP-Options报头的late-tolerance域的值，如果该值不存在，则这个属性值为-1。
        StrPtrLen                   fLateToleranceStr;		// ??
        Float32                     fPrebufferAmt;			// The maxtime field of the x-Prebuffer RTSP header
        
        StrPtrLen                   fFirstTransport;		// ...
        
        QTSS_StreamRef              fStreamRef;		//qtssRTSPReqStreamRef 这个属性指向一个QTSS_StreamRef类型的值，用于将数据发送给RTSP客户。
										//这个流引用与QTSS_RTSPSessionObject对象属性提供的流不同，在响应QTSS_Write或QTSS_WriteV调用时从不返回QTSS_WouldBlock。
        
        //
        // For reliable UDP
        UInt32                      fWindowSize;			
        StrPtrLen                   fWindowSizeStr;			

        //Because of URL decoding issues, we need to make a copy of the file path. Here is a buffer for it.
        char                        fFilePath[kMaxFilePathSizeInBytes];				//qtssRTSPReqFilePath 当前请求的URI，已被转化为本地文件系统路径。
        char                        fMovieFolderPath[kMovieFolderBufSizeInBytes];
        char*                       fMovieFolderPtr;		//qtssRTSPReqRootDir 当前请求的根目录。这个属性的缺省值为服务器的媒体文件夹路径。
															//    模块可以在RTSP Route角色中对这个属性进行设定。
        
        QTSSDictionary              fHeaderDictionary;		// ??
        
        Bool16                      fAllowed;			//qtssRTSPReqUserAllowed
        QTSS_RTPTransportMode       fTransportMode;		//qtssRTSPReqTransportMode
        UInt16                      fSetUpServerPort;   //qtssRTSPReqSetUpServerPort ，send this back as the server_port if is SETUP request
    
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
        
        Bool16                      fSkipAuthorization; //qtssRTSPReqSkipAuthorization 如果模块希望当前请求通过所有认证模块的认证，则应该设置这个属性。

		SInt32                      fEnableDynamicRateState;	//qtssRTSPReqDynamicRateState
        
		// DJM PROTOTYPE
		UInt32						fRandomDataSize;		//??
        
    private:

        RTSPSessionInterface*   fSession;
        RTSPResponseStream*     fOutputStream;		//输出流
        

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
        static StrPtrLen        sPremadeHeaderPtr;		//在RTSPRequestInterface::Initialize()中初始化, 初始值为
				//RTSP/1.0 200 OK "\r\n" Server: DSS/5.5.1 (Build/489.8; Platform/Win32; Release/Darwin; ) "\r\n"  Cseq: 
        
        static char             sPremadeNoHeader[kStaticHeaderSizeInBytes];
        static StrPtrLen        sPremadeNoHeaderPtr;//在RTSPRequestInterface::Initialize()中初始化, 初始值为
				//             OK "\r\n" Server: DSS/5.5.1 (Build/489.8; Platform/Win32; Release/Darwin; ) "\r\n"  Cseq: 

        
        static StrPtrLen        sColonSpace;
        
        //Dictionary support
        static QTSSAttrInfoDict::AttrInfo   sAttributes[];
};
#endif // __RTSPREQUESTINTERFACE_H__

