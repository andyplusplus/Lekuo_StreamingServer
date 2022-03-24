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
 File:       SDPUtils.h

 Contains:   Some static routines for dealing with SDPs


 */

#ifndef __SDPUtilsH__
#define __SDPUtilsH__

#include "OS.h"
#include "StrPtrLen.h"
#include "ResizeableStringFormatter.h"
#include "StringParser.h"
#include "OSMemory.h"

//表示SDP的一行
class SDPLine : public StrPtrLen
{
public:
	SDPLine() : fHeaderType('\0') {}
    virtual ~SDPLine() {}

    char    fHeaderType;
};

//该类用于容纳sdp属性值
class SDPContainer
{
    enum { kBaseLines = 20, kLineTypeArraySize = 256};

    enum {  //和fReqLines相对应
            kVPos = 0,		//需要v=
            kSPos,			//需要s=
            kTPos,			//需要t=
            kOPos			//需要o=
         };

    enum {
            kV = 1 << kVPos, 
            kS = 1 << kSPos,
            kT = 1 << kTPos,
            kO = 1 << kOPos,
            kAllReq = kV | kS | kT | kO
          };
            

public:

    SDPContainer(UInt32 numStrPtrs = SDPContainer::kBaseLines) : 
        fNumSDPLines(numStrPtrs), 
        fSDPLineArray(NULL)
    {   
        Initialize();
    }

    ~SDPContainer() {delete [] fSDPLineArray;}
	void		Initialize();							//初始化变量, 申请fSDPLineArray的数组内存
    SInt32      AddHeaderLine (StrPtrLen *theLinePtr);	//把某一行加入到数组中fSDPLineArray
    SInt32      FindHeaderLineType(char id, SInt32 start);//找出某行开始的特定行, 并返回行编号
    SDPLine*    GetNextLine();							//获取下一行
    SDPLine*    GetLine(SInt32 lineIndex);				//获取特定的某一行
    void        SetLine(SInt32 index);					//设置当前行
    void        Parse();								//读出fSDPBuffer中的每一行, 把这些行存入数组, 并设置相关变量
    Bool16      SetSDPBuffer(char *sdpBuffer);			//把sdpBuffer赋值给fSDPBuffer, 并解析该值
    Bool16      SetSDPBuffer(StrPtrLen *sdpBufferPtr);	//把sdpBuffer赋值给fSDPBuffer, 并解析该值
    Bool16      IsSDPBufferValid() {return fValid;}
    Bool16      HasReqLines() { return (Bool16) (fReqLines == kAllReq) ; }
    Bool16      HasLineType( char lineType ) { return (Bool16) (lineType == fFieldStr[lineType]) ; }
    char*       GetReqLinesArray;
    void        PrintLine(SInt32 lineIndex);	//打印出特定的一行
    void        PrintAllLines();				//打印所有行
    SInt32      GetNumLines() { return  fNumUsedLines; }
    
    SInt32      fCurrentLine;	//当前行
    SInt32      fNumSDPLines;	//SDP文件的总行数
    SInt32      fNumUsedLines;	//已经使用的行数
    SDPLine*    fSDPLineArray;	//SDP行数组
    Bool16      fValid;			//SDP文件是否合法
    StrPtrLen   fSDPBuffer;		//SDP缓存
    UInt16      fReqLines;      //是否需要某一特定的行, 通过kV, KS, KT等信状态位表示出来

    char        fFieldStr[kLineTypeArraySize]; //类似哈希表的作用

};

//该类把SDPContainer中的数据按sSessionOrderedLines中定义的顺序重新排序
class SDPLineSorter {

public:
	SDPLineSorter(): fSessionLineCount(0),fSDPSessionHeaders(NULL,0), fSDPMediaHeaders(NULL,0) {};	
	SDPLineSorter(SDPContainer *rawSDPContainerPtr, Float32 adjustMediaBandwidthPercent = 1.0);	//该函数把SDPContainer中的数据按sSessionOrderedLines中定义的顺序重新排序
	
	StrPtrLen* GetSessionHeaders() { return &fSessionHeaders; }
	StrPtrLen* GetMediaHeaders() { return &fMediaHeaders; }
	char* GetSortedSDPCopy();			//该函数把媒体头, 以及会话头合并, 并返回合并后的缓存
	
	StrPtrLen fullSDPBuffSPL;
	SInt32 fSessionLineCount;						//和会话有关的行的行数
	SDPContainer fSessionSDPContainer;				//存放会话相关的信息, 可对此进行排序
	ResizeableStringFormatter fSDPSessionHeaders;	//会话头信息, 用于存放排序后的信息
	ResizeableStringFormatter fSDPMediaHeaders;		//媒体头信息, 用于存放排序后的信息
	StrPtrLen fSessionHeaders;						//和会话相关的头信息, 如
			/*	v=0
				o=QTSS_Play_List 3357231181 3357249607 IN IP4 159.99.242.80
				s=C:\Program Files\VideoRelayServer\Playlists\PlayList300K\@
				c=IN IP4 127.0.0.1
				b=AS:253
				t=0 0
				a=x-broadcastcontrol:RTSP*/
	StrPtrLen fMediaHeaders;//和媒体信息相关的头信息
			/*	m=video 0 RTP/AVP 96
				b=AS:209
				a=rtpmap:96 X-SV3V-ES/90000
				a=control:trackID=1
				m=audio 0 RTP/AVP 97
				b=AS:44
				a=rtpmap:97 X-QDM/32000/2
				a=control:trackID=2
				a=x-bufferdelay:3.66*/
	static char sSessionOrderedLines[];// = "vosiuepcbtrzka"; // chars are order dependent: declared by rfc 2327
	static char sessionSingleLines[];//  = "vosiuepcbzk";    // return only 1 of each of these session field types
	static StrPtrLen sEOL;				//"\r\n"
    static StrPtrLen sMaxBandwidthTag;	// = ("b=AS:");
};


#endif

