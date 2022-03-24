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
    File:       RTSPSession.h

    Contains:   代表一个RTSP会话，定义为一个从连接到Fin或Reset结束符的TCP连接生命周期。该对象是调度和完成工作的active element
	            它创建请求，并且在数据到达时进行处理，并需要关闭连接时，它对此进行处理。

    Contains:   Represents an RTSP session (duh), which I define as a complete TCP connection
                lifetime, from connection to FIN or RESET termination. This object is 
                the active element that gets scheduled and gets work done. It creates requests
                and processes them when data arrives. When it is time to close the connection
                it takes care of that.

DSS定义了一个TCPListenerSocket类的子类RTSPListenerSocket，用于监听RTSP连接请求。RTSPListenerSocket类做的唯一一
件事就是重载了GetSessionTask函数，当客户的连接请求到达后，它创建了一个Socket对象和RTSPSession对象的配对。RTSPSession
对象是Task类的子类，是专门用于处理RTSP请求的任务类。

如图7所示，RTSP连接建立后，服务器会为每个客户维护一个Socket对象和RTSPSession对象的配对；当客户的RTSP请求到达时，
Socket对象就会调用RTSPSession对象的Signal方法传信，即将RTSPSession对象加入到TaskThread对象的任务队列中去；而当
时间片到来，TaskThread线程就会调用RTSPSession对象的Run方法，这个方法就会处理客户发送过来的RTSP请求。因此，下面
我们将主要分析RTSPSession的Run方法。

*/

#ifndef __RTSPSESSION_H__
#define __RTSPSESSION_H__

#include "RTSPSessionInterface.h"
#include "RTSPRequestStream.h"
#include "RTSPRequest.h"
#include "RTPSession.h"
#include "TimeoutTask.h"

class RTSPSession : public RTSPSessionInterface
{
    public:
		//构造函数，1.设置任务名 2.设置fRoleParams的不可变部分 3. 设置fModuleState 4. 初始化fProxySessionIDPtr以及fLastRTPSessionIDPtr
        RTSPSession(Bool16 doReportHTTPConnectionAddress);	
        //析构函数，1.关闭模块的RTSP会话 2.清除相关资源
		virtual ~RTSPSession();
        
        // 使用该对象以前调用该函数
        static void Initialize();

        Bool16 IsPlaying() {if (fRTPSession == NULL) return false; if (fRTPSession->GetSessionState() == qtssPlayingState) return true; return false; }
        
        
    private:
		//整个系统的最主要核心之一，维护RTSP状态机
        SInt64 Run();
        
        // Gets & creates RTP session for this request.
        QTSS_Error  FindRTPSession(OSRefTable* inTable);
        QTSS_Error  CreateNewRTPSession(OSRefTable* inTable);
        void        SetupClientSessionAttrs();
        
        // Does request prep & request cleanup, respectively
        void SetupRequest();
        void CleanupRequest();
        
		Bool16 ParseOptionsResponse();
		
        // Fancy random number generator
        UInt32 GenerateNewSessionID(char* ioBuffer);
        
        // Sends an error response & returns error if not ok.
        QTSS_Error IsOkToAddNewRTPSession();
        
        // Checks authentication parameters
        void CheckAuthentication();
        
        // test current connections handled by this object against server pref connection limit
        Bool16 OverMaxConnections(UInt32 buffer);


        char                fLastRTPSessionID[QTSS_MAX_SESSION_ID_LENGTH];
        StrPtrLen           fLastRTPSessionIDPtr;

        RTSPRequest*        fRequest;		//对应的RTSP请求??
        RTPSession*         fRTPSession;	//对应的RTP会话??
        
    
    /* -- begin adds for HTTP ProxyTunnel -- */

    // This gets grabbed whenever the input side of the session is being used.
    // It is to protect POST snarfage while input stuff is in action
    OSMutex             fReadMutex;
    
    OSRef*              RegisterRTSPSessionIntoHTTPProxyTunnelMap(QTSS_RTSPSessionType inSessionType);
    QTSS_Error          PreFilterForHTTPProxyTunnel();              // prefilter for HTTP proxies
    Bool16              ParseProxyTunnelHTTP();                     // use by PreFilterForHTTPProxyTunnel
    void                HandleIncomingDataPacket();
        
    static              OSRefTable* sHTTPProxyTunnelMap;    //?? 该表在Initialize函数中初始化  a map of available partners.

    enum
    {
        kMaxHTTPResponseLen = 300
    };
    static              char        sHTTPResponseHeaderBuf[kMaxHTTPResponseLen];
    static              StrPtrLen   sHTTPResponseHeaderPtr;
    
    static              char        sHTTPResponseNoServerHeaderBuf[kMaxHTTPResponseLen];
    static              StrPtrLen   sHTTPResponseNoServerHeaderPtr;
    
    static              char        *sHTTPResponseFormatStr;
    static              char        *sHTTPNoServerResponseFormatStr;
    char                fProxySessionID[QTSS_MAX_SESSION_ID_LENGTH];    // our magic cookie to match proxy connections
    StrPtrLen           fProxySessionIDPtr;
        OSRef               fProxyRef;
    enum
    {
        // the kinds of HTTP Methods we're interested in for
        // RTSP tunneling
          kHTTPMethodInit       // initialize to this
        , kHTTPMethodUnknown    // tested, but unknown
        , kHTTPMethodGet        // found one of these methods...
        , kHTTPMethodPost
    };
    
    UInt16      fHTTPMethod;
    Bool16      fWasHTTPRequest;
    Bool16      fFoundValidAccept;
    Bool16      fDoReportHTTPConnectionAddress; // true if we need to report our IP adress in reponse to the clients GET request (necessary for servers behind DNS round robin)
    /* -- end adds for HTTP ProxyTunnel -- */
    
    
        // Module invocation and module state. This info keeps track of our current state so that the state machine works properly.
	    //为了跟踪当前处理的情况，RTSPSession类内部定义了多个状态，而Run方法其实就是通过在这些状态之间不断切换，
	    //同时对客户的RTSP请求做出不同的处理。
        enum
        {
            kReadingRequest             = 0,
            kFilteringRequest           = 1,
            kRoutingRequest             = 2,
            kAuthenticatingRequest      = 3,
            kAuthorizingRequest         = 4,
            kPreprocessingRequest       = 5,
            kProcessingRequest          = 6,
            kSendingResponse            = 7,
            kPostProcessingRequest      = 8,
            kCleaningUp                 = 9,
        
        // 当RTSP协议通过HTTP隧道实现时将用到下面的状态 
       
            kWaitingToBindHTTPTunnel = 10,                  // POST or GET side waiting to be joined with it's matching half
            kSocketHasBeenBoundIntoHTTPTunnel = 11,         // POST side after attachment by GET side ( its dying )
            kHTTPFilteringRequest = 12,                     // after kReadingRequest, enter this state
            kReadingFirstRequest = 13,                      // initial state - the only time we look for an HTTP tunnel
            kHaveNonTunnelMessage = 14                  // we've looked at the message, and its not an HTTP tunnle message
        };
        
        UInt32 fCurrentModule;					//当前模块
        UInt32 fState;							//模块状态，用于维护状态机

        QTSS_RoleParams     fRoleParams;		//角色的模块参数块??  module param blocks for roles.
        QTSS_ModuleState    fModuleState;		//模块的状态??
        
        QTSS_Error SetupAuthLocalPath(RTSPRequest *theRTSPRequest);
        
        
        void SaveRequestAuthorizationParams(RTSPRequest *theRTSPRequest);
        QTSS_Error DumpRequestData();

};
#endif // __RTSPSESSION_H__

