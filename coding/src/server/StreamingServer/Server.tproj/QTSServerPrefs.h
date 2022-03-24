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
    File:       QTSServerPrefs.h
    Contains:   Object store for RTSP server preferences.
    
*/

#ifndef __QTSSERVERPREFS_H__
#define __QTSSERVERPREFS_H__

#include "StrPtrLen.h"
#include "QTSSPrefs.h"
#include "XMLPrefsParser.h"

class QTSServerPrefs : public QTSSPrefs
{
    public:

        // INITIALIZE
        //
        // This function sets up the dictionary map. Must be called before instantiating
        // the first RTSPPrefs object.
    
        static void Initialize();

        QTSServerPrefs(XMLPrefsParser* inPrefsSource, Bool16 inWriteMissingPrefs);
        virtual ~QTSServerPrefs() {}
        
        //This is callable at any time, and is thread safe wrt to the accessors.
        //Pass in true if you want this function to update the prefs file if
        //any defaults need to be used. False otherwise
        void RereadServerPreferences(Bool16 inWriteMissingPrefs);
        
        //Individual accessor methods for preferences.
        
        //Amount of idle time after which respective protocol sessions are timed out
        //(stored in seconds)
        
        //This is the value we advertise to clients (lower than the real one)
        UInt32  GetRTSPTimeoutInSecs()  { return fRTSPTimeoutInSecs; }
        UInt32  GetRTPTimeoutInSecs()   { return fRTPTimeoutInSecs; }
        StrPtrLen*  GetRTSPTimeoutAsString() { return &fRTSPTimeoutString; }
        
        //This is the real timeout
        UInt32  GetRealRTSPTimeoutInSecs(){ return fRealRTSPTimeoutInSecs; }
        
        //-1 means unlimited
        SInt32  GetMaxConnections()         { return fMaximumConnections; }
        SInt32  GetMaxKBitsBandwidth()      { return fMaxBandwidthInKBits; }
        
        // Thinning algorithm parameters
        SInt32  GetDropAllPacketsTimeInMsec()           { return fDropAllPacketsTimeInMsec; }
        SInt32  GetDropAllVideoPacketsTimeInMsec()      { return fDropAllVideoPacketsTimeInMsec; }
        SInt32  GetThinAllTheWayTimeInMsec()            { return fThinAllTheWayTimeInMsec; }
        SInt32  GetAlwaysThinTimeInMsec()               { return fAlwaysThinTimeInMsec; }
        SInt32  GetStartThinningTimeInMsec()            { return fStartThinningTimeInMsec; }
        SInt32  GetStartThickingTimeInMsec()            { return fStartThickingTimeInMsec; }
        SInt32  GetThickAllTheWayTimeInMsec()           { return fThickAllTheWayTimeInMsec; }
        UInt32  GetQualityCheckIntervalInMsec()         { return fQualityCheckIntervalInMsec; }
                
        // for tcp buffer size scaling
        UInt32  GetMinTCPBufferSizeInBytes()            { return fMinTCPBufferSizeInBytes; }
        UInt32  GetMaxTCPBufferSizeInBytes()            { return fMaxTCPBufferSizeInBytes; }
        Float32 GetTCPSecondsToBuffer()                 { return fTCPSecondsToBuffer; }
        
        //for joining HTTP sessions from behind a round-robin DNS
        Bool16  GetDoReportHTTPConnectionAddress()      { return fDoReportHTTPConnectionAddress;  }
        
        //for debugging, mainly
        Bool16      ShouldServerBreakOnAssert()         { return fBreakOnAssert; }
        Bool16      IsAutoRestartEnabled()              { return fAutoRestart; }

        UInt32      GetTotalBytesUpdateTimeInSecs()     { return fTBUpdateTimeInSecs; }
        UInt32      GetAvgBandwidthUpdateTimeInSecs()   { return fABUpdateTimeInSecs; }
        UInt32      GetSafePlayDurationInSecs()         { return fSafePlayDurationInSecs; }
        
        // For the compiled-in error logging module
        
