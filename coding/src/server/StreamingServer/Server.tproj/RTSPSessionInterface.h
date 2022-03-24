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
    File:       RTSPSessionInterface.h

    Contains:   提供了模块使用的会话级资源API，实现了QTSS API使用的RTSP Session dictionary
*/

#ifndef __RTSPSESSIONINTERFACE_H__
#define __RTSPSESSIONINTERFACE_H__

#include "RTSPRequestStream.h"
#include "RTSPResponseStream.h"
#include "Task.h"
#include "QTSS.h"
#include "QTSSDictionary.h"
#include "atomic.h"

class RTSPSessionInterface : public QTSSDictionary, public Task
{
public:

    //Initialize must be called right off the bat to initialize dictionary resources
    static void     Initialize();		//初始化字典对象及sOptionsRequestBody字段
    static void     SetBase64Decoding(Bool16 newVal) { sDoBase64Decoding = newVal; }
    
    RTSPSessionInterface();
    virtual ~RTSPSessionInterface();
    
    //Is this session alive? If this returns false, clean up and begone as fast as possible
    Bool16 IsLiveSession()      { return fSocket.IsConnected() && fLiveSession; }
    
    // Allows clients to refresh the timeout
    void RefreshTimeout()       { fTimeoutTask.RefreshTimeout(); }

	// 为了便于发送在RTSP连接上发送带外数据，其它对象需要直接访问该对象指针。但因由该对象是task object，对象会在任何时候删除
	// 但如果object holders数>0，RTSPSession不被删除。然而管理session的object应aware如果IsLiveSession返回false，则应放弃对session的控制
    void IncrementObjectHolderCount() { (void)atomic_add(&fObjectHolders, 1); }
    void DecrementObjectHolderCount();
    
    // If RTP data is interleaved into the RTSP connection, we need to associate
    // 2 unique channel numbers with each RTP stream, one for RTP and one for RTCP.
    // 该函数分配两个通道号，返回一个小的通道号，另一个通道号大１
    //
    // Pass in the RTSP Session ID of the Client session to which these channel numbers will belong.
    UInt8               GetTwoChannelNumbers(StrPtrLen* inRTSPSessionID);

    //
    // Given a channel number, returns the RTSP Session ID to which this channel number refers
    StrPtrLen*  GetSessionIDForChannelNum(UInt8 inChannelNum);
    
    //Two main things are persistent through the course of a session, not associated with any one request. 
	//The RequestStream (which can be used for getting data from the client), and the socket. OOps, and the ResponseStream
    RTSPRequestStream*  GetInputStream()    { return &fInputStream; }
    RTSPResponseStream* GetOutputStream()   { return &fOutputStream; }
    TCPSocket*          GetSocket()         { return &fSocket; }
    OSMutex*            GetSessionMutex()   { return &fSessionMutex; }
    
    UInt32              GetSessionID()      { return fSessionID; }
    
    // Request Body Length
    // This object can enforce a length of the request body to prevent callers
    // of Read() from overrunning the request body and going into the next request.
    // -1 is an unknown request body length. If the body length is unknown, this object will do no length enforcement. 
    void                SetRequestBodyLength(SInt32 inLength)   { fRequestBodyLen = inLength; }  //??
    SInt32              GetRemainingReqBodyLen()                { return fRequestBodyLen; }
    
    // QTSS STREAM FUNCTIONS
    
    // Allows non-buffered writes to the client. These will flow control.
    
    // THE FIRST ENTRY OF THE IOVEC MUST BE BLANK!!!
    virtual QTSS_Error WriteV(iovec* inVec, UInt32 inNumVectors, UInt32 inTotalLength, UInt32* outLenWritten);
    virtual QTSS_Error Write(void* inBuffer, UInt32 inLength, UInt32* outLenWritten, UInt32 inFlags);
    virtual QTSS_Error Read(void* ioBuffer, UInt32 inLength, UInt32* outLenRead);
    virtual QTSS_Error RequestEvent(QTSS_EventType inEventMask);

    // performs RTP over RTSP
    QTSS_Error  InterleavedWrite(void* inBuffer, UInt32 inLen, UInt32* outLenWritten, unsigned char channel);

	// OPTIONS request
	void		SaveOutputStream();
	void		RevertOutputStream();
	void		ResetOutputStream() { fOutputStream.Reset(); fOutputStream.ResetBytesWritten();}
	void		SendOptionsRequest();
	Bool16		SentOptionsRequest() { return fSentOptionsRequest; }
	SInt32		RoundTripTime() { return fRoundTripTime; }
	
