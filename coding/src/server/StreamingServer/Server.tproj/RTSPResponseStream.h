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
    File:       RTSPResponseStream.h

    Contains:   Object that provides a "buffered WriteV" service. Clients
                can call this function to write to a socket, and buffer flow
                controlled data in different ways.
                
                It is derived from StringFormatter, which it uses as an output
                stream buffer. The buffer may grow infinitely.
*/

#ifndef __RTSP_RESPONSE_STREAM_H__
#define __RTSP_RESPONSE_STREAM_H__

#include "ResizeableStringFormatter.h"
#include "TCPSocket.h"
#include "TimeoutTask.h"
#include "QTSS.h"

class RTSPResponseStream : public ResizeableStringFormatter
{
    public:
    
        // 该对象提供了一些流控缓存机制，当数据成功写入socket以后，更新timeout
        RTSPResponseStream(TCPSocket* inSocket, TimeoutTask* inTimeoutTask)
            :   ResizeableStringFormatter(fOutputBuf, kOutputBufferSizeInBytes),
                fSocket(inSocket), fBytesSentInBuffer(0), fTimeoutTask(inTimeoutTask),fPrintRTSP(false) {}
        
        virtual ~RTSPResponseStream() {}

        // WriteV
        //
        // 该函数取得一个input ioVec并写入socket。如果任何数据通过Put写入该流，这部分数据首先被写
        //
        // 如果socket上有流控制, 部分数据被请求，或者不发送任何数据。通过inSendType说明你想如何处理未发送数据
        //
        //     kAlwaysBuffer:   内部缓冲任何未发送的数据
        //     kAllOrNothing:   如果没有数据可发，则返回EWOULDBLOCK. 否则缓存任何没发数据
        //     kDontBuffer:     从不缓存数据
        //
        // 如果数据最终被缓存，outLengthSent　= inTotalLength, 并返回QTSS_NoErr 
        
        enum
        {
            kDontBuffer     = 0,
            kAllOrNothing   = 1,
            kAlwaysBuffer   = 2
        };
        QTSS_Error WriteV(iovec* inVec, UInt32 inNumVectors, UInt32 inTotalLength,
                                UInt32* outLengthSent, UInt32 inSendType);

        // Flushes任何待发数据到socket. 如果所有数据都已经发送，返回QTSS_NoErr, 否则EWOULDBLOCK
        QTSS_Error Flush();
        
        void        ShowRTSP(Bool16 enable) {fPrintRTSP = enable; }		//调试用函数 

        
    private:
    
        enum
        {
            kOutputBufferSizeInBytes = 512  //UInt32
        };
        
        //默认的buffer size分配为对象的一部分，因为该大小对99.9%的请求来说足够了，从而避免了大多数情况下的动态内存分配的问题
        //但如果应答太大，BufferIsFull函数将分配一块更大的内存
        char                    fOutputBuf[kOutputBufferSizeInBytes];
        TCPSocket*              fSocket;				//Socket
        UInt32                  fBytesSentInBuffer;		//已经发送的字节数
        TimeoutTask*            fTimeoutTask;			//超时任务
        Bool16                  fPrintRTSP;     // debugging printfs
        
        friend class RTSPRequestInterface;
};


#endif // __RTSP_RESPONSE_STREAM_H__
