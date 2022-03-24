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
    File:       RTCPSRPacket.h

    Contains:   A class that writes a RTCP Sender Report
                
    Change History (most recent first):
写RTCP发送者报告的类
    
*/

#ifndef __RTCP_SR_PACKET__
#define __RTCP_SR_PACKET__

#include "OSHeaders.h"
#include "OS.h"
#include "MyAssert.h"

#ifndef __Win32__
#include <netinet/in.h> //definition of htonl
#endif

class RTCPSRPacket
{
    public:
    
        enum
        {
            kSRPacketType = 200,    //UInt32
            kByePacketType = 203
        };
		//初始化fSenderReportBuffer, 填入SR, SDES, APP, qtsi, qt04, BYE等字节
        RTCPSRPacket();
        ~RTCPSRPacket() {}
        
        // ACCESSORS
        
        void*   GetSRPacket()       { return &fSenderReportBuffer[0]; }
        UInt32  GetSRPacketLen()    { return fSenderReportWithServerInfoSize; }
        UInt32  GetSRWithByePacketLen() { return fSenderReportWithServerInfoSize + kByeSizeInBytes; }
        
        void*   GetServerInfoPacket() { return &fSenderReportBuffer[fSenderReportSize]; }
        UInt32  GetServerInfoPacketLen() { return kServerInfoSizeInBytes; }

        //
        // MODIFIERS
        
        //
        // FOR SR
        inline void SetSSRC(UInt32 inSSRC);
        inline void SetClientSSRC(UInt32 inClientSSRC);

        inline void SetNTPTimestamp(SInt64 inNTPTimestamp);
        inline void SetRTPTimestamp(UInt32 inRTPTimestamp);
        
        inline void SetPacketCount(UInt32 inPacketCount);
        inline void SetByteCount(UInt32 inByteCount);
        
        //
        // FOR SERVER INFO APP PACKET
        inline void SetAckTimeout(UInt32 inAckTimeoutInMsec);

        //RTCP support requires generating unique CNames for each session.
        //This function generates a proper cName and returns its length. The buffer
        //passed in must be at least kMaxCNameLen
        enum
        {
            kMaxCNameLen = 60   //Uint32
        };
        static UInt32           GetACName(char* ioCNameBuffer);		//获得一个唯一的名字,唯一的名字设置为base name("QTSS") + 当前时间, 如 QTSS1147085908

    private:
    
        enum
        {
            kSenderReportSizeInBytes = 36,
            kServerInfoSizeInBytes = 28,
            kByeSizeInBytes = 8
        };
		//指向SR包的缓冲 36 + 60 + 28 + 8 = 132 /4= 
        char        fSenderReportBuffer[kSenderReportSizeInBytes + kMaxCNameLen + kServerInfoSizeInBytes + kByeSizeInBytes];
        UInt32      fSenderReportSize;	
        UInt32      fSenderReportWithServerInfoSize;	//SR包的长度

};

/*     该类的构造函数构造了以下结构的包

		0                   1                   2                   3
		0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
0	   |V=2|P|    RC   |   PT=SR=200   |             length            | SR		//V=2, RC=0, RT=SR=200, length=6
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
1	   |                         SSRC of sender                        |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
2	   |              NTP timestamp, most significant word             | sender
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ info
3	   |             NTP timestamp, least significant word             |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
4	   |                         RTP timestamp                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
5	   |                     sender's packet count                     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
6	   |                      sender's octet count                     |
	   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
7	   |V=2|P| subtype |   PT=APP=204  |             length            | SDES	// V=2, subtype=0, RT=SR=202, length=6, SDES包		
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
8	   |                           SSRC/CSRC                           |  
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
9~14   |                          name (ASCII)                         |	    // QTSS 1147 0859 08      //长度为20个字节
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
14	   |                   application-dependent data                  | APP    // V=2, Sub type=0, RT=SR=204, length=6, APP包
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
15	   |                           SSRC/CSRC                           |  
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
16	   |                   application-dependent data                  | qtsi   // qtsi
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
17	   |                           SSRC/CSRC                           |  
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
18	   |                   application-dependent data                  |		// 2 ???
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
19	   |                   application-dependent data                  | qt04   //  qt04
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
20     |                   application-dependent data                  |        //  
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
21	   |                   application-dependent data                  | BYE    //  V=2, Sub type=0, RT=SR=203, length=6
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
22	   |                           SSRC/CSRC                           |  
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
22~33  |                                                               | 
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

//设置 1, 8, 15, 22
inline void RTCPSRPacket::SetSSRC(UInt32 inSSRC)
{
    // Set SSRC in SR
    ((UInt32*)&fSenderReportBuffer)[1] = htonl(inSSRC);
    
    // Set SSRC in SDES
    ((UInt32*)&fSenderReportBuffer)[8] = htonl(inSSRC);
    
    // Set SSRC in SERVER INFO
    Assert((fSenderReportSize & 3) == 0);
    ((UInt32*)&fSenderReportBuffer)[(fSenderReportSize >> 2) + 1] = htonl(inSSRC);

    // Set SSRC in BYE
    Assert((fSenderReportWithServerInfoSize & 3) == 0);
    ((UInt32*)&fSenderReportBuffer)[(fSenderReportWithServerInfoSize >> 2) + 1] = htonl(inSSRC);
}

//设置17字节, qtsi
inline void RTCPSRPacket::SetClientSSRC(UInt32 inClientSSRC)
{
    //
    // Set Client SSRC in SERVER INFO
    ((UInt32*)&fSenderReportBuffer)[(fSenderReportSize >> 2) + 3] = htonl(inClientSSRC);    
}

//该函数是否有问题??
inline void RTCPSRPacket::SetNTPTimestamp(SInt64 inNTPTimestamp)
{
#if ALLOW_NON_WORD_ALIGN_ACCESS
    ((SInt64*)&fSenderReportBuffer)[1] = OS::HostToNetworkSInt64(inNTPTimestamp);
#else
    SInt64 temp = OS::HostToNetworkSInt64(inNTPTimestamp);
    ::memcpy(&((SInt64*)&fSenderReportBuffer)[1], &temp, sizeof(temp));
#endif
}

//RTP时间
inline void RTCPSRPacket::SetRTPTimestamp(UInt32 inRTPTimestamp)
{
    ((UInt32*)&fSenderReportBuffer)[4] = htonl(inRTPTimestamp);
}

inline void RTCPSRPacket::SetPacketCount(UInt32 inPacketCount)
{
    ((UInt32*)&fSenderReportBuffer)[5] = htonl(inPacketCount);
}

inline void RTCPSRPacket::SetByteCount(UInt32 inByteCount)
{
    ((UInt32*)&fSenderReportBuffer)[6] = htonl(inByteCount);
}   

//设置第20个字节
inline void RTCPSRPacket::SetAckTimeout(UInt32 inAckTimeoutInMsec)
{
    ((UInt32*)&fSenderReportBuffer)[(fSenderReportWithServerInfoSize >> 2) - 1] = htonl(inAckTimeoutInMsec);
}

#endif //__RTCP_SR_PACKET__
