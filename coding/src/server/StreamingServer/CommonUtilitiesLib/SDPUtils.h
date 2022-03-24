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

//��ʾSDP��һ��
class SDPLine : public StrPtrLen
{
public:
	SDPLine() : fHeaderType('\0') {}
    virtual ~SDPLine() {}

    char    fHeaderType;
};

//������������sdp����ֵ
class SDPContainer
{
    enum { kBaseLines = 20, kLineTypeArraySize = 256};

    enum {  //��fReqLines���Ӧ
            kVPos = 0,		//��Ҫv=
            kSPos,			//��Ҫs=
            kTPos,			//��Ҫt=
            kOPos			//��Ҫo=
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
	void		Initialize();							//��ʼ������, ����fSDPLineArray�������ڴ�
    SInt32      AddHeaderLine (StrPtrLen *theLinePtr);	//��ĳһ�м��뵽������fSDPLineArray
    SInt32      FindHeaderLineType(char id, SInt32 start);//�ҳ�ĳ�п�ʼ���ض���, �������б��
    SDPLine*    GetNextLine();							//��ȡ��һ��
    SDPLine*    GetLine(SInt32 lineIndex);				//��ȡ�ض���ĳһ��
    void        SetLine(SInt32 index);					//���õ�ǰ��
    void        Parse();								//����fSDPBuffer�е�ÿһ��, ����Щ�д�������, ��������ر���
    Bool16      SetSDPBuffer(char *sdpBuffer);			//��sdpBuffer��ֵ��fSDPBuffer, ��������ֵ
    Bool16      SetSDPBuffer(StrPtrLen *sdpBufferPtr);	//��sdpBuffer��ֵ��fSDPBuffer, ��������ֵ
    Bool16      IsSDPBufferValid() {return fValid;}
    Bool16      HasReqLines() { return (Bool16) (fReqLines == kAllReq) ; }
    Bool16      HasLineType( char lineType ) { return (Bool16) (lineType == fFieldStr[lineType]) ; }
    char*       GetReqLinesArray;
    void        PrintLine(SInt32 lineIndex);	//��ӡ���ض���һ��
    void        PrintAllLines();				//��ӡ������
    SInt32      GetNumLines() { return  fNumUsedLines; }
    
    SInt32      fCurrentLine;	//��ǰ��
    SInt32      fNumSDPLines;	//SDP�ļ���������
    SInt32      fNumUsedLines;	//�Ѿ�ʹ�õ�����
    SDPLine*    fSDPLineArray;	//SDP������
    Bool16      fValid;			//SDP�ļ��Ƿ�Ϸ�
    StrPtrLen   fSDPBuffer;		//SDP����
    UInt16      fReqLines;      //�Ƿ���Ҫĳһ�ض�����, ͨ��kV, KS, KT����״̬λ��ʾ����

    char        fFieldStr[kLineTypeArraySize]; //���ƹ�ϣ�������

};

//�����SDPContainer�е����ݰ�sSessionOrderedLines�ж����˳����������
class SDPLineSorter {

public:
	SDPLineSorter(): fSessionLineCount(0),fSDPSessionHeaders(NULL,0), fSDPMediaHeaders(NULL,0) {};	
	SDPLineSorter(SDPContainer *rawSDPContainerPtr, Float32 adjustMediaBandwidthPercent = 1.0);	//�ú�����SDPContainer�е����ݰ�sSessionOrderedLines�ж����˳����������
	
	StrPtrLen* GetSessionHeaders() { return &fSessionHeaders; }
	StrPtrLen* GetMediaHeaders() { return &fMediaHeaders; }
	char* GetSortedSDPCopy();			//�ú�����ý��ͷ, �Լ��Ựͷ�ϲ�, �����غϲ���Ļ���
	
	StrPtrLen fullSDPBuffSPL;
	SInt32 fSessionLineCount;						//�ͻỰ�йص��е�����
	SDPContainer fSessionSDPContainer;				//��ŻỰ��ص���Ϣ, �ɶԴ˽�������
	ResizeableStringFormatter fSDPSessionHeaders;	//�Ựͷ��Ϣ, ���ڴ����������Ϣ
	ResizeableStringFormatter fSDPMediaHeaders;		//ý��ͷ��Ϣ, ���ڴ����������Ϣ
	StrPtrLen fSessionHeaders;						//�ͻỰ��ص�ͷ��Ϣ, ��
			/*	v=0
				o=QTSS_Play_List 3357231181 3357249607 IN IP4 159.99.242.80
				s=C:\Program Files\VideoRelayServer\Playlists\PlayList300K\@
				c=IN IP4 127.0.0.1
				b=AS:253
				t=0 0
				a=x-broadcastcontrol:RTSP*/
	StrPtrLen fMediaHeaders;//��ý����Ϣ��ص�ͷ��Ϣ
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

