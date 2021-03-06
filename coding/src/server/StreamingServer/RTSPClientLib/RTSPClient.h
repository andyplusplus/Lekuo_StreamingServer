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
    File:       RTSPClient.h

    
    
*/

#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include "OSHeaders.h"
#include "StrPtrLen.h"
#include "TCPSocket.h"
#include "ClientSocket.h"
#include "RTPMetaInfoPacket.h"

// for authentication
#include "StringParser.h"
class Authenticator 
{

    public:
        enum { kAuthBufferLen = 1024 };

        enum
        {
            kNoType = 0,
            kBasicType = 1,
            kDigestType = 2 // higher number is stronger type
        };
        
        Authenticator();
        virtual ~Authenticator() {};
        virtual SInt16 GetType() { return 0;};
        virtual Bool16 ParseParams(StrPtrLen *authParamsPtr) {return 0;};
        virtual void AttachAuthParams(StrPtrLen *theRequestPtr) {};
        virtual void ResetAuthParams() {};

        StrPtrLen fAuthBuffer;
        
        StrPtrLen fNameSPL;// client
        StrPtrLen fPasswordSPL;// client
        StrPtrLen fRealmSPL; // server
        StrPtrLen fURISPL; // client
        StrPtrLen fMethodSPL; // client -- must be all caps
        time_t    fAuthTime;
        
        char *GetRequestHeader( StrPtrLen *inSourceStr, StrPtrLen *searchHeaderStr,StrPtrLen *outHeaderStr = NULL);
        void RemoveAuthLine(StrPtrLen *theRequestPtr);      

        Bool16 CopyParam(StrPtrLen *inPtr, StrPtrLen *outPtr);
        
        void SetName(StrPtrLen *inNamePtr);             
        void SetPassword(StrPtrLen *inPasswordPtr); 
        void SetMethod(StrPtrLen *inMethodStr);     
        void SetRealm(StrPtrLen *inRealmPtr);       
        void SetURI(StrPtrLen *inURIPtr);

        void ResetRequestLen(StrPtrLen *theRequestPtr, StrPtrLen *theParamsPtr);
        void ParseTag(StringParser *parserPtr,StrPtrLen *outTagPtr);

        Bool16 GetParamValue(StringParser *valueSourcePtr, StrPtrLen *outParamValuePtr);
        Bool16 GetParamValueAsNewCopy(StringParser *valueSourcePtr, StrPtrLen *outParamValueCopyPtr);
        Bool16 GetMatchListParamValueAsNewCopy(StringParser *valueSourcePtr, StrPtrLen *inMatchListParamValuePtr, SInt16 numToMatch, StrPtrLen *outParamValueCopyPtr);
        Bool16 ParseRealm(StringParser *realmParserPtr);

        static StrPtrLen    sAuthorizationStr;
        static StrPtrLen    sRealmStr;
        static StrPtrLen    sAuthBasicStr;
        static StrPtrLen    sAuthDigestStr;
        static StrPtrLen    sUsernameStr;
        static StrPtrLen    sWildCardMatch;
        static StrPtrLen    sTrue;
        static StrPtrLen    sFalse;

        void Clean();
        
        
    private:
        
        StrPtrLen           fCurrentAuthLine;
};

class BasicAuth : public Authenticator
{
    public:
        BasicAuth() {};
        ~BasicAuth() {Clean();}

        SInt16  GetType() { return kBasicType; };
        Bool16  ParseParams(StrPtrLen *authParamsPtr);
        void    AttachAuthParams(StrPtrLen *theRequestPtr);
        UInt32  ParamsLen(StrPtrLen *requestParams);
        void    ResetAuthParams() {};   
    protected:
        enum {  kEncodedBufferLen = 256 };
        char fEncodedBuffer[kEncodedBufferLen];
        
};

class DigestAuth : public Authenticator 
{
    public:
        
        SInt16  GetType() { return kDigestType; };
        Bool16  ParseParams(StrPtrLen *authParamsPtr);
        void    AttachAuthParams(StrPtrLen *theRequestPtr);
        void    ResetAuthParams();      

        StrPtrLen   fcnonce; 
        StrPtrLen   fNonceCountStr; 
        SInt16      fNonceCount; 
        
        StrPtrLen   fnonce; 
        StrPtrLen   fopaque;

