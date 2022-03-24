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

/*  File:       RTSPRequest.h

    Contains:   This class encapsulates a single RTSP request. It stores the meta data
                associated with a request, and provides an interface (through its base
                class) for modules to access request information
                
                It divides the request into a series of states.
                
                State 1: At first, when the object is constructed or right after 
                         its Reset function is called, it is in an uninitialized state.
                State 2: Parse() parses an RTSP header. Once this function returns,
                         most of the request-related query functions have been setup.
                         (unless an error occurs)
                State 3: GetHandler() uses the request information to create the
                         proper request Handler object for this request. After that,
                         it is the Handler object's responsibilty to process the
                         request and send a response to the client.

  该类封装了单个RTSP请求. 它存贮了与一个请求关联的元数据, 并为模块访问请求信息提供了一个接口(通过它的基类).

  它把请求分为一系列的状态:

  状态1: 当对象构造成功, 或调用它的Reset函数以后, 处于未初始化状态
  状态2: Parse()解析RTSP头, 一旦该函数返回, 大多数请求相关的功能已经设置成功. (除非发生错误)
  状态3: GetHandler()通过请求信息为该请求创建合适的请求Handler对象. 此后, Handler对象的责任是处理请求并给客户发送回应数据.
*/

#ifndef __RTSPREQUEST_H__
#define __RTSPREQUEST_H__

#include "RTSPRequestInterface.h"
#include "RTSPRequestStream.h"
#include "RTSPResponseStream.h"
#include "RTSPSessionInterface.h"
#include "StringParser.h"
#include "QTSSRTSPProtocol.h"

//HTTPRequest class definition
class RTSPRequest : public RTSPRequestInterface
{
public:

    //CONSTRUCTOR / DESTRUCTOR
    //these do very little. Just initialize / delete some member data.
    //
    //Arguments:        session: the session this request is on (massive cyclical dependency)
    RTSPRequest(RTSPSessionInterface* session)
        : RTSPRequestInterface(session) {}
    virtual ~RTSPRequest() {}
    
    //Parses the request. Returns an error handler if there was an error encountered
    //in parsing.
	//解析RTSP请求, 并把解析结果存入变量
    QTSS_Error Parse();
    
    QTSS_Error ParseAuthHeader(void);
    // called by ParseAuthHeader
    QTSS_Error ParseBasicHeader(StringParser *inParsedAuthLinePtr);
    
    // called by ParseAuthHeader
    QTSS_Error ParseDigestHeader(StringParser *inParsedAuthLinePtr);
	
    void SetupAuthLocalPath(void);			//设置RTSP请求的本地路径
    QTSS_Error SendBasicChallenge(void);
    QTSS_Error SendDigestChallenge(UInt32 qop, StrPtrLen *nonce, StrPtrLen* opaque);
    QTSS_Error SendForbiddenResponse(void);
private:

    //PARSING
    enum { kRealmBuffSize = 512, kAuthNameAndPasswordBuffSize = 128, kAuthChallengeHeaderBufSize = 512};
    
    //Parsing the URI line (first line of request
	//解析方法(如Describe)以及版本号(是一个枚举类型的数据结构)
    QTSS_Error ParseFirstLine(StringParser &parser);
    
    //Utility functions called by above
	//解析URI部分
    QTSS_Error ParseURI(StringParser &parser);

    //Parsing the rest of the headers
    //This assumes that the parser is at the beginning of the headers. It will parse
    //the headers, fill out the data & HTTPParameters object.
    //
    //Returns:      A handler object signifying that a fatal syntax error has occurred
    QTSS_Error ParseHeaders(StringParser& parser);


    //Functions to parse the contents of particuarly complicated headers (as a convienence
    //for modules)
    void    ParseRangeHeader();
    void    ParseTransportHeader();		//解析Transport: RTP/AVP;unicast;client_port=6974-6975
    void    ParseIfModSinceHeader();
    void    ParseAddrSubHeader(StrPtrLen* inSubHeader, StrPtrLen* inHeaderName, UInt32* outAddr);
    void    ParseRetransmitHeader();
    void    ParseContentLengthHeader();
    void    ParseSpeedHeader();
    void    ParsePrebufferHeader();
    void    ParseTransportOptionsHeader();
    void    ParseSessionHeader();
    void    ParseClientPortSubHeader(StrPtrLen* inClientPortSubHeader);
    void    ParseTimeToLiveSubHeader(StrPtrLen* inTimeToLiveSubHeader);
    void    ParseModeSubHeader(StrPtrLen* inModeSubHeader);
    Bool16  ParseNetworkModeSubHeader(StrPtrLen* inSubHeader);
	void 	ParseDynamicRateHeader();
	// DJM PROTOTYPE
	void	ParseRandomDataSizeHeader();

    static UInt8    sURLStopConditions[];
};
#endif // __RTSPREQUEST_H__

