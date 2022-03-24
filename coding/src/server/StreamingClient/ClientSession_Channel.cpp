#include "stdAfx.h"
#include "RelayDVR.h"
#include "ClientSession.h"
#include "Log.h"
#include "SocketUtils.h"


ClientSession::ClientSession(rsString inStr, Tag inTag, IMutex* inMutex, LPIDVR inDVR): 
		m_isFirstPacket(true),
		m_CreateStr(inStr),
		m_IDataReceiver(NULL),
		m_Tag(inTag), 
		m_Mutex(inMutex),
		m_DVR(inDVR),
		m_PingServerRequest(0),

		fSocket(NULL),
		fClient(NULL),
		fTimeoutTask(NULL, kIdleTimeoutInMsec),			//2008.01.01 _WZD_  fTimeoutTask(this, kIdleTimeoutInMsec), Modify the Compiler Warning (levels 1 and 4) C4355
		fTransactionStartTimeMilli(0),

		fState(kSendingDescribe),
		fDeathReason(kDiedNormally),
		fNumSetups(0),
		fUDPSocketArray(NULL),

		fPlayTime(0),
		fTotalPlayTime(0),
		fLastRTCPTime(0),
		fTeardownImmediately(false),

		fStats(NULL),
		fCurRTCPTrack(0),
		fNumPacketsReceived(0)
{
	CLog::LogFun_SSTI("ClientSession::ClientSession", inStr.c_str(), inTag, inMutex);
	m_MetaDataLen = 0;

	qtss_printf("ClientSession::ClientSession   =		(ClientSession*) 0x%lx\n", this);
}


LK_Error ClientSession::Attach_DataReceiver(LPIDataReceiver inPlayer)
{
	CLog::LogFun_SI("ClientSession::Attach_DataReceiver", inPlayer);
	m_IDataReceiver = inPlayer;
	return LK_NoErr;
}


LPIDataReceiver ClientSession::Detach_DataReceiver()
{
	CLog::LogFun_S("ClientSession::Detach_DataReceiver");
	LPIDataReceiver theReceiver = m_IDataReceiver;
	m_IDataReceiver = NULL;
	return theReceiver;
}


LK_Error ClientSession::Play(const char* inPlayOption )
{
	CLog::LogFun_SS("ClientSession::Play", inPlayOption);
	rsString thePlayStr = m_CreateStr;
	if(inPlayOption)
	{
		thePlayStr = thePlayStr + ";" +inPlayOption;
	}
	ClientSession_Play(thePlayStr.c_str());

	return LK_NoErr;
}


BOOL ClientSession::IsPlaying()
{
	return (fState == kPlaying);
}

LK_Error ClientSession::Stop()
{
	CLog::LogFun_S("ClientSession::Stop");
	m_IDataReceiver = NULL;
	return LK_NoErr;
}


LK_Error ClientSession::Destroy()
{
	assert(m_DVR);
	m_DVR->DestroyChannel(this);

	return LK_NoErr;
}

//LK_Error ClientSession::Pause();


MetadataLen ClientSession::getMetadataLen()
{
	return m_MetaDataLen;
}


MetadataLen ClientSession::getMetadata(unsigned char* outDataBuffer, MetadataLen inBufferLength)
{
	MetadataLen theLen = m_MetaDataLen;
	if(m_MetaDataLen > 0)
	{
		assert(outDataBuffer != 0);
		memcpy(outDataBuffer, m_MetaDataBuffer, m_MetaDataLen*sizeof(unsigned char));
		assert(m_MetaDataLen<inBufferLength);
	}

	return m_MetaDataLen;
}


//
//
/*Parse the following the data, and set the related values

rtsp://127.0.0.1,ClientType=RTSPUDPClientType,Ext=CreateRelayServer Para
/CreateChannelInRelayServer:DeviceIP=127.0.0.1,DataType=Video,DeviceType=HIKVISION,Ext=CreateChannelInRelayServer Para
/CreateChannelInDevice:ChanID=1,ChanName="",Ext=CreateChannelInDevice Para
/PlayChannel:VideoType=Realtime,Ext=PlayChannel Para
*/
void ClientSession::ClientSession_Play(const char* inStr)  
{
	std::auto_ptr<ILkTokenParser> theTokenParser(CreateTokenParser(inStr));

	char theParsedStr[MAX_CHAR_SIZE];

	UInt32 inAddr;
	UInt16 inPort;
	char inURL[MAX_CHAR_SIZE];
	ClientType inClientType;
	UInt32 inHTTPCookie = 1;
	Float32 inLateTolerance = 0;
	char inMetaInfoFields[MAX_CHAR_SIZE];
		memset(inMetaInfoFields, 0, MAX_CHAR_SIZE);
	Bool16 inVerbosePrinting = FALSE;

	// 
	//Set the IP addr, Port, URL
	theTokenParser->getToken(LK_KEYWORD_STR[LK_KEYWORD_RelayServer_ServerIP], theParsedStr, MAX_CHAR_SIZE, NULL);
	inAddr = SocketUtils::ConvertStringToAddr(theParsedStr);
	inPort = atoi("554");

	rsString theURL("rtsp://");
	rsString theURLEnd("/;");
	theURL += theParsedStr + theURLEnd;	//	rtsp://127.0.0.1/;

	theTokenParser->getAllTokens(theParsedStr, MAX_CHAR_SIZE);
	theURL = theURL + theParsedStr;
	strcpy_s(inURL, theURL.c_str());

	theTokenParser->getToken(LK_KEYWORD_STR[LK_KEYWORD_RelayServer_PingServer], theParsedStr, MAX_CHAR_SIZE, NULL);
	m_PingServerRequest = atoi(theParsedStr);
	//
	//Parse the client type:  LK_CLIENT_TYPE_UDP，LK_CLIENT_TYPE_TCP，LK_CLIENT_TYPE_HTTP，LK_CLIENT_TYPE_HTTPDropPost，LK_CLIENT_TYPE_ReliableUDP
	theTokenParser->getToken(LK_KEYWORD_STR[LK_KEYWORD_RelayServer_ClientType], theParsedStr, MAX_CHAR_SIZE, NULL);
	std::auto_ptr<ILkEnumParser> theEnumParser(CreateEnumParser());
	inClientType = theEnumParser->getEnumItem(LK_CLIENT_TYPE_Min, LK_CLIENT_TYPE_Max, LK_CLIENT_TYPE_STR, theParsedStr);

	fDurationInSec = 0; 
	fStartPlayTimeInSec = 0;
	fRTCPIntervalInSec = 10; 
	fOptionsIntervalInSec = 0;
	if(m_PingServerRequest)
	{
		fOptions= TRUE;							//是否要发送Option请求，如TRUE
	}
	else
	{
		fOptions= FALSE;							//是否要发送Option请求，如TRUE
	}
	fOptionsRequestRandomData = TRUE; 
	fOptionsRandomDataSize = 256;

	fAppendJunk = FALSE; 					//是否在发送的Describe请求后加200字节填充数据，这些数据为'd'
	fReadInterval = 50;
	fSockRcvBufSize = 32768;					//接收Buffer的大小，如32768
	fSpeed = 1; 
	fPacketRangePlayHeader = NULL; 
	
	fOverbufferWindowSizeInK = 5;


	ClientSession_Play(inAddr, inPort, inURL,
		inClientType, inHTTPCookie, 
		inLateTolerance, inMetaInfoFields, inVerbosePrinting);
}
