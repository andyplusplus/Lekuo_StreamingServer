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
    File:       RTSPRequestStream.h

    Contains:   为RTSP提供一个流抽象。给定一个socket，该对象可读入数据，直到读入整个RTSP请求（）
				Provides a stream abstraction for RTSP. Given a socket, this object
                can read in data until an entire RTSP request header is available.
                (do this by calling ReadRequest). It handles RTSP pipelining (request
                headers are produced serially even if multiple headers arrive simultaneously),
                & RTSP request data.
                    
*/

#ifndef __RTSPREQUESTSTREAM_H__
#define __RTSPREQUESTSTREAM_H__


//INCLUDES
#include "StrPtrLen.h"
#include "TCPSocket.h"
#include "QTSS.h"

class RTSPRequestStream
{
public:

    //CONSTRUCTOR / DESTRUCTOR
    RTSPRequestStream(TCPSocket* sock);
    
    // We may have to delete this memory if it was allocated due to base64 decoding
    ~RTSPRequestStream() { if (fRequest.Ptr != &fRequestBuffer[0]) delete [] fRequest.Ptr; }

    //ReadRequest
    //该函数不会阻塞: 尝试从流读数据，当hit EOL时停止－－EOL结束RTSP头
    //
    //Returns:          QTSS_NoErr:     Out of data, haven't hit EOL - EOL yet
    //                  QTSS_RequestArrived: full request has arrived
    //                  E2BIG: ran out of buffer space
    //                  QTSS_RequestFailed: if the client has disconnected
    //                  EINVAL: if we are base64 decoding and the stream is corrupt
    //                  QTSS_OutOfState: 
    QTSS_Error      ReadRequest();
    
    // Read
    //
    // 该函数从流读取数据，并把数据存入提供的缓存
    // Returns: QTSS_NoErr, EAGAIN 阻塞或其它socket错误
    QTSS_Error      Read(void* ioBuffer, UInt32 inBufLen, UInt32* outLengthRead);
    
    // 采用不同的TCPSocket读数据，由RTSPSessionInterface::SnarfInputSocket使用
    void                AttachToSocket(TCPSocket* sock) { fSocket = sock; }
    
    // 告诉请求流是否从base64解码
    void                IsBase64Encoded(Bool16 isDataEncoded) { fDecode = isDataEncoded; }
    
    //GetRequestBuffer
    //读取一个包含完整客户端请求的Buffer. 长度设置为请求的精确长度
	//This will return NULL UNLESS this object
    //is in the proper state (has been initialized, ReadRequest has been called until it returns
        //RequestArrived).
    StrPtrLen*  GetRequestBuffer()  { return fRequestPtr; }
    Bool16      IsDataPacket()      { return fIsDataPacket; }
    void        ShowRTSP(Bool16 enable) {fPrintRTSP = enable; }     
    void SnarfRetreat( RTSPRequestStream &fromRequest );
        
private:

        
    //CONSTANTS:
    enum
    {
        kRequestBufferSizeInBytes = 2048        //UInt32
    };
    
    // Base64 decodes into fRequest.Ptr, updates fRequest.Len, and returns the amount of data left undecoded in inSrcData
    QTSS_Error              DecodeIncomingData(char* inSrcData, UInt32 inSrcDataLen);

    TCPSocket*              fSocket;			//对应的socket
    UInt32                  fRetreatBytes;		//读取的字节数
    UInt32                  fRetreatBytesRead; // Used by Read() when it is reading RetreatBytes
    
    char                    fRequestBuffer[kRequestBufferSizeInBytes];	//请求缓存
    UInt32                  fCurOffset;				// 跟踪以上缓存有多少合法数据
    UInt32                  fEncodedBytesRemaining; // 如果我们正在decoding, 跟踪buffer有多少encoded字节
    
    StrPtrLen               fRequest;		//请求的完整内容
    StrPtrLen*              fRequestPtr;    // pointer to a request header
    Bool16                  fDecode;        // should we base 64 decode?
    Bool16                  fIsDataPacket;  // is this a data packet? Like for a record?
    Bool16                  fPrintRTSP;     // debugging printfs
    
};

#endif
