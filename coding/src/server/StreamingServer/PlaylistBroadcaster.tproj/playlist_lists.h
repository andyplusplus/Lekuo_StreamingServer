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

/*  File:       playlist_lists.h

    Contains:   

*/
#ifndef playlist_lists_H
#define playlist_lists_H


#include <stdio.h>
#include <stdlib.h>
#include "SafeStdLib.h"
#include <string.h>
#include <errno.h>


#ifndef __Win32__
    #include <sys/types.h>
    #include <fcntl.h>
#endif

#include "OSHeaders.h"
#include "playlist_array.h"
#include "playlist_elements.h"
#include "playlist_utils.h"



// ************************
//
// SOCKET LIST
//
// ************************


class SocketList : public ArrayList<UDPSocketPair> {
    
};

// ************************
//
// MEDIA STREAM LIST
//
// ************************


class MediaStreamList : public ArrayList<MediaStream> {

    protected:
    
    public:
        SInt16 UpdateStreams();             //调用所有MediaStream的ReceiveOnPorts()
        void UpdateSenderReportsOnStreams();//调用所有MediaStream的UpdateSenderReport(theTime);
        void SetUpStreamSSRCs();			//为每个媒体流MediaStream设置一个唯一的SSRC, SSRC随机生成
        void StreamStarted(SInt64 startTime);//启动所有的流, 调用所有MediaStream的StreamStart(startTime);
        void MovieStarted(SInt64 startTime);//启动Movie, 调用所有MediaStream的MovieStart(startTime)
        void MovieEnded(SInt64 endTime);    //结束Movie, 调用所有MediaStream的MovieEnd(endTime)
};

// ************************
//
// SDP MEDIA LIST
//
// ************************


class SDPMediaList : public ArrayList<TypeMap> {
        
};

#endif //playlist_lists_H