        StrPtrLen   fqop;
        StrPtrLen   fAlgorithmStr;
        StrPtrLen   fStaleStr;
        StrPtrLen   fURIStr;
        StrPtrLen   fRequestDigestStr;
                            
        SInt16      fAlgorithm; 
        Bool16      fStale;
        
        void GenerateAuthorizationRequestLine(StrPtrLen *requestPtr);
        DigestAuth();
        ~DigestAuth();

        enum {kMaxReqParams = 10};
        
    private:
        
        struct ReqFields
        {
            StrPtrLen   *fReqParamTags[kMaxReqParams];
            StrPtrLen   *fReqParamValues[kMaxReqParams];
            Bool16      fQuoted[kMaxReqParams];
            SInt16      fNumFields;
        };
        ReqFields fReqFields;
        void ReqFieldsClean() { memset( (void *) fReqFields.fReqParamTags,0,sizeof(StrPtrLen *) * kMaxReqParams);
                                memset( (void *) fReqFields.fReqParamValues,0,sizeof(StrPtrLen *) * kMaxReqParams);
                                memset( (void *) fReqFields.fQuoted,0,sizeof(Bool16) * kMaxReqParams);
                                fReqFields.fNumFields = 0;
                               }
            
        void AddAuthParam(StrPtrLen *theTagPtr, StrPtrLen *theValuePtr, Bool16 quoted);
        
        UInt32 ParamsLen(StrPtrLen *requestParams);
        void SetNonceCountStr();
        void MakeRequestDigest();
        void MakeCNonce();

// request tags
        static StrPtrLen sCnonceStr;
        static StrPtrLen sUriStr;   
        static StrPtrLen sResponseStr;     
        static StrPtrLen sNonceCountStr;       

// response tags
        static StrPtrLen sStaleStr;

// request and response tags
        static StrPtrLen sNonceStr;    
        static StrPtrLen sQopStr;      
        static StrPtrLen sOpaqueStr;       
        static StrPtrLen sDomainStr;    
        static StrPtrLen sAlgorithmStr;
 
// response values
        static StrPtrLen sQopAuthStr;      
        static StrPtrLen sQopAuthIntStr;       
        static StrPtrLen sMD5Str;
        static StrPtrLen sMD5SessStr;
    
};

class AuthParser
{
    public:
        AuthParser() {};
        ~AuthParser() {}
        
        Authenticator *ParseChallenge(StrPtrLen *challenge);
};

class RTSPClient
{
    public:
    
        //
        // Before using this class, you must set the User Agent this way.
        static void     SetUserAgentStr(char* inUserAgent) { sUserAgent = inUserAgent; }
    
        // verbosePrinting = print out all requests and responses
        RTSPClient(ClientSocket* inSocket, Bool16 verbosePrinting, char* inUserAgent = NULL);
        ~RTSPClient();
        
        // This must be called before any other function. Sets up very important info.
        void        Set(const StrPtrLen& inURL);
        
        //
        // This function allows you to add some special-purpose headers to your
        // SETUP request if you want. These are mainly used for the caching proxy protocol,
        // though there may be other uses.
        //
        // inLateTolerance: default = 0 (don't care)
        // inMetaInfoFields: default = NULL (don't want RTP-Meta-Info packets
        // inSpeed: default = 1 (normal speed)
        void        SetSetupParams(Float32 inLateTolerance, char* inMetaInfoFields);
        
        // Send specified RTSP request to server, wait for complete response.
        // These return EAGAIN if transaction is in progress, OS_NoErr if transaction
        // was successful, EINPROGRESS if connection is in progress, or some other
        // error if transaction failed entirely.
        OS_Error    SendDescribe(Bool16 inAppendJunkData = false);
        
        OS_Error    SendReliableUDPSetup(UInt32 inTrackID, UInt16 inClientPort);
        OS_Error    SendUDPSetup(UInt32 inTrackID, UInt16 inClientPort);
        OS_Error    SendTCPSetup(UInt32 inTrackID, UInt16 inClientRTPid, UInt16 inClientRTCPid);
        OS_Error    SendPlay(UInt32 inStartPlayTimeInSec, Float32 inSpeed = 1);
        OS_Error    SendPacketRangePlay(char* inPacketRangeHeader, Float32 inSpeed = 1);
        OS_Error    SendReceive(UInt32 inStartPlayTimeInSec);       
        OS_Error    SendAnnounce(char *sdp);
        OS_Error    SendTeardown();
        OS_Error    SendInterleavedWrite(UInt8 channel, UInt16 len, char*data,Bool16 *getNext);
                
