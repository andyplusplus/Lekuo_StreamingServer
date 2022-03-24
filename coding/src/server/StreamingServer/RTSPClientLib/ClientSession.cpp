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

/*  File:       ClientSession.cpp
Contains:   2008.05.09 把New 换成 WZDNEW，再定义成new, "OSMemory.h"有点问题			*/


#include "Log.h"
#ifdef _WZD_CODE_ENABLED_
	#include "lkRTPPacketFormat.h"
	#include "RelayDVR.h"
	#define WZDNEW new
	#include "RsRTPUtility.h"
#else
	#define WZDNEW NEW
	#include "OSMemory.h"
#endif //_WZD_CODE_ENABLED_

DWORD sDebugValue = 0;


#include "ClientSession.h"
#include <stdlib.h>
#include "SafeStdLib.h"
#define CLIENT_SESSION_DEBUG 1

static const SInt64 kMaxWaitTimeInMsec = 5000;
static const SInt16 kSanitySeqNumDifference = 3000;

UInt32          ClientSession::sActiveConnections = 0;
UInt32          ClientSession::sPlayingConnections = 0;
UInt32          ClientSession::sTotalConnectionAttempts = 0;

UInt32          ClientSession::sBytesReceived = 0;
UInt32          ClientSession::sPacketsReceived = 0;

 
#ifdef _WZD_CODE_ENABLED_

	#include "SocketUtils.h"
	#include <Math.h>		
	#define _WZD_CLIENT_CLIENT_SESSION_DEBUG_ 0
	static int sFileHandle;

#endif //_WZD_CODE_ENABLED_





#pragma region Server Code ===========================================================



	//处理读取到的媒体数据
	//统计RTCP数据的接收情况
	void    ClientSession::ProcessMediaPacket(  char* inPacket, UInt32 inLength,
		UInt32 inTrackID, Bool16 isRTCP)
	{
		Assert(inLength > 4);

		// Currently we do nothing with RTCPs.
		if (isRTCP)
			return;

		UInt16* theSeqNumP = (UInt16*)inPacket;
		UInt16 theSeqNum = ntohs(theSeqNumP[1]);

#ifdef _WZD_CODE_ENABLED_

		DATA_TYPES_DEF	theDataType = RsRTPUtility::getDataType((BYTE*)inPacket, inLength);

		if(en_Print_Received_DataTrace && inLength<en_Print_DataLen_Range)
			RsRTPUtility::printRTPInfo((BYTE*)inPacket, inLength);

		UInt32 theLen = inLength - RS_RTP_TAG_HEAD_SIZE;
		unsigned char* theData = (unsigned char*)(inPacket + RS_RTP_TAG_HEAD_SIZE);

		if(en_ReceiveData && m_IDataReceiver)
			m_IDataReceiver->ReceiveData((HANDLE) this, m_Tag, theData, theLen, theDataType);

		if(DATA_TYPES_NET_DVR_SYSHEAD==theDataType)
		{
			m_MetaDataLen = theLen;
			memcpy(m_MetaDataBuffer, theData, theLen);
		}

		if(m_isFirstPacket)	//用于确认第一个包是否收到
		{
			m_isFirstPacket = false;
			if(DATA_TYPES_NET_DVR_SYSHEAD!=theDataType)
				qtss_printf("==>  ==>  ==>  ==>  The first packet is not DATA_TYPES_NET_DVR_SYSHEAD **********************  \n");
		}


#endif //_WZD_CODE_ENABLED_

		//UInt32* theSsrcP = (UInt32*)inPacket;
		//UInt32 theSSRC = ntohl(theSsrcP[2]);

		for (UInt32 x = 0; x < fSDPParser.GetNumStreams(); x++)
		{
			if (fSDPParser.GetStreamInfo(x)->fTrackID == inTrackID)
			{
				// Check if this packet is even for our stream
				//if (!fStats[x].fIsSSRCValid)
				//  fStats[x].fSSRC = theSSRC; // If we don't know SSRC yet, just use first one we get
				//if (theSSRC != fStats[x].fSSRC)
				//  return;
				fNumPacketsReceived ++;
				fStats[x].fNumPacketsReceived++;
				fStats[x].fNumBytesReceived += inLength;
				sBytesReceived += inLength;
				sPacketsReceived ++;

				// Check if this packet is out of order
				if (fStats[x].fHighestSeqNumValid)
				{                       
					SInt16 theValidationDifference = theSeqNum - fStats[x].fWrapSeqNum;
					if (theValidationDifference < 0)
						theValidationDifference -= 2 * theValidationDifference; // take the absolute value
					if (theValidationDifference > kSanitySeqNumDifference)
					{
						//
						// If this sequence number is really far out of range, then just toss
						// the packet and increment our count of crazy packets
						fStats[x].fNumThrownAwayPackets++;
						return;
					}


					SInt16 theSeqNumDifference = theSeqNum - fStats[x].fHighestSeqNum;

					if ((fTransportType == kReliableUDPTransportType) &&
						(theSeqNumDifference != 1))
						this->AckPackets(x, theSeqNum, true);

					if (theSeqNumDifference > 0)
					{
						fStats[x].fNumOutOfOrderPackets += theSeqNumDifference - 1;
						fStats[x].fHighestSeqNum = theSeqNum;
					}
				}
				else
				{
					fStats[x].fHighestSeqNumValid = true;
					fStats[x].fWrapSeqNum = fStats[x].fHighestSeqNum = theSeqNum;
					fStats[x].fLastAckedSeqNum = theSeqNum - 1;
				}


				UInt32 debugblah = 0;
				// Put this sequence number into the map to track packet loss
				while ( (SInt32)  ( (SInt32) theSeqNum - (SInt32) fStats[x].fWrapSeqNum) > TrackStats::kSeqNumMapSize)
				{
					debugblah++;   
#if CLIENT_SESSION_DEBUG
					if (debugblah > 10)
						printf("theSeqNum= %u fStats[x].fWrapSeqNum =%u debugblah=%lu\n", theSeqNum,fStats[x].fWrapSeqNum,  debugblah);
#endif              
					if (debugblah > 100)
						break;
					// We've cycled through the entire map. Calculate packet
					// loss on the lowest 50 indexes in the map (don't get too
					// close to where we are lest we mistake out of order packets
					// as packet loss)
					UInt32 halfSeqNumMap = TrackStats::kSeqNumMapSize / 2;
					UInt32 curIndex = (fStats[x].fWrapSeqNum + 1) % TrackStats::kSeqNumMapSize;
					UInt32 numPackets = 0;

					for (UInt32 y = 0; y < halfSeqNumMap; y++, curIndex++)
					{
						if (curIndex == TrackStats::kSeqNumMapSize)
							curIndex = 0;

						if (fStats[x].fSequenceNumberMap[curIndex] > 0)
							numPackets++;
						fStats[x].fSequenceNumberMap[curIndex] = 0;
					}

					// We've figured out how many lost packets there are in the lower
					// half of the map. Increment our counters.
					fStats[x].fNumOutOfOrderPackets -= halfSeqNumMap - numPackets;
					fStats[x].fNumLostPackets += halfSeqNumMap - numPackets;
					fStats[x].fWrapSeqNum += (UInt16) halfSeqNumMap;

#if CLIENT_SESSION_DEBUG
					if ( (fStats[x].fNumOutOfOrderPackets > 0) || (fStats[x].fNumLostPackets > 0) )
						qtss_printf("Got %lu packets for trackID %lu. %lu packets lost, %lu packets out of order\n", fStats[x].fNumPacketsReceived, inTrackID, fStats[x].fNumLostPackets, fStats[x].fNumOutOfOrderPackets);  //07.11.30_22：49，客户端Trace表明有丢包存在
#endif              

				}

				//
				// Track duplicate packets
				if (fStats[x].fSequenceNumberMap[theSeqNum % 100])
					fStats[x].fNumDuplicates++;

				fStats[x].fSequenceNumberMap[theSeqNum % 100] = 1;
				theSeqNum = 0;

				RTPMetaInfoPacket::FieldID* theMetaInfoFields = fClient->GetFieldIDArrayByTrack(inTrackID);
				if (theMetaInfoFields != NULL)
				{
					//
					// This packet is an RTP-Meta-Info packet. Parse it out and print out the results
					RTPMetaInfoPacket theMetaInfoPacket;
					if (!theMetaInfoPacket.ParsePacket((UInt8*)inPacket, inLength, theMetaInfoFields))
					{
						qtss_printf("Received invalid RTP-Meta-Info packet\n");
					}
					else
					{
						qtss_printf("---\n");
						qtss_printf("TrackID: %lu\n", inTrackID);
						qtss_printf("Packet transmit time: %" _64BITARG_ "d\n", theMetaInfoPacket.GetTransmitTime());
						qtss_printf("Frame type: %u\n", theMetaInfoPacket.GetFrameType());
						qtss_printf("Packet number: %" _64BITARG_ "u\n", theMetaInfoPacket.GetPacketNumber());
						qtss_printf("Packet position: %" _64BITARG_ "u\n", theMetaInfoPacket.GetPacketPosition());
						qtss_printf("Media data length: %lu\n", theMetaInfoPacket.GetMediaDataLen());
					}
				}
			}
		}
		//  Assert(theSeqNum == 0); // We should always find a track with this track ID
	}


