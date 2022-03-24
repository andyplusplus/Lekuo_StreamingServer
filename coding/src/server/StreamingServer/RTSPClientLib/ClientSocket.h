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
    File:       ClientSocket.h
*/


#ifndef __CLIENT_SOCKET__
#define __CLIENT_SOCKET__

#include "OSHeaders.h"
#include "TCPSocket.h"

class ClientSocket
{
    public:
    
        ClientSocket();
        virtual ~ClientSocket() {}
        
        void    Set(UInt32 hostAddr, UInt16 hostPort)
            { fHostAddr = hostAddr; fHostPort = hostPort; }
            
        //
        // Sends data to the server. If this returns EAGAIN or EINPROGRESS, call again
        // until it returns OS_NoErr or another error. On subsequent calls, you need not
        // provide a buffer.
        //
        // When this call returns EAGAIN or EINPROGRESS, caller should use GetEventMask
        // and GetSocket to wait for a socket event.
        OS_Error    Send(char* inData, const UInt32 inLength);

        //
        // Sends an ioVec to the server. Same conditions apply as above function 
        virtual OS_Error    SendV(iovec* inVec, UInt32 inNumVecs) = 0;
        
        //
        // Reads data from the server. If this returns EAGAIN or EINPROGRESS, call
        // again until it returns OS_NoErr or another error. This call may return OS_NoErr
        // and 0 for rcvLen, in which case you should call it again.
        //
        // When this call returns EAGAIN or EINPROGRESS, caller should use GetEventMask
        // and GetSocket to wait for a socket event.
        virtual OS_Error    Read(void* inBuffer, const UInt32 inLength, UInt32* outRcvLen) = 0;
        
        //
        // ACCESSORS
        UInt32          GetHostAddr()           { return fHostAddr; }
        virtual UInt32  GetLocalAddr() = 0;

        // If one of the above methods returns EWOULDBLOCK or EINPROGRESS, you
        // can check this to see what events you should wait for on the socket
        UInt32      GetEventMask()          { return fEventMask; }
        Socket*     GetSocket()             { return fSocketP; }
        
        virtual void    SetRcvSockBufSize(UInt32 inSize) = 0;

    protected:
    
        // Generic connect function
        OS_Error    Connect(TCPSocket* inSocket);   
        // Generic open function
        OS_Error    Open(TCPSocket* inSocket);
        
        OS_Error    SendSendBuffer(TCPSocket* inSocket);
    
        UInt32      fHostAddr;		//远程地址和端口
        UInt16      fHostPort;		//  ...
        
        UInt32      fEventMask;		//fEventMask = EV_RE | EV_WR;
        Socket*     fSocketP;		//本地的TCPSocket, 用于连接远程地址和端口

        enum
        {
            kSendBufferLen = 2048
        };
        
        // Buffer for sends.
        char        fSendBuf[kSendBufferLen + 1];	//该变量和fSendBuffer是一一对应关系
        StrPtrLen   fSendBuffer;	//发送数据的缓冲区
        UInt32      fSentLength;	//已经发送的长度
};

class TCPClientSocket : public ClientSocket
{
    public:
        
        TCPClientSocket(UInt32 inSocketType);
        virtual ~TCPClientSocket() {}
        
        //
        // Implements the ClientSocket Send and Receive interface for a TCP connection
        virtual OS_Error    SendV(iovec* inVec, UInt32 inNumVecs);	//该函数先把iovec中的数据copy到fSendBuffer中,然后通过SendSendBuffer发数据发出去
        virtual OS_Error    Read(void* inBuffer, const UInt32 inLength, UInt32* outRcvLen);

        virtual UInt32  GetLocalAddr() { return fSocket.GetLocalAddr(); }
        virtual void    SetRcvSockBufSize(UInt32 inSize) { fSocket.SetSocketRcvBufSize(inSize); }
        virtual void    SetOptions(int sndBufSize = 8192,int rcvBufSize=1024);		//设置socket的发送缓存区以及接收缓存区的大小
        
        virtual UInt16  GetLocalPort() { return fSocket.GetLocalPort(); }
        
    private:
    
        TCPSocket   fSocket;	//在构造该对象以前, 最好正确打开socket, 因为QTSSSplitterModule模块
								//在QTSS_CreateStreamFromSocket中使用该socket 文件描述符的
};

class HTTPClientSocket : public ClientSocket
{
    public:
        
        HTTPClientSocket(const StrPtrLen& inURL, UInt32 inCookie, UInt32 inSocketType);
        virtual ~HTTPClientSocket();
        
        //
        // Closes the POST half of the RTSP / HTTP connection
        void                ClosePost()     { delete fPostSocket; fPostSocket = NULL; }

        //
        // Implements the ClientSocket Send and Receive interface for an RTSP / HTTP connection
        virtual OS_Error    SendV(iovec* inVec, UInt32 inNumVecs);		//对发送的数据进行Base64编码, 并发送这部分数据
        virtual OS_Error    Read(void* inBuffer, const UInt32 inLength, UInt32* outRcvLen);		
				//调用该函数以前, 先把发送GET请求, fSendBuffer, 然后不断调用Read函数读取数据, 直到所有的数据都已经读出.
				//如果能读出所有数据, 则直接返回, 否则进入事件读取模式

        virtual UInt32  GetLocalAddr() { return fGetSocket.GetLocalAddr();  }
        virtual void    SetRcvSockBufSize(UInt32 inSize) { fGetSocket.SetSocketRcvBufSize(inSize); }

    private:
    
        void        EncodeVec(iovec* inVec, UInt32 inNumVecs);		//对发送的数据进行Base64编码

        StrPtrLen   fURL;		//URL的地址
        UInt32      fCookie;	//Cookie对象
        
        UInt32      fSocketType;	//Socket类型
        UInt32      fGetReceived;	//
        TCPSocket   fGetSocket;		//读取数据的socket
        TCPSocket*  fPostSocket;	//发送数据的socket
};

#endif //__CLIENT_SOCKET__
