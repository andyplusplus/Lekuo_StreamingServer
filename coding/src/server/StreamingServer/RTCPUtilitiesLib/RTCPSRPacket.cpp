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
    File:       RTCPSRPacket.cpp

    Contains:   A class that writes a RTCP Sender Report
                
    Change History (most recent first):

    

*/

#include <string.h>

#include "RTCPSRPacket.h"
#include "MyAssert.h"
#include "OS.h"

/*     ����Ĺ��캯�����������½ṹ�İ�

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
7	   |V=2|P| subtype |   PT=APP=204  |             length            | SDES	// V=2, subtype=0, RT=SR=202, length=6, SDES��		
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
8	   |                           SSRC/CSRC                           |  
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
9~14   |                          name (ASCII)                         |	    // QTSS 1147 0859 08      //����Ϊ20���ֽ�
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
15	   |                   application-dependent data                  | APP    // V=2, Sub type=0, RT=SR=204, length=6, APP��
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
16	   |                           SSRC/CSRC                           |  
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
17	   |                   application-dependent data                  | qtsi   // qtsi
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
18	   |                           SSRC/CSRC                           |  
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
19	   |                   application-dependent data                  | qt04   //  qt04
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
20	   |                   application-dependent data                  |        //  
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
21	   |                   application-dependent data                  | BYE    //  V=2, Sub type=0, RT=SR=203, length=6
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
22~33  |                                                               | 
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */

//  ��ʼ��SR��, עSR���ĳ��Ȳ�������һ��
//  1. ���һ��Ψһ��������ΪSDES,Ψһ����������Ϊbase name("QTSS") + ��ǰʱ��, �� theTempCName = QTSS1147085908
//  2. ����SR, SDES, APP, qtsi, qt04, BYE
RTCPSRPacket::RTCPSRPacket()
{
  //1. д��SR��ͷ����Ϣ Write as much of the Sender Report as is possible
    char theTempCName[kMaxCNameLen];
    UInt32 cNameLen = RTCPSRPacket::GetACName(theTempCName);	// theTempCName = QTSS1147085908, ��ʱ����Ϊ20���ֽ�
    
    //write the SR & SDES headers
    UInt32* theSRWriter = (UInt32*)&fSenderReportBuffer;
    *theSRWriter = htonl(0x80c80006);							// SR�ĵ�һ��, V=2, RC=0, RT=SR=200, length=6, �����߱��汾����Ϊ6
																// V=2, Sub type=0, RT=SR=202, length=6

    theSRWriter += 7; //number of UInt32s in an SR.				// Я����SDES��, �����ߵ��ֽ���
    
  //2. д��SDES, SDES length is the length of the CName, plus 2 32bit words, plus the 32bit word for the SSRC
    *theSRWriter = htonl(0x81ca0000 + (cNameLen >> 2) + 1);		// SDES�ĳ����ֶ�ΪCName�ĳ���/4, ����1��SSRC�ֵĳ���. SDES����������������һ���ֵ�ͷһ�г���
    ::memcpy(&fSenderReportBuffer[kSenderReportSizeInBytes], theTempCName, cNameLen);	//kSenderReportSizeInBytes = 9*4
    fSenderReportSize = kSenderReportSizeInBytes + cNameLen;							//kSenderReportSizeInBytes = 14*4, �����߱���ĳ��ȵ���9*4+SDES���ֵĳ���

/*
 SERVER INFO PACKET FORMAT
struct qtss_rtcp_struct
{
    RTCPHeader      header;
    UInt32          ssrc;       // ssrc of rtcp originator
    OSType          name;
    UInt32          senderSSRC;
    SInt16          reserved;
    SInt16          length;     // bytes of data (atoms) / 4
    // qtsi_rtcp_atom structures follow
};
*/

   //3. д��Server��Ϣ, Ӧ�ó����
    // Write the SERVER INFO APP packet


    UInt32* theAckInfoWriter = (UInt32*)&fSenderReportBuffer[fSenderReportSize];
    *theAckInfoWriter = htonl(0x81cc0006);  // V=2, Sub type=0, RT=SR=204, length=6

    theAckInfoWriter += 2;					//����8���ֽ�
    *(theAckInfoWriter++) = htonl(FOUR_CHARS_TO_INT('q', 't', 's', 'i')); // Ack Info APP name
    theAckInfoWriter++; // leave space for the ssrc (again)
    *(theAckInfoWriter++) = htonl(2); // 2 UInt32s for the 'at' field
    *(theAckInfoWriter++) = htonl(FOUR_CHARS_TO_INT( 'a', 't', 0, 4 ));
    fSenderReportWithServerInfoSize = (char*)(theAckInfoWriter+1) - fSenderReportBuffer;    
    
    UInt32* theByeWriter = (UInt32*)&fSenderReportBuffer[fSenderReportWithServerInfoSize];
    *theByeWriter = htonl(0x81cb0001);
}


//���һ��Ψһ������,Ψһ����������Ϊbase name("QTSS") + ��ǰʱ��, ��QTSS1147085908
UInt32 RTCPSRPacket::GetACName(char* ioCNameBuffer)
{
    static char*    sCNameBase = "QTSS";

    //clear out the whole buffer
    ::memset(ioCNameBuffer, 0, kMaxCNameLen);
    
    //cName identifier
    ioCNameBuffer[0] = 1;
    
    //Unique cname is constructed from the base name and the current time
    qtss_sprintf(&ioCNameBuffer[1], " %s%" _64BITARG_ "d", sCNameBase, OS::Milliseconds() / 1000);
    UInt32 cNameLen = ::strlen(ioCNameBuffer);
    //2nd byte of CName should be length
    ioCNameBuffer[1] = (UInt8) (cNameLen - 2);//don't count indicator or length byte

    // This function assumes that the cName is the only item in this SDES chunk
    // (see RTP rfc for details).
    // The RFC says that the item (the cName) should not be NULL terminated, but
    // the chunk *must* be NULL terminated. And padded to a 32-bit boundary.
    //
    // qtss_sprintf already put a NULL terminator in the cName buffer. So all we have to
    // do is pad out to the boundary.
    cNameLen += 1; //add on the NULL character
    UInt32 paddedLength = cNameLen + (4 - (cNameLen % 4));
    
    // Pad, and zero out as we pad.
    for (; cNameLen < paddedLength; cNameLen++)
        ioCNameBuffer[cNameLen] = '\0';
    
    Assert((cNameLen % 4) == 0);
    return cNameLen;
}