//
//把字节转换成16进制的格式
//
char* ConvertBytesToCHexString( void* inValue, const UInt32 inValueLen)
{
    static const char* kHEXChars={ "0123456789ABCDEF" };

    UInt8* theDataPtr = (UInt8*) inValue;
    UInt32 len = inValueLen *2;
    
    char *theString = WZDNEW char[len+1];
    char *resultStr = theString;
    if (theString != NULL)
    {
        UInt8 temp;
        UInt32 count = 0;
        for (count = 0; count < inValueLen; count++)
        {
            temp = *theDataPtr++;
            *theString++ = kHEXChars[temp >> 4];
            *theString++ = kHEXChars[temp & 0xF];
        }
        *theString = 0;
    }
    return resultStr;
}

// ----------------------------------------------------------------------------------------
//初始化变量, 构造socket以及相关变量
// 1. 创建数据传输Socket
// 2. 在创建的数据传输Socket上建立RTSPClient
// 3. Signal开始发送数据事件
// 4. 开始处理连接相关
//
ClientSession::ClientSession(   UInt32 inAddr, UInt16 inPort, char* inURL,
                                ClientType inClientType,
                                UInt32 inDurationInSec, UInt32 inStartPlayTimeInSec,
                                UInt32 inRTCPIntervalInSec, UInt32 inOptionsIntervalInSec,
                                UInt32 inHTTPCookie, Bool16 inAppendJunkData, UInt32 inReadInterval,
                                UInt32 inSockRcvBufSize, Float32 inLateTolerance, char* inMetaInfoFields,
                                Float32 inSpeed, Bool16 inVerbosePrinting, char* inPacketRangePlayHeader, UInt32 inOverbufferWindowSizeInK,
                                Bool16 sendOptions, Bool16 requestRandomData, SInt32 randomDataSize

								#ifdef _WZD_CODE_ENABLED_
								, LPIDVR inDVR
								, LPIDataReceiver inIDataReceiver
								, Tag inTag
								#endif //_WZD_CODE_ENABLED_								

								)