    enum
    {
        kMaxUserNameLen         = 32,
        kMaxUserPasswordLen     = 32,
        kMaxUserRealmLen        = 64
    };

    enum                        // Quality of protection
    {
        kNoQop          = 0,    // No Quality of protection
        kAuthQop        = 1,    // Authentication
        kAuthIntQop     = 2     // Authentication with Integrity        
    };
        
	// DJM PROTOTYPE
	enum
    {
        kMaxRandomDataSize		= 256 * 1024,
    };
        
protected:
    enum
    {
        kFirstRTSPSessionID     = 1,    //UInt32
    };

    //Each RTSP session has a unique number that identifies it.

    char                fUserNameBuf[kMaxUserNameLen];
    char                fUserPasswordBuf[kMaxUserPasswordLen];
    char                fUserRealmBuf[kMaxUserRealmLen];

    TimeoutTask         fTimeoutTask;		//allows the session to be timed out
    
    RTSPRequestStream   fInputStream;		//用于从客户读取数据
    RTSPResponseStream  fOutputStream;		//用于向客户写数据
    
    // Any RTP session sending interleaved data on this RTSP session must 
    // be prevented from writing while an RTSP request is in progress
    OSMutex             fSessionMutex;
    
    // for coalescing small interleaved writes into a single TCP frame
    enum
    {
          kTCPCoalesceBufferSize = 1450 //1450 is the max data space in an TCP segment over ent
        , kTCPCoalesceDirectWriteSize = 0 // if > this # bytes bypass coalescing and make a direct write
        , kInteleaveHeaderSize = 4  // '$ '+ 1 byte ch ID + 2 bytes length
    };
    char*       fTCPCoalesceBuffer;
    SInt32      fNumInCoalesceBuffer;


    //+rt  socket we get from "accept()"
    TCPSocket           fSocket;		//
    TCPSocket*          fOutputSocketP;	//qtssRTSPSesEventCntxt RTCP连接的事件上下文。在服务器对客户端进行响应时，这个属性主要用于等待流量控制的EV_WR事件。
    TCPSocket*          fInputSocketP;  // 接收socket <-- usually same as fSocketP, unless we're HTTP Proxying
    
    void        SnarfInputSocket( RTSPSessionInterface* fromRTSPSession );
    
    // What session type are we?
    QTSS_RTSPSessionType    fSessionType;		//qtssRTSPSesType RTSP会话类型。可能的值有qtssRTSPSession，qtssRTSPHTTPSession（基于HTTP通道的RTSP会话），以及qtssRTSPHTTPInputSession。类型为		//qtssRTSPHTTPInputSession的会话的生命周期通常非常短。、、会话类型，包括qtssRTSPSession，qtssRTSPHTTPSession, qtssRTSPHTTPInputSession
    Bool16              fLiveSession;			//是否是fLiveSession
    unsigned int        fObjectHolders;			//对象的拥有者数
    UInt8               fCurChannelNum;			//
    StrPtrLen*          fChNumToSessIDMap;  
    
    QTSS_StreamRef      fStreamRef;				//qtssRTSPSesStreamRef 一个QTSS_StreamRef类型的值，用于将数据发送到RTSP客户端。

    UInt32              fSessionID;				//qtssRTSPSesID 这个属性是一个ID，唯一标识自服务器启动以来的每个RTSP会话。//该会话对应的会话ID
    UInt32              fLocalAddr;				//qtssRTSPSesLocalAddr 当前RTSP会话的本地IP地址。
    UInt32              fRemoteAddr;			//qtssRTSPSesRemoteAddr 客户端的IP地址。
    SInt32              fRequestBodyLen;		//强制请求体长度
    
    UInt16              fLocalPort;				//qtssRTSPSesLocalPort RTSP连接的本地端口。
    UInt16              fRemotePort;			//qtssRTSPSesRemotePort Remote RTSP连接的（客户端）端口。
    
	// For OPTIONS request
	StrPtrLen				fOldOutputStreamBuffer;
	Bool16					fSentOptionsRequest;
	SInt64					fOptionsRequestSendTime;
	SInt32					fRoundTripTime;
	Bool16					fRoundTripTimeCalculation;
		
    static unsigned int sSessionIDCounter;
    static Bool16           sDoBase64Decoding;
    
	static 	UInt32			sOptionsRequestBody[kMaxRandomDataSize / sizeof(UInt32)];
	
    //Dictionary support
    
    // Param retrieval function
    static void*        SetupParams(QTSSDictionary* inSession, UInt32* outLen);
    
    static QTSSAttrInfoDict::AttrInfo   sAttributes[];
};
#endif // __RTSPSESSIONINTERFACE_H__