        Bool16  IsErrorLogEnabled()             { return fErrorLogEnabled; }
        Bool16  IsScreenLoggingEnabled()        { return fScreenLoggingEnabled; }

        UInt32  GetMaxErrorLogBytes()           { return fErrorLogBytes; }
        UInt32  GetErrorRollIntervalInDays()    { return fErrorRollIntervalInDays; }
        UInt32  GetErrorLogVerbosity()          { return fErrorLogVerbosity; }
        void	SetErrorLogVerbosity(UInt32 verbosity)		{ fErrorLogVerbosity = verbosity; }
        Bool16  GetAppendSrcAddrInTransport()   { return fAppendSrcAddrInTransport; }
        
        //
        // For UDP retransmits
        UInt32  IsReliableUDPEnabled()          { return fReliableUDP; }
        UInt32  GetMaxRetransmitDelayInMsec()   { return fMaxRetransDelayInMsec; }
        Bool16  IsAckLoggingEnabled()           { return fIsAckLoggingEnabled; }
        UInt32  GetRTCPPollIntervalInMsec()     { return fRTCPPollIntervalInMsec; }
        UInt32  GetRTCPSocketRcvBufSizeinK()    { return fRTCPSocketRcvBufSizeInK; }
        UInt32  GetSendIntervalInMsec()         { return fSendIntervalInMsec; }
        UInt32  GetMaxSendAheadTimeInSecs()     { return fMaxSendAheadTimeInSecs; }
        Bool16  IsSlowStartEnabled()            { return fIsSlowStartEnabled; }
        Bool16  GetReliableUDPPrintfsEnabled()  { return fReliableUDPPrintfs; }
        Bool16  GetRTSPDebugPrintfs()           { return fEnableRTSPDebugPrintfs; }
        Bool16  GetRTSPServerInfoEnabled()      { return fEnableRTSPServerInfo; }
        
		Float32	GetOverbufferRate()				{ return fOverbufferRate; }
		
        // RUDP window size
        UInt32  GetSmallWindowSizeInK()         { return fSmallWindowSizeInK; }
		UInt32	GetMediumWindowSizeInK()		{ return fMediumWindowSizeInK; }
        UInt32  GetLargeWindowSizeInK()         { return fLargeWindowSizeInK; }
        UInt32  GetWindowSizeThreshold()        { return fWindowSizeThreshold; }
 		UInt32	GetWindowSizeMaxThreshold()		{ return fWindowSizeMaxThreshold; }
       
        //
        // force logs to close after each write (true or false)
        Bool16  GetCloseLogsOnWrite()           { return fCloseLogsOnWrite; }
        void    SetCloseLogsOnWrite(Bool16 closeLogsOnWrite);
        
        //
        // Optionally require that reliable UDP content be in certain folders
        Bool16 IsPathInsideReliableUDPDir(StrPtrLen* inPath);

        // Movie folder pref. If the path fits inside the buffer provided,
        // the path is copied into that buffer. Otherwise, a new buffer is allocated
        // and returned.
        char*   GetMovieFolder(char* inBuffer, UInt32* ioLen);
        
        //
        // Transport addr pref. Caller must provide a buffer big enough for an IP addr
        void    GetTransportSrcAddr(StrPtrLen* ioBuf);
                
        // String preferences. Note that the pointers returned here is allocated
        // memory that you must delete!
        
        char*   GetErrorLogDir()
            { return this->GetStringPref(qtssPrefsErrorLogDir); }
        char*   GetErrorLogName()
            { return this->GetStringPref(qtssPrefsErrorLogName); }

        char*   GetModuleDirectory()
        { 
			return this->GetStringPref(qtssPrefsModuleFolder); 
		}
            
        char*   GetAuthorizationRealm()
            { return this->GetStringPref(qtssPrefsDefaultAuthorizationRealm); }

        char*   GetRunUserName()
            { return this->GetStringPref(qtssPrefsRunUserName); }
        char*   GetRunGroupName()
            { return this->GetStringPref(qtssPrefsRunGroupName); }