:   fSocket(NULL),
    fClient(NULL),
    fTimeoutTask(NULL, kIdleTimeoutInMsec),			//2008.01.01 _WZD_  fTimeoutTask(this, kIdleTimeoutInMsec), Modify the Compiler Warning (levels 1 and 4) C4355

    fDurationInSec(inDurationInSec - inStartPlayTimeInSec),	//??
    fStartPlayTimeInSec(inStartPlayTimeInSec),
    fRTCPIntervalInSec(inRTCPIntervalInSec),
    fOptionsIntervalInSec(inOptionsIntervalInSec),
    
    fOptions(sendOptions),
    fOptionsRequestRandomData(requestRandomData),
    fOptionsRandomDataSize(randomDataSize),
    fTransactionStartTimeMilli(0),

    fState(kSendingDescribe),
    fDeathReason(kDiedNormally),
    fNumSetups(0),
    fUDPSocketArray(NULL),
    
    fPlayTime(0),
    fTotalPlayTime(0),
    fLastRTCPTime(0),
    fTeardownImmediately(false),
    fAppendJunk(inAppendJunkData),
    fReadInterval(inReadInterval),
    fSockRcvBufSize(inSockRcvBufSize),
    
    fSpeed(inSpeed),
    fPacketRangePlayHeader(inPacketRangePlayHeader),
    fStats(NULL),
    fOverbufferWindowSizeInK(inOverbufferWindowSizeInK),
    fCurRTCPTrack(0),
    fNumPacketsReceived(0)
	#ifdef _WZD_CODE_ENABLED_
	, m_isFirstPacket(true)
	, m_DVR(inDVR)
	, m_IDataReceiver(inIDataReceiver)
	, m_Tag(inTag)
	, m_PingServerRequest(0)
	#endif //_WZD_CODE_ENABLED_
{
	ClientSession_Play(inAddr, inPort, inURL,
		inClientType, inHTTPCookie, 
		inLateTolerance, inMetaInfoFields, inVerbosePrinting);
	
	qtss_printf("ClientSession::ClientSession   =		(ClientSession*) 0x%lx\n", this);
}

//_WZD_CHANGE_  This function is moved from ClientSession::ClientSession(...) by wzd, 2008.06.08
void ClientSession::ClientSession_Play(UInt32 inAddr, UInt16 inPort, char* inURL,
									   ClientType inClientType, UInt32 inHTTPCookie, 
									   Float32 inLateTolerance, char* inMetaInfoFields, Bool16 inVerbosePrinting)
{
	fTimeoutTask.SetTask(this);
	this->SetTaskName("RTSPClientLib:ClientSession");
	StrPtrLen theURL(inURL);

	if (TRUE == fOptions)
		fState = kSendingOptions;

#if CLIENT_SESSION_DEBUG
	//qtss_printf("Connecting to: %s, port %d\n", inet_ntoa(inAddr), inPort);
	struct in_addr theAddr;
	char theStrAddr[128]; 
	memcpy(&theAddr, &inAddr, sizeof(UInt32));
	strcpy(theStrAddr, inet_ntoa(theAddr));
	qtss_printf("Connecting to: %s, port %d\n", theStrAddr, inPort);
	//
	//	char theStrResult[1024];
	//	sprintf(theStrResult,"Connecting to: %s, port %d\n", theStrAddr, inPort);
	//	OutputDebugString(theStrResult);
#endif  
	// ----------------------------------------------------------------------------------------
	// 1. 创建数据传输Socket，Construct the appropriate ClientSocket type depending on what type of client we are supposed to be
	//
	switch (inClientType)
	{
	case LK_CLIENT_TYPE_UDP:
		{
			fControlType = kRawRTSPControlType;
			fTransportType = kUDPTransportType;
			fSocket = WZDNEW TCPClientSocket(Socket::kNonBlockingSocketType);
			break;
		}
	case LK_CLIENT_TYPE_TCP:
		{
			fControlType = kRawRTSPControlType;
			fTransportType = kTCPTransportType;
			fSocket = WZDNEW TCPClientSocket(Socket::kNonBlockingSocketType);
			break;
		}
	case LK_CLIENT_TYPE_HTTP:
		{
			fControlType = kRTSPHTTPControlType;
			fTransportType = kTCPTransportType;
			fSocket = WZDNEW HTTPClientSocket(theURL, inHTTPCookie, Socket::kNonBlockingSocketType);
			break;
		}
	case LK_CLIENT_TYPE_HTTPDropPost:
		{
			fControlType = kRTSPHTTPDropPostControlType;
			fTransportType = kTCPTransportType;
			fSocket = WZDNEW HTTPClientSocket(theURL, inHTTPCookie, Socket::kNonBlockingSocketType);
			break;
		}
	case LK_CLIENT_TYPE_ReliableUDP:
		{
			fControlType = kRawRTSPControlType;
			fTransportType = kReliableUDPTransportType;
			fSocket = WZDNEW TCPClientSocket(Socket::kNonBlockingSocketType);
			break;
		}
	default:
		{
			qtss_printf("ClientSession: Attempt to create unsupported client type.\n");
			::exit(-1);
		}
	}

	fSocket->Set(inAddr, inPort);
	// ----------------------------------------------------------------------------------------
	// 2. 在创建的数据传输Socket上建立RTSPClient，Construct the client object using this socket.
	//
	char *theUserAgent = "LeKuo Client";
	fClient = WZDNEW RTSPClient(fSocket, inVerbosePrinting, theUserAgent);
	fClient->Set(theURL);
	fClient->SetSetupParams(inLateTolerance, inMetaInfoFields);

	// ----------------------------------------------------------------------------------------
	// 3. 开始处理连接相关，Start the connection process going
	//    
	this->Signal(Task::kStartEvent);

}

// ----------------------------------------------------------------------------------------
// 1. 从fUDPSocketArray接收所有数据，直数据接收错误为止
// 2. 删除fUDPSocketArray数组里面的每个UDPSocket对象
//
//  在此处注意代码关联性
//      RsPlayer --> Channel
//      RelayDVR --> Channel
//      Task     --> Channel
//  Channel对象只能通过以下删除
//                            Teardown           delete task
//  1. RsPlayer --> RelayDVR  -----------> Task -------------->  Channel
//  2.              Connection failed ---> Task -------------->  Channel
//                   对于第于种情况，需要做两种处理
//  1. Tell the RelayDVR about the failure
//  2. Tell the RsPlayer about the failure

