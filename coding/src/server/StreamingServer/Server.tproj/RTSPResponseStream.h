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
    
        // �ö����ṩ��һЩ���ػ�����ƣ������ݳɹ�д��socket�Ժ󣬸���timeout
        RTSPResponseStream(TCPSocket* inSocket, TimeoutTask* inTimeoutTask)
            :   ResizeableStringFormatter(fOutputBuf, kOutputBufferSizeInBytes),
                fSocket(inSocket), fBytesSentInBuffer(0), fTimeoutTask(inTimeoutTask),fPrintRTSP(false) {}
        
        virtual ~RTSPResponseStream() {}

        // WriteV
        //
        // �ú���ȡ��һ��input ioVec��д��socket������κ�����ͨ��Putд��������ⲿ���������ȱ�д
        //
        // ���socket����������, �������ݱ����󣬻��߲������κ����ݡ�ͨ��inSendType˵��������δ���δ��������
        //
        //     kAlwaysBuffer:   �ڲ������κ�δ���͵�����
        //     kAllOrNothing:   ���û�����ݿɷ����򷵻�EWOULDBLOCK. ���򻺴��κ�û������
        //     kDontBuffer:     �Ӳ���������
        //
        // ����������ձ����棬outLengthSent��= inTotalLength, ������QTSS_NoErr 
        
        enum
        {
            kDontBuffer     = 0,
            kAllOrNothing   = 1,
            kAlwaysBuffer   = 2
        };
        QTSS_Error WriteV(iovec* inVec, UInt32 inNumVectors, UInt32 inTotalLength,
                                UInt32* outLengthSent, UInt32 inSendType);

        // Flushes�κδ������ݵ�socket. ����������ݶ��Ѿ����ͣ�����QTSS_NoErr, ����EWOULDBLOCK
        QTSS_Error Flush();
        
        void        ShowRTSP(Bool16 enable) {fPrintRTSP = enable; }		//�����ú��� 

        
    private:
    
        enum
        {
            kOutputBufferSizeInBytes = 512  //UInt32
        };
        
        //Ĭ�ϵ�buffer size����Ϊ�����һ���֣���Ϊ�ô�С��99.9%��������˵�㹻�ˣ��Ӷ������˴��������µĶ�̬�ڴ���������
        //�����Ӧ��̫��BufferIsFull����������һ�������ڴ�
        char                    fOutputBuf[kOutputBufferSizeInBytes];
        TCPSocket*              fSocket;				//Socket
        UInt32                  fBytesSentInBuffer;		//�Ѿ����͵��ֽ���
        TimeoutTask*            fTimeoutTask;			//��ʱ����
        Bool16                  fPrintRTSP;     // debugging printfs
        
        friend class RTSPRequestInterface;
};


#endif // __RTSP_RESPONSE_STREAM_H__