		char*   GetPidFilePath()
			{ return this->GetStringPref(qtssPrefsPidFile); }

        char*   GetStatsMonitorFileName()
            { return this->GetStringPref(qtssPrefsMonitorStatsFileName); }

        Bool16 ServerStatFileEnabled()      { return fEnableMonitorStatsFile; }
        UInt32 GetStatFileIntervalSec()     { return fStatsFileIntervalSeconds; }
        Bool16  AutoDeleteSDPFiles()        { return fauto_delete_sdp_files; }
        QTSS_AuthScheme GetAuthScheme()     { return fAuthScheme; }
        
        Bool16 PacketHeaderPrintfsEnabled() { return fEnablePacketHeaderPrintfs; }
        Bool16 PrintRTPHeaders()    { return (Bool16) (fPacketHeaderPrintfOptions & kRTPALL); }
        Bool16 PrintSRHeaders()     { return (Bool16) (fPacketHeaderPrintfOptions & kRTCPSR); }
        Bool16 PrintRRHeaders()     { return (Bool16) (fPacketHeaderPrintfOptions & kRTCPRR); }
        Bool16 PrintAPPHeaders()     { return (Bool16) (fPacketHeaderPrintfOptions & kRTCPAPP); }
        Bool16 PrintACKHeaders()     { return (Bool16) (fPacketHeaderPrintfOptions & kRTCPACK); }

        UInt32 DeleteSDPFilesInterval()     { return fsdp_file_delete_interval_seconds; }
                
        UInt32  GetNumThreads()             { return fNumThreads; }
        
        Bool16  DisableThinning()           { return fDisableThinning; }

		//_WZD_PATH_ Start
		static char** GetPrefInfoDefaultValueAddr(int nIndex)     { return &(sPrefInfo[nIndex].fDefaultValue); }
		//_WZD_PATH_ End

    private:

        UInt32      fRTSPTimeoutInSecs;		//qtssPrefsRTSPTimeout 服务器告知客户在断开空闲RTSP客户之前准备等待的时间，以秒计。缺省情况下，这个属性的值为0。
        char        fRTSPTimeoutBuf[20];
        StrPtrLen   fRTSPTimeoutString;
        UInt32      fRealRTSPTimeoutInSecs;	//qtssPrefsRealRTSPTimeout 服务器在断开空闲RTSP客户之前的实际等待时间，以秒计。每次服务器从客户端接收到新的RTSP请求时，这个定时器都会被重置。这个值为０表示永远不超时。缺省情况下，这个属性的值为180。
        UInt32      fRTPTimeoutInSecs;		//qtssPrefsRTPTimeout 服务器在断开空闲的RTP连接之前准备等待的时间，以秒计。每次服务器从客户端接收到新的RTCP状态包时，这个定时器都会被重置。这个值为０表示永远不超时。缺省情况下，这个属性的值为120。
        
        SInt32  fMaximumConnections;		//qtssPrefsMaximumConnections 服务器允许的RTP连接的最大并发数。这个值为1表示允许无限连接数。缺省情况下，这个属性的值为1000。
        SInt32  fMaxBandwidthInKBits;		//qtssPrefsMaximumBandwidth 允许服务器使用的最大带宽，以Kb计。如果服务器使用的带宽超过这个值，则向请求流媒体服务的新客户返回453号的RTSP错误，"没有足够的带宽"。这个值为1表示允许服务器使用无限带宽。缺省情况下，这个属性的值为102400。
        