ClientSession::~ClientSession()
{
	CLog::LogFun_S("ClientSession::~ClientSession()");

#ifdef _WZD_CODE_ENABLED_
	//if(m_DVR)
	//{
	//	m_DVR->DestroyChannel(this);   //  1. Tell the RelayDVR about the failure
	//}
#endif //_WZD_CODE_ENABLED_

    if (fUDPSocketArray != NULL)
    {
        for (UInt32 x = 0; x < fSDPParser.GetNumStreams() * 2; x++)
        {
            OS_Error theErr = OS_NoErr;
            while (theErr == OS_NoErr)
            {
                    UInt32 theRemoteAddr = 0;
                    UInt32 theLength = 0;
                    UInt16 theRemotePort = 0;
                    char thePacketBuf[2048];
            
                    // Get a packet from one of the UDP sockets.
					//_WZD_ADD_Start
					qtss_printf("x = %d\n", x);
					//_WZD_ADD_End

                    theErr = fUDPSocketArray[x]->RecvFrom(&theRemoteAddr, &theRemotePort,
                                                                &thePacketBuf[0], 2048,
                                                                &theLength);
            }
            delete fUDPSocketArray[x];
        }
    }
    
	//_WZD_Code_Add_Start
	if(fStats)
		delete []fStats;
	//_WZD_Code_Add_End

	delete [] fUDPSocketArray;
    delete fClient;
    delete fSocket;
}