        OS_Error    SendSetParameter();
        OS_Error    SendOptions();
        OS_Error    SendOptionsWithRandomDataRequest(SInt32 dataSize);
        //
        // If you just want to send a generic request, do it this way
		//????????????????????????????????????
        OS_Error    SendRTSPRequest(iovec* inRequest, UInt32 inNumVecs);

                //
                // Once you call all of the above functions, assuming they return an error, you
                // should call DoTransaction until it returns OS_NoErr, then you can move onto your
                // next request
        OS_Error    DoTransaction();
        
        //
        // If any of the tracks are being interleaved, this fetches a media packet from
        // the control connection. This function assumes that SendPlay has already completed
        // successfully and media packets are being sent.
        OS_Error    GetMediaPacket( UInt32* outTrackID, Bool16* outIsRTCP,
                                    char** outBuffer, UInt32* outLen);

        //
        // If any of the tracks are being interleaved, this puts a media packet to the control
        // connection. This function assumes that SendPlay has already completed
        // successfully and media packets are being sent.
        OS_Error    PutMediaPacket( UInt32 inTrackID, Bool16 isRTCP, char* inBuffer, UInt16 inLen);

        // set name and password for authentication in case we are challenged
        void    SetName(char *name);                
        void    SetPassword(char *password);
                    
        // set control id string default is "trackID"
        void    SetControlID(char* controlID);
 		            
         // ACCESSORS

        StrPtrLen*  GetURL()                { return &fURL; }
        UInt32      GetStatus()             { return fStatus; }
        StrPtrLen*  GetSessionID()          { return &fSessionID; }
        UInt16      GetServerPort()         { return fServerPort; }
        UInt32      GetContentLength()      { return fContentLength; }
        char*       GetContentBody()        { return fRecvContentBuffer; }
        ClientSocket*   GetSocket()         { return fSocket; }
        UInt32      GetTransportMode()      { return fTransportMode; }
        void        SetTransportMode(UInt32 theMode)  { fTransportMode = theMode; };
        
        char*       GetResponse()           { return fRecvHeaderBuffer; }
        UInt32      GetResponseLen()        { return fHeaderLen; }
        Bool16      IsTransactionInProgress() { return fTransactionStarted; }
        Bool16      IsVerbose()             { return fVerbose; }
        
        // If available, returns the SSRC associated with the track in the PLAY response.
        // Returns 0 if SSRC is not available.
        UInt32      GetSSRCByTrack(UInt32 inTrackID);
    
        enum { kPlayMode=0,kPushMode=1,kRecordMode=2};

        //
        // If available, returns the RTP-Meta-Info field ID array for
        // a given track. For more details, see RTPMetaInfoPacket.h
        RTPMetaInfoPacket::FieldID* GetFieldIDArrayByTrack(UInt32 inTrackID);
        
        enum
        {
            kMinNumChannelElements = 5,
            kReqBufSize = 4095,
            kMethodBuffLen = 24 //buffer for "SETUP" or "PLAY" etc.
        };
        
        OSMutex*            GetMutex()      { return &fMutex; }

    private:
        static char*    sUserAgent;	//QuickTime/7.0.3 (qtver=7.0.3;os=Windows NT 5.1Service Pack 2)

    
        // Helper methods
        void        ParseInterleaveSubHeader(StrPtrLen* inSubHeader);
        void        ParseRTPInfoHeader(StrPtrLen* inHeader);
        void        ParseRTPMetaInfoHeader(StrPtrLen* inHeader);

        // Call this to receive an RTSP response from the server.
        // Returns EAGAIN until a complete response has been received.
        OS_Error    ReceiveResponse();
        
        OSMutex             fMutex;//this data structure is shared!
        
        AuthParser      fAuthenticationParser;	
        Authenticator   *fAuthenticator;		// only one will be supported??????????????????????WWW-Authenticate????

        ClientSocket*   fSocket;				//??????socket
        Bool16          fVerbose;				//??????????????????

        // Information we need to send the request
        StrPtrLen   fURL;				//RTSP??URL????rtsp://159.99.242.108/sample_100kbit.mov RTSP/1.0
        UInt32      fCSeq;				//Seq??????1
        
