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
    File:       SDPSourceInfo.h

    Contains:   This object takes input SDP data, and uses it to support the SourceInfo API.

该对象采用输入SDP数据,并用它来支持SourceInfo API

*/

#ifndef __SDP_SOURCE_INFO_H__
#define __SDP_SOURCE_INFO_H__

#include "StrPtrLen.h"
#include "SourceInfo.h"
#include "StringParser.h"

class SDPSourceInfo : public SourceInfo
{
    public:
    
        // Uses the SDP Data to build up the StreamInfo structures
        SDPSourceInfo(char* sdpData, UInt32 sdpLen) { Parse(sdpData, sdpLen); }
        SDPSourceInfo() {}
        virtual ~SDPSourceInfo();
        
        // Parses out the SDP file provided, sets up the StreamInfo structures
        void    Parse(char* sdpData, UInt32 sdpLen);

        // This function uses the Parsed SDP file, and strips out all the network information,
        // producing an SDP file that appears to be local.
        virtual char*   GetLocalSDP(UInt32* newSDPLen);

        // Returns the SDP data
        StrPtrLen*  GetSDPData()    { return &fSDPData; }
        
        // Utility routines
        
        // Assuming the parser is currently pointing at the beginning of an dotted-
        // decimal IP address, this consumes it (stopping at inStopChar), and returns
        // the IP address (host ordered) as a UInt32
        static UInt32 GetIPAddr(StringParser* inParser, char inStopChar);
      
    private:

        enum
        {
            kDefaultTTL = 15    //UInt16
        };
        StrPtrLen   fSDPData;	// 设置为具体的SDP数据
};

/*fSDPData数据的范例

	v=0
	o=StreamingServer 3353367493 1109641162000 IN IP4 159.99.242.108
	s=\sample_100kbit.mov
	u=http:///
	e=admin@
	c=IN IP4 0.0.0.0
	b=AS:94
	t=0 0
	a=control:*
	a=range:npt=0-  70.00000
	m=video 0 RTP/AVP 96
	b=AS:79
	a=rtpmap:96 X-SV3V-ES/90000
	a=control:trackID=3
	m=audio 0 RTP/AVP 97
	b=AS:14
	a=rtpmap:97 X-QDM/22050/2
	a=control:trackID=4

*/


#endif // __SDP_SOURCE_INFO_H__