// ----------------------------------------------------------------------------------------
//1. 维护客户端RTSP的状态机
//2. 实现RTP的读取过程
//3. 实现RTCP过程
//
SInt64 ClientSession::Run()
{
	//qtss_printf("*** ClientSession::Run,   this =			%lx\n", this);
	//1.获取事件,如果是kStartEvent, 则返回一个随机时间
		EventFlags theEvents = this->GetEvents();
    
		if (theEvents & Task::kStartEvent)
		{
			sActiveConnections++;
			sTotalConnectionAttempts++;
			Assert(theEvents == Task::kStartEvent);
			//
			// Determine a random connection interval, and go away until that time comes around.
			return ::rand() % kMaxWaitTimeInMsec;
		}
    
	//2.如果是超时事件,则不做任何处理 
		if (theEvents & Task::kTimeoutEvent)
		{
			#if CLIENT_SESSION_DEBUG
					qtss_printf("Session timing out.\n");
			#endif
			fDeathReason = kSessionTimedout;
			fState = kDone;
			return 0;
		}
                
	//3.如果TEARDOWN,则立即停止
		if (theEvents & ClientSession::kTeardownEvent)
		{
			#if CLIENT_SESSION_DEBUG
					qtss_printf("Session tearing down immediately.\n");
			#endif
			fTeardownImmediately = true;
		}
    
	//4.如果kKillEvent,则减少链接数, We have been told to delete ourselves. Do so... NOW!!!!!!!!!!!!!!!
		if (theEvents & Task::kKillEvent)
		{
			#if CLIENT_SESSION_DEBUG
					qtss_printf("Session killed.\n");
			#endif

			sActiveConnections--;
			return -1;
		}   
    
	//5.刷新超时时间, Refresh the timeout. There is some legit activity going on...
		fTimeoutTask.RefreshTimeout();
    
		OS_Error theErr = OS_NoErr;
    
		while ((theErr == OS_NoErr) && (fState != kDone))		//While循环只有一个状态机Switch
		{
			switch (fState)
			{
				case kSendingOptions:
				{
					if (TRUE == fOptionsRequestRandomData)
						theErr = fClient->SendOptionsWithRandomDataRequest(fOptionsRandomDataSize);
					else
						theErr = fClient->SendOptions();

					#if CLIENT_SESSION_DEBUG
						qtss_printf("Sending OPTIONS. Result = %lu. Response code = %lu\n", theErr, fClient->GetStatus());
					#endif

					if (0 == fTransactionStartTimeMilli) 
					   fTransactionStartTimeMilli = OS::Milliseconds();

					if (theErr == OS_NoErr)
					{
                        
						// Check that the OPTIONS response is a 200 OK. If not, bail
						if (fClient->GetStatus() != 200)
						{
							theErr = ENOTCONN; // Exit the state machine
							break;
						}
						else
						{
							if (fClient->IsVerbose())	//是否需要输入跟踪信息
							{
								qtss_printf("--- Options transaction time ms = %qd  ---\n", (SInt64) ( OS::Milliseconds() - fTransactionStartTimeMilli) );
								SInt32 receivedLength = (SInt32) fClient->GetContentLength();
								if (receivedLength != 0)
									qtss_printf("--- Options Request Random Data Received requested = %ld received = %ld  ---\n", fOptionsRandomDataSize, receivedLength);
                                
								StrPtrLenDel theBody(ConvertBytesToCHexString(fClient->GetContentBody(), receivedLength));
								theBody.PrintStr("\n");		//增加结束字符串
							}
							fState = kSendingDescribe;

							#ifdef _WZD_CODE_ENABLED_
								if (m_PingServerRequest) 
								{
									fDeathReason = kWZDResponseTestOK;
									fState = kDone;
									break;
								}
							#endif //_WZD_CODE_ENABLED_
						}
					}

					break;
				}//kSendingOptions:
				case kSendingDescribe:
				{
					theErr = fClient->SendDescribe(fAppendJunk);
	
					#if CLIENT_SESSION_DEBUG
						qtss_printf("Sending DESCRIBE. Result = %lu. Response code = %lu\n", theErr, fClient->GetStatus());
					#endif              

					if (theErr == OS_NoErr)
					{
						// Check that the DESCRIBE response is a 200 OK. If not, bail
						if (fClient->GetStatus() != 200)
						{
							theErr = ENOTCONN; // Exit the state machine
							break;
						}
						else
						{
							//
							// We've sent a describe and gotten a response from the server. Parse the response and look for track information.

							fSDPParser.Parse(fClient->GetContentBody(), fClient->GetContentLength());
							
							#ifdef _WZD_CODE_ENABLED_
								// The following code is used to computing the duration time in second
								StrPtrLen * theSDPPtr = fSDPParser.GetSDPData();
								StringParser theRangeParser(theSDPPtr);
								theRangeParser.GetThru(NULL, '=');//consume "npt="
								theRangeParser.ConsumeWhitespace();
								Float64 theStartTime, theStopTime;
								UInt32 theDuration;
								theStartTime = (Float64)theRangeParser.ConsumeFloat();
								//see if there is a stop time as well.
								if (theRangeParser.GetDataRemaining() > 1)
								{
									theRangeParser.GetThru(NULL, '-');
									theRangeParser.ConsumeWhitespace();
									theStopTime = (Float64)theRangeParser.ConsumeFloat();
								}

								theDuration = (UInt32)ceil(theStopTime - theStartTime);

								fDurationInSec = theDuration > fDurationInSec? theDuration: fDurationInSec;

							#endif //_WZD_CODE_ENABLED_

							//
							// The SDP must have been misformatted.
							if (fSDPParser.GetNumStreams() == 0)
								fDeathReason = kBadSDP;
                            
							//
							// We have valid SDP. If this is a UDP connection, construct a UDP socket array to act as incoming sockets.
							if ((fTransportType == kUDPTransportType) || (fTransportType == kReliableUDPTransportType))
								this->SetupUDPSockets();
                            
							//
							// Setup client stats
							fStats = WZDNEW TrackStats[fSDPParser.GetNumStreams()];
							::memset(fStats, 0, sizeof(TrackStats) * fSDPParser.GetNumStreams());
						}
						fState = kSendingSetup;
					}

					break;
				}//kSendingDescribe:
				case kSendingSetup:
				{
					// The SETUP request is different depending on whether we are interleaving or not
					if (fTransportType == kUDPTransportType)
					{
						theErr = fClient->SendUDPSetup(fSDPParser.GetStreamInfo(fNumSetups)->fTrackID,
													fUDPSocketArray[fNumSetups*2]->GetLocalPort());
					}
					else if (fTransportType == kTCPTransportType)
					{
						fSocket->SetRcvSockBufSize(fSockRcvBufSize); // Make the rcv buf really big
						theErr = fClient->SendTCPSetup(fSDPParser.GetStreamInfo(fNumSetups)->fTrackID,(UInt16)(fNumSetups * 2), (UInt16)((fNumSetups * 2) +1 ) );		//WuZengde: Here added the force conversion
					}
					else if (fTransportType == kReliableUDPTransportType)
					{
						theErr = fClient->SendReliableUDPSetup(fSDPParser.GetStreamInfo(fNumSetups)->fTrackID,
													fUDPSocketArray[fNumSetups*2]->GetLocalPort());
					}

					#if CLIENT_SESSION_DEBUG
						qtss_printf("Sending SETUP #%lu. Result = %lu. Response code = %lu\n", fNumSetups, theErr, fClient->GetStatus());
					#endif              

					//
					// If this SETUP request / response is complete, check for errors, and if it succeeded, 
					//   move onto the next SETUP. If we're done setting up all tracks, move onto PLAY.
					if (theErr == OS_NoErr)
					{
						if (fClient->GetStatus() != 200)
						{
							theErr = ENOTCONN; // Exit the state machine
							break;
						}
						else
						{
							// Record the server port for RTCPs.
							fStats[fNumSetups].fDestRTCPPort = fClient->GetServerPort() + 1;
                        
							fNumSetups++;
							if (fNumSetups == fSDPParser.GetNumStreams())
								fState = kSendingPlay;
						}               
					}

					break;
				}//kSendingSetup:

				case kSendingPlay:
				{
					#ifdef _WZD_CODE_ENABLED_
						theErr = fClient->SendPlay(fStartPlayTimeInSec, fSpeed);		//由于fPacketRangePlayHeader = x-Packet-Range: 屯屯屯屯屯屯屯屯屯屯屯屯屯屯屯屯，所以加上该语句
					#else

						if (fPacketRangePlayHeader != NULL)
							theErr = fClient->SendPacketRangePlay(fPacketRangePlayHeader, fSpeed);
						else
							theErr = fClient->SendPlay(fStartPlayTimeInSec, fSpeed);
					#endif

					#if CLIENT_SESSION_DEBUG
									qtss_printf("Sending PLAY. Result = %lu. Response code = %lu\n", theErr, fClient->GetStatus());
					#endif              
					// If this PLAY request / response is complete, then we are done with setting up all the client crap. 
					//		Now all we have to do is receive the data until it's time to send the TEARDOWN
					if (theErr == OS_NoErr)
					{
						if (fClient->GetStatus() != 200)
						{
							theErr = ENOTCONN; // Exit the state machine
							break;
						}
                        
						// Mark down the SSRC for each track, if possible. 
						for (UInt32 ssrcCount = 0; ssrcCount < fSDPParser.GetNumStreams(); ssrcCount++)
						{
							fStats[ssrcCount].fSSRC = fClient->GetSSRCByTrack(fSDPParser.GetStreamInfo(ssrcCount)->fTrackID);
							if (fStats[ssrcCount].fSSRC != 0)
								fStats[ssrcCount].fIsSSRCValid = true;
						}
                    
						fState = kPlaying;
						sPlayingConnections++;
                    
						//
						// Start our duration timer. Use this to figure out when to send a teardown
						fPlayTime = fLastRTCPTime = OS::Milliseconds();     
					}
					break;
				}//kSendingPlay:

				case kPlaying:
				{
					//qtss_printf("ClientSession::Run  kPlaying =			0x%lx\n", this);
					//if(		((DWORD) this) == sDebugValue	)
					//{
					//	sDebugValue = sDebugValue;
					//}

					#if CLIENT_SESSION_DEBUG
							//qtss_printf("ClientSession::Run  case kPlaying:.\n");
					#endif

					UInt32 theNumStreams = fSDPParser.GetNumStreams();
					theErr = this->ReadMediaData();	

					//_WZD_Comment_Start			
					//if (fCurRTCPTrack == theNumStreams)
					//{
					//	qtss_printf("ClientSession::Run    fCurRTCPTrack = %d, theNumStreams = %d\n", fCurRTCPTrack, theNumStreams);
					//	//theErr = this->ReadMediaData();		//_WZD_Comment_Off
					//}
					//else
					//	qtss_printf("*******************ClientSession::Run    fCurRTCPTrack = %d, theNumStreams = %d\n", fCurRTCPTrack, theNumStreams);
					//_WZD_Comment_End

					//
					// If we've encountered some fatal error, bail.
					if ((theErr != EINPROGRESS) && (theErr != EAGAIN) && (theErr != OS_NoErr))
					{
						sPlayingConnections--;
						break;
					}
					theErr = OS_NoErr; // Ignore flow control errors here.
                    
					//
					// Should we send a teardown? We should if either we've been told to teardown, or if our time has run out
					SInt64 curTime = OS::Milliseconds();


				#ifdef _WZD_CODE_ENABLED_
					if (fTeardownImmediately)
				#else
					if (((curTime - fPlayTime) > fDurationInSec * 1000) || (fTeardownImmediately))
				#endif //_WZD_CODE_ENABLED_					

					{
						sPlayingConnections--;
						fState = kSendingTeardown;
                    
						fTotalPlayTime = curTime - fPlayTime;
					}
					else
					{
						if ((curTime - fLastRTCPTime) > (fRTCPIntervalInSec * 1000))
						{
							//
							// If we are using TCP as our media transport, we only need to
							// send 1 set of RTCPs to the server, to tell it about overbuffering
							if (fTransportType != kTCPTransportType)
							{    fCurRTCPTrack = 0;
								//(void) fClient->SendSetParameter(); // test for keep alives and error responses
								//(void) fClient->SendOptions(); // test for keep alives  and error responses

							}
							fLastRTCPTime = curTime;
						}
                        
						for ( ; fCurRTCPTrack < fSDPParser.GetNumStreams(); fCurRTCPTrack++)
						{
							if (this->SendReceiverReport(fCurRTCPTrack) != OS_NoErr) 
								break;
						}

						// If we are supposed to drop the POST side of the HTTP connection,
						// do so now, after the 1st set of RTCP packets
						if ((fCurRTCPTrack == fSDPParser.GetNumStreams()) && (fControlType == kRTSPHTTPDropPostControlType))
							((HTTPClientSocket*)fSocket)->ClosePost();

						return fReadInterval;
					}
					break;
				}//kPlaying:

				case kSendingTeardown:
				{
					theErr = fClient->SendTeardown();
	#if CLIENT_SESSION_DEBUG
					qtss_printf("Sending TEARDOWN. Result = %lu. Response code = %lu\n", theErr, fClient->GetStatus());
	#endif              
					// Once the TEARDOWN is complete, we are done, so mark ourselves as dead, and wait
					// for the owner of this object to delete us
					if (theErr == OS_NoErr)
						fState = kDone;
					break;
				}//kSendingTeardown:

			}

		}//while ((theErr == OS_NoErr) && (fState != kDone))
    
		if ((theErr == EINPROGRESS) || (theErr == EAGAIN))
		{
			//
			// Request an async event
			fSocket->GetSocket()->SetTask(this);
			fSocket->GetSocket()->RequestEvent(fSocket->GetEventMask());
		}
		else if (theErr != OS_NoErr)
		{
			//
			// We encountered some fatal error with the socket. Record this as a connection failure
			if (fState == kSendingTeardown)
				fDeathReason = kTeardownFailed;
			else if (fState == kPlaying)
				fDeathReason = kDiedWhilePlaying;
			else if (fClient->GetStatus() != 200)
				fDeathReason = kRequestFailed;
			else
				fDeathReason = kConnectionFailed;

			fState = kDone;
		}

		#if CLIENT_SESSION_DEBUG
			if (fState == kDone) qtss_printf("Client connection complete. Death reason = %ul\n", fDeathReason);
			//if(fDeathReason)
			//{
			//	if(fDeathReason<LK_SESSION_Max)
			//		qtss_printf(" +++++++++++++++++  Death reason =    %s\n", LK_SESSION_DIE_REASON_STR[fDeathReason]);
			//	else
			//		qtss_printf(" +++++++++++++++++  Death reason =    %ul\n", fDeathReason);
			//}

		#endif

		#ifdef _WZD_CODE_ENABLED_
		if (fState == kDone)
		{
			if (m_DVR) 
			{
				int isChanDeleted = 1;
				((RelayDVR*)m_DVR)->ReceiveMsg(this, m_Tag, fDeathReason, NULL, 0, isChanDeleted);
			}
			Signal(Task::kKillEvent);
		}
		#endif //_WZD_CODE_ENABLED_

		return 0;
}


