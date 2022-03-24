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

    Contains:   ����һ��RTSP�Ự������Ϊһ�������ӵ�Fin��Reset��������TCP�����������ڡ��ö����ǵ��Ⱥ���ɹ�����active element
	            ���������󣬲��������ݵ���ʱ���д�������Ҫ�ر�����ʱ�����Դ˽��д���

    Contains:   Represents an RTSP session (duh), which I define as a complete TCP connection
                lifetime, from connection to FIN or RESET termination. This object is 
                the active element that gets scheduled and gets work done. It creates requests
                and processes them when data arrives. When it is time to close the connection
                it takes care of that.

DSS������һ��TCPListenerSocket�������RTSPListenerSocket�����ڼ���RTSP��������RTSPListenerSocket������Ψһһ
���¾���������GetSessionTask���������ͻ����������󵽴����������һ��Socket�����RTSPSession�������ԡ�RTSPSession
������Task������࣬��ר�����ڴ���RTSP����������ࡣ

��ͼ7��ʾ��RTSP���ӽ����󣬷�������Ϊÿ���ͻ�ά��һ��Socket�����RTSPSession�������ԣ����ͻ���RTSP���󵽴�ʱ��
Socket����ͻ����RTSPSession�����Signal�������ţ�����RTSPSession������뵽TaskThread��������������ȥ������
ʱ��Ƭ������TaskThread�߳̾ͻ����RTSPSession�����Run��������������ͻᴦ��ͻ����͹�����RTSP������ˣ�����
���ǽ���Ҫ����RTSPSession��Run������

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
		//���캯����1.���������� 2.����fRoleParams�Ĳ��ɱ䲿�� 3. ����fModuleState 4. ��ʼ��fProxySessionIDPtr�Լ�fLastRTPSessionIDPtr
        RTSPSession(Bool16 doReportHTTPConnectionAddress);	
        //����������1.�ر�ģ���RTSP�Ự 2.��������Դ
		virtual ~RTSPSession();
        
        // ʹ�øö�����ǰ���øú���
        static void Initialize();

        Bool16 IsPlaying() {if (fRTPSession == NULL) return false; if (fRTPSession->GetSessionState() == qtssPlayingState) return true; return false; }
        
        
    private:
		//����ϵͳ������Ҫ����֮һ��ά��RTSP״̬��
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

        RTSPRequest*        fRequest;		//��Ӧ��RTSP����??
        RTPSession*         fRTPSession;	//��Ӧ��RTP�Ự??
        
    
    /* -- begin adds for HTTP ProxyTunnel -- */

    // This gets grabbed whenever the input side of the session is being used.
    // It is to protect POST snarfage while input stuff is in action
    OSMutex             fReadMutex;
    
    OSRef*              RegisterRTSPSessionIntoHTTPProxyTunnelMap(QTSS_RTSPSessionType inSessionType);
    QTSS_Error          PreFilterForHTTPProxyTunnel();              // prefilter for HTTP proxies
    Bool16              ParseProxyTunnelHTTP();                     // use by PreFilterForHTTPProxyTunnel
    void                HandleIncomingDataPacket();
        
    static              OSRefTable* sHTTPProxyTunnelMap;    //?? �ñ���Initialize�����г�ʼ��  a map of available partners.

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
	    //Ϊ�˸��ٵ�ǰ����������RTSPSession���ڲ������˶��״̬����Run������ʵ����ͨ������Щ״̬֮�䲻���л���
	    //ͬʱ�Կͻ���RTSP����������ͬ�Ĵ���
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
        
        // ��RTSPЭ��ͨ��HTTP���ʵ��ʱ���õ������״̬ 
       
            kWaitingToBindHTTPTunnel = 10,                  // POST or GET side waiting to be joined with it's matching half
            kSocketHasBeenBoundIntoHTTPTunnel = 11,         // POST side after attachment by GET side ( its dying )
            kHTTPFilteringRequest = 12,                     // after kReadingRequest, enter this state
            kReadingFirstRequest = 13,                      // initial state - the only time we look for an HTTP tunnel
            kHaveNonTunnelMessage = 14                  // we've looked at the message, and its not an HTTP tunnle message
        };
        
        UInt32 fCurrentModule;					//��ǰģ��
        UInt32 fState;							//ģ��״̬������ά��״̬��

        QTSS_RoleParams     fRoleParams;		//��ɫ��ģ�������??  module param blocks for roles.
        QTSS_ModuleState    fModuleState;		//ģ���״̬??
        
        QTSS_Error SetupAuthLocalPath(RTSPRequest *theRTSPRequest);
        
        
        void SaveRequestAuthorizationParams(RTSPRequest *theRTSPRequest);
        QTSS_Error DumpRequestData();

};
#endif // __RTSPSESSION_H__