        Bool16  fBreakOnAssert;		//qtssPrefsBreakOnAssert 如果这个属性值为真，则服务器在满足断言（assert）条件时会进入调试器。缺省情况下，这个属性的值为假（false）。
        Bool16  fAutoRestart;		//qtssPrefsAutoRestart 如果这个属性值为真，则服务器在崩溃后会自动重起。缺省情况下，这个属性的值为真（true）。
        UInt32  fTBUpdateTimeInSecs;		//qtssPrefsTotalBytesUpdate 两次更新服务器的字节总数和当前带宽统计值之间的间隔时间，以秒计。缺省情况下，这个属性的值为1。
        UInt32  fABUpdateTimeInSecs;		//qtssPrefsAvgBandwidthUpdate 两次计算服务器平均带宽的间隔时间，以秒计。缺省情况下，这个属性的值为60。
        UInt32  fSafePlayDurationInSecs;	//qtssPrefsSafePlayDuration 如果服务器发现自己使用的带宽超出允许的最大带宽（通过计算平均带宽），会试图断开最新建立连接的客户，直到平均带宽下降到可接受的水平。然而，如果客户建立连接的时间比这个属性指定的秒数还长，则不会断开该连接。如果这个属性值设定为０，则服务器将不会断开客户连接。缺省情况下，这个属性的值为600。
        
        UInt32  fErrorRollIntervalInDays;	//qtssPrefsErrorRollInterval 滚动错误日志文件的间隔时间，按天计。缺省情况下，这个属性的值为0，表示不滚动错误日志文件。
        UInt32  fErrorLogBytes;		//qtssPrefsMaxErrorLogSize 错误日志的最大尺寸，按字节算。值为０表示服务器没有对此设限。缺省情况下，这个属性的值为256000。
        UInt32  fErrorLogVerbosity;		//qtssPrefsMaxErrorLogSize 错误日志的最大尺寸，按字节算。值为０表示服务器没有对此设限。缺省情况下，这个属性的值为256000。
        Bool16  fScreenLoggingEnabled;	//qtssPrefsScreenLogging 如果这个属性被设置为真（），则每行错误日志都会被写入到终端窗口中。请注意，如果希望看到这些错误日志，服务器必须通过命令行启动，并且通过true-d 选项使之处于前台运行模式。缺省情况下，这个属性的值为真（ true）。
        Bool16  fErrorLogEnabled;		//qtssPrefsErrorLogEnabled 将这个属性设置为真（true）可以激活错误日志功能。缺省情况下，这个属性的值为真（true）。
        
        SInt32  fDropAllPacketsTimeInMsec;		//qtssPrefsDropVideoAllPacketsDelayInMsec 如果视频数据包在这个属性指定的时间内（以毫秒计）没有被发送出去，则服务器就会将它丢弃。这个属性用于服务器的打薄（thinning）算法中。缺省情况下，这个属性的值为1750。
        SInt32  fDropAllVideoPacketsTimeInMsec;		//qtssPrefsDropVideoAllPacketsDelayInMsec 如果视频数据包在这个属性指定的时间内（以毫秒计）没有被发送出去，则服务器就会将它丢弃。这个属性用于服务器的打薄（thinning）算法中。缺省情况下，这个属性的值为1750。
        SInt32  fThinAllTheWayTimeInMsec;		//qtssPrefsStartThinningDelayInMsec 如果数据包在这个属性指定的时间后还没有到达，则服务器开始打薄。缺省情况下，这个属性的值为0。
        SInt32  fAlwaysThinTimeInMsec;		//qtssPrefsAlwaysThinDelayInMsec 如果数据包延迟的时间达到这个属性指定的毫秒数，服务器就会开始打薄。这个属性是服务器打薄算法的一部分。缺省情况下，这个属性的值为750。
        SInt32  fStartThinningTimeInMsec;		//qtssPrefsStartThickingDelayInMsec 如果数据包延迟的时间达到这个属性指定的毫秒数，服务器就会开始增厚（thicking）。这个属性是服务器打薄算法的一部分。缺省情况下，这个属性的值为250。
        SInt32  fStartThickingTimeInMsec;	    //qtssPrefsStartThickingDelayInMsec 如果数据包延迟的时间达到这个属性指定的毫秒数，服务器就会开始增厚（thicking）。这个属性是服务器打薄算法的一部分。缺省情况下，这个属性的值为250。
        SInt32  fThickAllTheWayTimeInMsec;		//qtssPrefsThickAllTheWayDelayInMsec 如果数据包在这个属性指定的时间内发出（负值表示超前），则将流媒体的品质完全恢复。这个属性是服务器打薄算法的一部分。缺省情况下，这个属性的值为-2000。
        UInt32  fQualityCheckIntervalInMsec;		//qtssPrefsStartQualityCheckIntervalInMsec 服务器检查是否打薄并在必要时进行调整的间隔时间，以毫秒计。这个属性是服务器打薄算法的一部分。缺省情况下，这个属性的值为1000。