//为SDP中的每个流建立UDP端口对(一个端口用于RTP, 另一个端口用于RTCP),并且在该端口上启动事件通知机制, 
void    ClientSession::SetupUDPSockets()
{

    UInt16 sCurrentRTPPortToUse = kLowestUDPPort_Client;
    //static const UInt16 kMinRTPPort = 6970;
    //static const UInt16 kMaxRTPPort = 36970;

    OS_Error theErr = OS_NoErr;
    
    //
    // Create a UDP socket pair (RTP, RTCP) for each stream
    fUDPSocketArray = WZDNEW UDPSocket*[fSDPParser.GetNumStreams() * 2];
    for (UInt32 x = 0; x < fSDPParser.GetNumStreams() * 2; x++)
    {
        fUDPSocketArray[x] = WZDNEW UDPSocket(this, Socket::kNonBlockingSocketType);
        theErr = fUDPSocketArray[x]->Open();
        if (theErr != OS_NoErr)
        {
            qtss_printf("ClientSession: Failed to open a UDP socket.\n");
            ::exit(-1);
        }
    }
    
    for (UInt32 y = 0; y < fSDPParser.GetNumStreams(); y++)
    {   
        for (UInt32 portCheck = 0; true; portCheck++)
        {
            theErr = fUDPSocketArray[y * 2]->Bind(INADDR_ANY, sCurrentRTPPortToUse);
            if (theErr == OS_NoErr)
                theErr = fUDPSocketArray[(y*2)+1]->Bind(INADDR_ANY, sCurrentRTPPortToUse + 1);

            sCurrentRTPPortToUse += 2;
            //if (sCurrentRTPPortToUse > 30000)	//2008.07.10 Comment this two lines
            //   sCurrentRTPPortToUse = 6970;		
                
            if (theErr == OS_NoErr)
            {
                // This is a good pair. Set the rcv buf on the RTP socket to be really big
                fUDPSocketArray[y * 2]->SetSocketRcvBufSize(fSockRcvBufSize);
                break;
            }
                
            if (sCurrentRTPPortToUse >= kHighestUDPPort_Client)
                sCurrentRTPPortToUse = kLowestUDPPort_Client;

            if (portCheck == 5000)
            {
                // Make sure we don't loop forever trying to bind a UDP socket. If we can't
                // after a certain point, just bail...
                qtss_printf("ClientSession: Failed to bind a UDP socket.\n");
                ::exit(-1);
            }
        }
    }                       
#if CLIENT_SESSION_DEBUG
    qtss_printf("Opened UDP sockets for %lu streams\n", fSDPParser.GetNumStreams());
#endif              
}

