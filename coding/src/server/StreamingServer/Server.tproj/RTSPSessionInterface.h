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

    Contains:   �ṩ��ģ��ʹ�õĻỰ����ԴAPI��ʵ����QTSS APIʹ�õ�RTSP Session dictionary
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
    static void     Initialize();		//��ʼ���ֵ����sOptionsRequestBody�ֶ�
    static void     SetBase64Decoding(Bool16 newVal) { sDoBase64Decoding = newVal; }
    
    RTSPSessionInterface();
    virtual ~RTSPSessionInterface();
    
    //Is this session alive? If this returns false, clean up and begone as fast as possible
    Bool16 IsLiveSession()      { return fSocket.IsConnected() && fLiveSession; }
    
    // Allows clients to refresh the timeout
    void RefreshTimeout()       { fTimeoutTask.RefreshTimeout(); }

	// Ϊ�˱��ڷ�����RTSP�����Ϸ��ʹ������ݣ�����������Ҫֱ�ӷ��ʸö���ָ�롣�����ɸö�����task object����������κ�ʱ��ɾ��
	// �����object holders��>0��RTSPSession����ɾ����Ȼ������session��objectӦaware���IsLiveSession����false����Ӧ������session�Ŀ���
    void IncrementObjectHolderCount() { (void)atomic_add(&fObjectHolders, 1); }
    void DecrementObjectHolderCount();
    
    // If RTP data is interleaved into the RTSP connection, we need to associate
    // 2 unique channel numbers with each RTP stream, one for RTP and one for RTCP.
    // �ú�����������ͨ���ţ�����һ��С��ͨ���ţ���һ��ͨ���Ŵ�
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
    
    RTSPRequestStream   fInputStream;		//���ڴӿͻ���ȡ����
    RTSPResponseStream  fOutputStream;		//������ͻ�д����
    
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
    TCPSocket*          fOutputSocketP;	//qtssRTSPSesEventCntxt RTCP���ӵ��¼������ġ��ڷ������Կͻ��˽�����Ӧʱ�����������Ҫ���ڵȴ��������Ƶ�EV_WR�¼���
    TCPSocket*          fInputSocketP;  // ����socket <-- usually same as fSocketP, unless we're HTTP Proxying
    
    void        SnarfInputSocket( RTSPSessionInterface* fromRTSPSession );
    
    // What session type are we?
    QTSS_RTSPSessionType    fSessionType;		//qtssRTSPSesType RTSP�Ự���͡����ܵ�ֵ��qtssRTSPSession��qtssRTSPHTTPSession������HTTPͨ����RTSP�Ự�����Լ�qtssRTSPHTTPInputSession������Ϊ		//qtssRTSPHTTPInputSession�ĻỰ����������ͨ���ǳ��̡������Ự���ͣ�����qtssRTSPSession��qtssRTSPHTTPSession, qtssRTSPHTTPInputSession
    Bool16              fLiveSession;			//�Ƿ���fLiveSession
    unsigned int        fObjectHolders;			//�����ӵ������
    UInt8               fCurChannelNum;			//
    StrPtrLen*          fChNumToSessIDMap;  
    
    QTSS_StreamRef      fStreamRef;				//qtssRTSPSesStreamRef һ��QTSS_StreamRef���͵�ֵ�����ڽ����ݷ��͵�RTSP�ͻ��ˡ�

    UInt32              fSessionID;				//qtssRTSPSesID ���������һ��ID��Ψһ��ʶ�Է���������������ÿ��RTSP�Ự��//�ûỰ��Ӧ�ĻỰID
    UInt32              fLocalAddr;				//qtssRTSPSesLocalAddr ��ǰRTSP�Ự�ı���IP��ַ��
    UInt32              fRemoteAddr;			//qtssRTSPSesRemoteAddr �ͻ��˵�IP��ַ��
    SInt32              fRequestBodyLen;		//ǿ�������峤��
    
    UInt16              fLocalPort;				//qtssRTSPSesLocalPort RTSP���ӵı��ض˿ڡ�
    UInt16              fRemotePort;			//qtssRTSPSesRemotePort Remote RTSP���ӵģ��ͻ��ˣ��˿ڡ�
    
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