        // authenticate info
        StrPtrLen   fName;				//??????
        StrPtrLen   fPassword;			//????????
        
        // Response data we get back
        UInt32      fStatus;			//??RTSP????????????????
        StrPtrLen   fSessionID;			//????ID
        UInt16      fServerPort;		//??????????
        UInt32      fContentLength;		//????????
        StrPtrLen   fRTPInfoHeader;		//??????????????
        
        // Special purpose SETUP params
        char*           fSetupHeaders;	//SETUP??????????????SETUP????
        
        // If we are interleaving, this maps channel numbers to track IDs
        struct ChannelMapElem  //????????interleaving??????????????channel number??????track ID
        {
            UInt32  fTrackID;
            Bool16  fIsRTCP;
        };							
        ChannelMapElem* fChannelTrackMap;		//????????????channel number??????track ID
        UInt8           fNumChannelElements;	//??????
        
        struct SSRCMapElem		        // ????????SSRC
        {
            UInt32 fTrackID;
            UInt32 fSSRC;
        };
        SSRCMapElem*    fSSRCMap;			// ????????SSRC
        UInt32          fNumSSRCElements;	// SSRC??
        UInt32          fSSRCMapSize;		//SSRC??????????

        struct FieldIDArrayElem				// For storing FieldID arrays??????????FieldID????
        {
            UInt32 fTrackID;
            RTPMetaInfoPacket::FieldID fFieldIDs[RTPMetaInfoPacket::kNumFields];
        };
        FieldIDArrayElem*   fFieldIDMap;			//FieldID????
        UInt32              fNumFieldIDElements;	//FieldID??????????
        UInt32              fFieldIDMapSize;		//FieldID????????
        
        // If we are interleaving, we need this stuff to support the GetMediaPacket function
		// ????????????????GetMediaPacket????
        char*           fPacketBuffer;			//????????????????????
        UInt32          fPacketBufferOffset;	//????????????????????????
        Bool16          fPacketOutstanding;		// ??????????fPacketBuffer??????????, 
			//??????fPacketOutstanding??????????????????, ????????????, ??fPacketOutstanding????????True, 
		    //??????????, ??????????????, ??????????False, ??????????????????
        
        // Data buffers
        char        fMethod[kMethodBuffLen];		// holds the current method??????????????????
        char        fSendBuffer[kReqBufSize + 1];   // for sending requests??????????????????
        char        fRecvHeaderBuffer[kReqBufSize + 1];// for receiving response headers??
        char*       fRecvContentBuffer;             // for receiving response body
        UInt32      fSendBufferLen;					//fSendBuffer????????
        // Tracking the state of our receives, ????????????
        UInt32      fContentRecvLen;		//1
        UInt32      fHeaderRecvLen;			//2
        UInt32      fHeaderLen;				//3
        UInt32      fSetupTrackID;		//RTSP SETUP??????????trackID
        
        // States
        Bool16      fTransactionStarted;	//??????????????RTSP Transaction??????RTSP????????fTransactionStarted = TRUE
        Bool16      fReceiveInProgress;		//??????????????????
        Bool16      fReceivedResponse;		//???????????????
        Bool16      fConnected;			    //????????????????
        Bool16      fHaveTransactionBuffer;	//Transaction??????????????????Socket??????????????????????????????????False
        UInt32      fEventMask;				//?????????
        UInt32      fResponseCount;			//??DoTransaction??????????????fAuthenticator??????
        UInt32      fTransportMode;			//????????????????????????????enum { kPlayMode=0,kPushMode=1,kRecordMode=2};

        //
        // For tracking media data that got read into the header buffer
        UInt32      fPacketDataInHeaderBufferLen;	//??????????????????
        char*       fPacketDataInHeaderBuffer;

            
        char*       fUserAgent;	//QuickTime/7.0.3 (qtver=7.0.3;os=Windows NT 5.1Service Pack 2)
static  char*       sControlID;	//sControlID = "trackID"
        char*       fControlID; // ==sControlID
                
#if DEBUG
        Bool16      fIsFirstPacket;
#endif
        
        //???????????????????????????
        struct InterleavedParams
        {
            char    *extraBytes;
            int     extraLen;
            UInt8   extraChannel;
            int     extraByteOffset;
        };
        static InterleavedParams sInterleavedParams; //??
        
};

#endif //__CLIENT_SESSION_H__