//从Socket读取媒体数据, 并在RTCP端口上返回RTCP数据
//ClientSession::Run()-->ClientSession::ReadMediaData()-->
OS_Error    ClientSession::ReadMediaData()
{
    // For iterating over the array of UDP sockets
    UInt32 theUDPSockIndex = 0;
    OS_Error theErr = OS_NoErr;
    
    while (true)
    {
        //
        // If the media data is being interleaved, get it from the control connection
        UInt32 theTrackID = 0;
        UInt32 theLength = 0;
        Bool16 isRTCP = false;
        char* thePacket = NULL;
        char thePacketBuf[RS_MAX_PACKET_SIZE];

        if (fTransportType == kTCPTransportType)
        {
            thePacket = NULL;
            theErr = fClient->GetMediaPacket(&theTrackID, &isRTCP, &thePacket, &theLength);
            if (thePacket == NULL)
                break;
        }
        else
        {
            
            UInt32 theRemoteAddr = 0;
            UInt16 theRemotePort = 0;
            
            // Get a packet from one of the UDP sockets.
            theErr = fUDPSocketArray[theUDPSockIndex]->RecvFrom(&theRemoteAddr, &theRemotePort,
                                                                &thePacketBuf[0], RS_MAX_PACKET_SIZE,
                                                                &theLength);
            if ((theErr != OS_NoErr) || (theLength == 0))
            {
                if ((fTransportType == kReliableUDPTransportType) &&
                    (!(theUDPSockIndex & 1)))
                {
                    for (UInt32 trackCount = 0; trackCount < fSDPParser.GetNumStreams(); trackCount++)
                    {
                        if (fSDPParser.GetStreamInfo(trackCount)->fTrackID == fSDPParser.GetStreamInfo(theUDPSockIndex >> 1)->fTrackID)
                        {
                            if (fStats[trackCount].fHighestSeqNumValid)
                                // If we are supposed to be sending acks, and we just finished
                                // receiving all packets for this track that are available at this time,
                                // send an ACK packet
                                this->AckPackets(trackCount, 0, false);
                        }
                    }
                }
                
                theUDPSockIndex++;
                if (theUDPSockIndex == fSDPParser.GetNumStreams() * 2)
                    break;
                continue;
            }
            
            theTrackID = fSDPParser.GetStreamInfo(theUDPSockIndex >> 1)->fTrackID;
            isRTCP = (Bool16)(theUDPSockIndex & 1);
            thePacket = &thePacketBuf[0];
        }

		//wzd_Debug_Start
		//qtss_printf("fTransportType = %d	theTraceID = %d		isRTCP = %d\n", fTransportType, theTrackID, isRTCP, theLength);        
		//wzd_Debug_End

        // We have a valid packet. Invoke the packet handler function
        this->ProcessMediaPacket(thePacket, theLength, theTrackID, isRTCP);
    }
    return theErr;
}


//通过RTCP端口返回确认包
void ClientSession::AckPackets(UInt32 inTrackIndex, UInt16 inCurSeqNum, Bool16 inCurSeqNumValid)
{
    char theRRBuffer[256];
    UInt32  *theWriterStart = (UInt32*)theRRBuffer;
    UInt32  *theWriter = (UInt32*)theRRBuffer;

    // APP PACKET - QoS info
    *(theWriter++) = htonl(0x80CC0000); 
    //*(ia++) = htonl(trk[i].TrackSSRC);
    *(theWriter++) = htonl(0);
    *(theWriter++) = htonl(FOUR_CHARS_TO_INT('q', 't', 'a', 'k'));
    *(theWriter++) = htonl(0);
    
    // Watch out for 16 bit seq num rollover. Dont use SInt32 for theSeqNumDifference, this routine will crash from wrong packet counts after a roll-over. 
    SInt16 theSeqNumDifference = (SInt16) (inCurSeqNum - fStats[inTrackIndex].fHighestSeqNum);
    
    if (!inCurSeqNumValid)
    {
        theSeqNumDifference = 1;
        inCurSeqNum = fStats[inTrackIndex].fHighestSeqNum;
    }
#if CLIENT_SESSION_DEBUG
    qtss_printf("Highest seq num: %d\n", inCurSeqNum);
#endif

    //
    // There may be nothing to do here
    if (inCurSeqNum == fStats[inTrackIndex].fLastAckedSeqNum)
        return;
        
    if (theSeqNumDifference > 0)
    {
        *(theWriter++) = htonl(fStats[inTrackIndex].fLastAckedSeqNum + 1);
#if CLIENT_SESSION_DEBUG
        qtss_printf("TrackID: %d Acking: %d\n", fSDPParser.GetStreamInfo(inTrackIndex)->fTrackID, fStats[inTrackIndex].fLastAckedSeqNum + 1);
#endif

        UInt16 maskPosition = fStats[inTrackIndex].fLastAckedSeqNum + 2;
        SInt16 numPacketsInMask = (inCurSeqNum + 1) - (fStats[inTrackIndex].fLastAckedSeqNum + 2);
        
#if CLIENT_SESSION_DEBUG
        qtss_printf("NumPacketsInMask: %d\n", numPacketsInMask);
#endif
        for (SInt32 y = 0; y < numPacketsInMask; y+=32)
        {
            UInt32 mask = 0;
            for (UInt32 x = 0; x < 32; x++)
            {
                SInt16 offsetFromHighest = fStats[inTrackIndex].fHighestSeqNum - maskPosition;
                mask <<= 1;
    
                if (offsetFromHighest >= 0)
                {
#if CLIENT_SESSION_DEBUG
                    qtss_printf("TrackID: %d Acking in mask: %d\n", fSDPParser.GetStreamInfo(inTrackIndex)->fTrackID, maskPosition);
#endif
                    mask |= 1;
                }
                else if (maskPosition == inCurSeqNum)
                {
#if CLIENT_SESSION_DEBUG
                    qtss_printf("TrackID: %d Acking in mask: %d\n", fSDPParser.GetStreamInfo(inTrackIndex)->fTrackID, inCurSeqNum);
#endif
                    mask |= 1;
                }

                maskPosition++;
            }
            
            // We have 1 completed mask. Add it to the packet
            *(theWriter++) = htonl(mask);
        }
        fStats[inTrackIndex].fLastAckedSeqNum = inCurSeqNum;
    }
    else
    {
        // Just ack cur seq num, this is an out of order packet
        *(theWriter++) = htonl(inCurSeqNum);
    }

    //
    // Set the packet length
    UInt16* lenP = (UInt16*)theRRBuffer;
    lenP[1] = htons((theWriter - theWriterStart) - 1); //length in octets - 1
    
    // Send the packet
    Assert(fStats[inTrackIndex].fDestRTCPPort != 0);
    fUDPSocketArray[(inTrackIndex*2)+1]->SendTo(fSocket->GetHostAddr(), fStats[inTrackIndex].fDestRTCPPort, theRRBuffer,
                                                (theWriter - theWriterStart) * sizeof(UInt32));

    //
    // Update the stats for this track
    fStats[inTrackIndex].fNumAcks++;
}