        UInt32  fMinTCPBufferSizeInBytes;		//qtssPrefsMinTCPBufferSizeInBytes TCP套接口发送缓冲区可以被设定的最小尺寸，以字节计。缺省情况下，这个属性的值为8192。
        UInt32  fMaxTCPBufferSizeInBytes;		//qtssPrefsMaxTCPBufferSizeInBytes TCP套接口发送缓冲区可以被设定的最大尺寸，以字节计。缺省情况下，这个属性的值为200000。
        Float32 fTCPSecondsToBuffer;			//qtssPrefsTCPSecondsToBuffer 在TCP之上进行流媒体服务时，发送缓冲区的尺寸会根据电影的位率进行调整。服务器会根据电影的位率对TCP发送缓冲区进行设定，使其与这个属性指定的时间长度（以秒计）对应的数据相匹配。缺省情况下，这个属性的值为.5。

        Bool16  fDoReportHTTPConnectionAddress;		//qtssPrefsDoReportHTTPConnectionAddress 当客户端位于一个round-robin DNS的后面时，需要知道处理客户端请求的机器的IP地址。这个属性告知服务器在通过HTTP建立RTSP通道时，需要在HTTP GET请求中报告自己的IP地址。缺省情况下，这个属性的值为假（false）。
        Bool16  fAppendSrcAddrInTransport;		//qtssPrefsSrcAddrInTransport 如果这个属性设置为真（true），则服务器会将其源地址加入到传输数据包头部中。对于某些不必知道源地址的网络，这个属性是很必要的。缺省情况下，这个属性的值为假（false）。

        UInt32  fSmallWindowSizeInK;		//qtssPrefsSmallWindowSizeInK 在Reliable UDP协议中，这个属性指定的是用于低位率电影的窗口尺寸（以K字节计）。对于没有指定窗口尺寸的客户，服务器可以使用这个属性的值。缺省情况下，这个属性的值为24。
		UInt32	fMediumWindowSizeInK;		//qtssPrefsMediumWindowSizeInK 在Reliable UDP协议中，这个属性指定用于中等位率电影的窗口尺寸，以K字节计。对于没有指定窗口尺寸的客户端，服务器可以使用这个属性的值。缺省情况下，这个属性的值为48。
        UInt32  fLargeWindowSizeInK;		//qtssPrefsLargeWindowSizeInK 在Reliable UDP协议中，窗口尺寸（以K字节计）用于高位率的电影。对于没有指定窗口尺寸的客户端，服务器可以使用这个属性的值。缺省情况下，这个属性的值为64。
        UInt32  fWindowSizeThreshold;		//qtssPrefsWindowSizeThreshold 在Reliable UDP（可靠的UDP）中，如果客户没有指定自己的窗口尺寸，则当位率低于这个属性指定的值（以K位／秒计）时，服务器将会使用qtssPrefsSmallWindowSizeInK属性的值作为窗口的尺寸。缺省情况下，这个属性的值为200。
		UInt32	fWindowSizeMaxThreshold;
			//qtssPrefsWindowSizeMaxThreshold 这个属性指定用于测量Reliable UDP 带宽的窗口尺寸，以字节计。
				//如果位率大于qtssPrefsWindowSizeMaxThreshold属性指定的值，则窗口尺寸会被设置为qtssPrefsLargeWindowSizeInK属性值。
				//如果位率大于qtssPrefsWindowSizeThreshold属性值且小于或等于qtssPrefsWindowSizeMaxThreshold属性值，则窗口尺寸就会被设置为qtssPrefsMediumWindowSizeInK指定的值。
				//如果位率小于或等于qtssPrefsWindSizeThreshold属性的值，则窗口尺寸就被设定为qtssPrefsSmallWindowSizeInK属性的值。缺省情况下，这个属性的值为1000。