//通过RTCP端口发送接收者报告
OS_Error ClientSession::SendReceiverReport(UInt32 inTrackID)
{
    //
    // build the RTCP receiver report.
    char theRRBuffer[256];
    UInt32  *theWriterStart = (UInt32*)theRRBuffer;
    UInt32  *theWriter = (UInt32*)theRRBuffer;

    // RECEIVER REPORT
    *(theWriter++) = htonl(0x81c90007);     // 1 src RR packet
    //*(theWriter++) = htonl(trk[i].rcvrSSRC);
    *(theWriter++) = htonl(0);
    //*(theWriter++) = htonl(trk[i].TrackSSRC);
    *(theWriter++) = htonl(0);
    //if (trk[i].rtp_num_received > 0)
    //  t = ((float)trk[i].rtp_num_lost / (float)trk[i].rtp_num_received) * 256;
    //else
    //  t = 0.0;
    //temp = t;
    //temp = (temp & 0xff) << 24;
    //temp |= (trk[i].rtp_num_received & 0x00ffffff);
    *(theWriter++) = htonl(0);
    //temp = (trk[i].seq_num_cycles & 0xffff0000) | (trk[i].last_seq_num & 0x0000ffff);
    //*(ia++) = toBigEndian_ulong(temp);
    *(theWriter++) = htonl(0);
    *(theWriter++) = 0;                         // don't do jitter yet.
    *(theWriter++) = 0;                         // don't do last SR timestamp
    *(theWriter++) = 0;                         // don't do delay since last SR

    // APP PACKET - QoS info
    *(theWriter++) = htonl(0x80CC000C); 
    //*(ia++) = htonl(trk[i].TrackSSRC);
    *(theWriter++) = htonl(0);
// this QTSS changes after beta to 'qtss'
    *(theWriter++) = htonl(FOUR_CHARS_TO_INT('Q', 'T', 'S', 'S'));
    //*(ia++) = toBigEndian_ulong(trk[i].rcvrSSRC);
    *(theWriter++) = htonl(0);
    *(theWriter++) = htonl(8);          // number of 4-byte quants below
#define RR 0x72720004
#define PR 0x70720004
#define PD 0x70640002
#define OB 0x6F620004
    *(theWriter++) = htonl(RR);
    //unsigned int now, secs;
    //now = microseconds();
    //secs = now - trk[i].last_rtcp_packet_sent_us / USEC_PER_SEC;
    //if (secs)
    //  temp = trk[i].bytes_received_since_last_rtcp / secs;
    //else
    //  temp = 0;
    //*(ia++) = htonl(temp);
    *(theWriter++) = htonl(0);
    *(theWriter++) = htonl(PR);
    //*(ia++) = htonl(trk[i].rtp_num_received);
    *(theWriter++) = htonl(0);
    //*(theWriter++) = htonl(PL);
    //*(ia++) = htonl(trk[i].rtp_num_lost);
    //*(theWriter++) = htonl(0);
    *(theWriter++) = htonl(OB);
    *(theWriter++) = htonl(fOverbufferWindowSizeInK * 1024);
    *(theWriter++) = htonl(PD);
    *(theWriter++) = htonl(0);      // should be a short, but we need to pad to a long for the entire RTCP app packet

#if CLIENT_SESSION_DEBUG
    qtss_printf("Sending receiver reports.\n");
#endif
    // Send the packet
    if (fUDPSocketArray != NULL)
    {
        Assert(fStats[inTrackID].fDestRTCPPort != 0);
        fUDPSocketArray[(inTrackID*2)+1]->SendTo(fSocket->GetHostAddr(), fStats[inTrackID].fDestRTCPPort, theRRBuffer,
                                                            (theWriter - theWriterStart) * sizeof(UInt32));
    }
    else
    {
        OS_Error theErr = fClient->PutMediaPacket(fSDPParser.GetStreamInfo(inTrackID)->fTrackID,
                                        true,
                                        theRRBuffer,
                                        (theWriter - theWriterStart) * sizeof(UInt32));
        if (theErr != OS_NoErr)
            return theErr;

    }
    return OS_NoErr;
}
#pragma endregion Server Code ===========================================================