        UInt32  fMaxRetransDelayInMsec;		//qtssPrefsMaxRetransDelayInMsec 在Reliable UDP协议中，重新传输的数据包被发送的时刻和实际发送时刻之间的最大间隔时间，以秒计。这个值越低，传输就越顺畅，但是也会降低服务器的性能。缺省情况下，这个属性的值为500。
        Bool16  fIsAckLoggingEnabled;		//qtssPrefsAckLoggingEnabled 这个属性使服务器产生UDP应答和传输数据包的详细记录。缺省情况下，这个属性的值为假（false）。
        UInt32  fRTCPPollIntervalInMsec;		//qtssPrefsRTCPPollIntervalInMsec 这个属性已经不再被使用，选举已经不再是RTCP的功能了。
        UInt32  fRTCPSocketRcvBufSizeInK;		//qtssPrefsRTCPSockRcvBufSizeInK 用于接收RTCP数据包的UDP套接口的接收缓冲区尺寸。这个缓冲区需要足够大，以吸收RTCP应答时冲高的数据量。缺省情况下，这个属性的值为768。
        Bool16  fIsSlowStartEnabled;		//qtssPrefsReliableUDPSlowStart 将这个属性设置为真（true）将激活Reliable UDP的慢速启动特性。如果禁用UDP慢速启动，可能会因为对可用带宽估计错误而导致起始阶段丢包率冲高。启用UDP慢速启动则可能导致位率的过早衰减（也称为"打薄"）。缺省情况下，这个属性的值为真（true）。
        UInt32  fSendIntervalInMsec;		//qtssPrefsSendInterval 服务器在向客户端发送数据包前准备等待的最短间隔时间。以毫秒计。缺省情况下，这个属性的值为50。
        UInt32  fMaxSendAheadTimeInSecs;
        Bool16  fauto_delete_sdp_files;		//qtssPrefsAutoDeleteSPDFiles 这是一个不再使用的预置属性，保留这个属性是为了API的兼容性。
        QTSS_AuthScheme fAuthScheme;		//qtssPrefsAuthenticationScheme 将这个属性设置为您希望服务器使用的认证策略。当前支持的值有basic（基本），digest（摘要），和none（无）。缺省情况下，这个属性的值为digest。
        UInt32  fsdp_file_delete_interval_seconds; //qtssPrefsDeleteSDPFilesInterval 服务器检查SDP文件的间隔时间，以秒计。对这个属性所做的修改在当前间隔结束时生效。缺省情况下，这个属性的值为10。服务器也维护一个内部的间隔时间，值为1。
        Bool16  fAutoStart;		//qtssPrefsAutoStart 这个属性已经过时，应该总是设定为假（false）。
        Bool16  fReliableUDP;		//qtssPrefsReliableUDP 如果这个属性设置为真（true），则服务器将可以应客户的请求使用Reliable UDP传输协议。缺省情况下，这个属性的值为真（true）。
        Bool16  fReliableUDPPrintfs;		//qtssPrefsReliableUDPPrintfs 当这个属性设置为真（true）时，服务器在断开客户连接时将Reliable UDP的统计信息打印到stdout上。这个属性工作的前提是运行服务器时必须使用-d的命令行选项。统计信息包括URL，最大的拥挤窗口，最小的拥挤窗口，最大，最小，以及平均的RTT，被跳过的帧的数目，以及被丢弃的延迟数据包的数目。缺省情况下，这个属性的值为假（ false）。
        Bool16  fEnableRTSPErrMsg;		//qtssPrefsEnableRTSPErrorMessage 如果这个属性设置为真（true），则服务器会为报告的RTSP错误附加一个内容字符串，描述错误信息。缺省情况下，这个属性的值为假（false）。
        Bool16  fEnableRTSPDebugPrintfs;		//qtssPrefsEnableRTSPDebugPrintfs 当这个属性设置为真（true）时，服务器会把进入的RTSP请求和输出的RTSP响应打印到stdout上。这个属性工作的前提是运行服务器时必须使用-d的命令行选项。缺省情况下，这个属性的值为假（ false）。
        Bool16  fEnableRTSPServerInfo;		//qtssPrefsEnableRTSPServerInfo 如果这个属性设置为真（true），则服务器会将服务器信息添加到RTSP数据包报头。这些信息包括服务器的平台，版本，以及连编（build）号。缺省情况下，这个属性的值为真（true）。
        UInt32  fNumThreads;		//qtssPrefsRunNumThreads 如果这个属性值为非零，则服务器将创建指定数量的线程，来处理RTSP和RTP流。否则，服务器会为每个处理器创建一个线程来处理RTSP和RTP流。缺省情况下，这个属性的值为0。
        Bool16  fEnableMonitorStatsFile;				//qtssPrefsEnableMonitorStatsFile 当这个属性设置为真（true）时，服务器会把服务器统计信息写入到监控文件中，该文件可由外部的监控程序读取。缺省情况下，这个属性的值为假（false）。
        UInt32  fStatsFileIntervalSeconds;		//qtssPrefsMonitorStatsFileIntervalSec 服务器将统计信息写入监控文件的间隔时间。缺省情况下，这个属性的值为10。
	
		Float32	fOverbufferRate;		//qtssPrefsOverbufferRate 服务器用这个属性值计算过度缓冲率。这个属性值需要和数据率相乘。缺省情况下，这个属性的值为2.0。
		
        Bool16  fEnablePacketHeaderPrintfs;		//qtssPrefsEnablePacketHeaderPrintfs 如果这个属性设置为真（true），则服务器会将输出的RTP和RTCP数据包的头信息打印到stdout上。这个属性工作的前提是运行服务器时必须使用-d的命令行选项。可用的打印选项请参见qtssPrefsPacketHeaderPrintfOptions属性。缺省情况下，这个属性的值为假 false。
        UInt32  fPacketHeaderPrintfOptions;		//qtssPrefsPacketHeaderPrintfOptions 当qtssPrefsEnabledPacketHeaderPrintfs属性为真（true）时，这个属性用于标识应该打印那些数据包报头信息。属性值的选项用分号（;）来分割。缺省情况下，这个属性的值为所有可用的选项，即rtp;rr;sr;app;ack;，表示打印RTP包（rtp），RTCP接收方报告（rr），RTCP发送方报告（sr），RTCP应用程序包（app），以及Reliable UDP RTP应答包（ack）。
        Bool16  fCloseLogsOnWrite;			//qtssPrefsCloseLogsOnWrite 如果这个属性设定为真（true），则服务器在每次写入数据后会关闭日志文件。 缺省情况下，这个属性的值为假（false）。
        
        Bool16 fDisableThinning;
        enum //fPacketHeaderPrintfOptions
        {
            kRTPALL = 1 << 0,
            kRTCPSR = 1 << 1,
            kRTCPRR = 1 << 2,
            kRTCPAPP = 1<< 3,
            kRTCPACK = 1<< 4
        };
        
        enum
        {
            kAllowMultipleValues = 1,
            kDontAllowMultipleValues = 0
        };
        
        struct PrefInfo
        {
            UInt32  fAllowMultipleValues;
            char*   fDefaultValue;
            char**  fAdditionalDefVals; // For prefs with multiple default values
        };
            
        void SetupAttributes();
        void UpdateAuthScheme();
        void UpdatePrintfOptions();
        //
        // Returns the string preference with the specified ID. If there
        // was any problem, this will return an empty string.
        char* GetStringPref(QTSS_AttributeID inAttrID);
        
        static QTSSAttrInfoDict::AttrInfo   sAttributes[];
        static PrefInfo sPrefInfo[];
        
        // Prefs that have multiple default values (rtsp_ports) have
        // to be dealt with specially
        static char*    sAdditionalDefaultPorts[];
        
        // Player prefs
        static char*    sRTP_Header_Players[];
        static char*    sAdjust_Bandwidth_Players[];
       
};
#endif //__QTSSPREFS_H__
