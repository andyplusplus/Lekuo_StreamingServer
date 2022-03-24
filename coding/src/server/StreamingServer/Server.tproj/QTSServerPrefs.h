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

        UInt32      fRTSPTimeoutInSecs;		//qtssPrefsRTSPTimeout ��������֪�ͻ��ڶϿ�����RTSP�ͻ�֮ǰ׼���ȴ���ʱ�䣬����ơ�ȱʡ����£�������Ե�ֵΪ0��
        char        fRTSPTimeoutBuf[20];
        StrPtrLen   fRTSPTimeoutString;
        UInt32      fRealRTSPTimeoutInSecs;	//qtssPrefsRealRTSPTimeout �������ڶϿ�����RTSP�ͻ�֮ǰ��ʵ�ʵȴ�ʱ�䣬����ơ�ÿ�η������ӿͻ��˽��յ��µ�RTSP����ʱ�������ʱ�����ᱻ���á����ֵΪ����ʾ��Զ����ʱ��ȱʡ����£�������Ե�ֵΪ180��
        UInt32      fRTPTimeoutInSecs;		//qtssPrefsRTPTimeout �������ڶϿ����е�RTP����֮ǰ׼���ȴ���ʱ�䣬����ơ�ÿ�η������ӿͻ��˽��յ��µ�RTCP״̬��ʱ�������ʱ�����ᱻ���á����ֵΪ����ʾ��Զ����ʱ��ȱʡ����£�������Ե�ֵΪ120��
        
        SInt32  fMaximumConnections;		//qtssPrefsMaximumConnections �����������RTP���ӵ���󲢷��������ֵΪ1��ʾ����������������ȱʡ����£�������Ե�ֵΪ1000��
        SInt32  fMaxBandwidthInKBits;		//qtssPrefsMaximumBandwidth ���������ʹ�õ���������Kb�ơ����������ʹ�õĴ��������ֵ������������ý�������¿ͻ�����453�ŵ�RTSP����"û���㹻�Ĵ���"�����ֵΪ1��ʾ���������ʹ�����޴���ȱʡ����£�������Ե�ֵΪ102400��
        
        Bool16  fBreakOnAssert;		//qtssPrefsBreakOnAssert ����������ֵΪ�棬���������������ԣ�assert������ʱ������������ȱʡ����£�������Ե�ֵΪ�٣�false����
        Bool16  fAutoRestart;		//qtssPrefsAutoRestart ����������ֵΪ�棬��������ڱ�������Զ�����ȱʡ����£�������Ե�ֵΪ�棨true����
        UInt32  fTBUpdateTimeInSecs;		//qtssPrefsTotalBytesUpdate ���θ��·��������ֽ������͵�ǰ����ͳ��ֵ֮��ļ��ʱ�䣬����ơ�ȱʡ����£�������Ե�ֵΪ1��
        UInt32  fABUpdateTimeInSecs;		//qtssPrefsAvgBandwidthUpdate ���μ��������ƽ������ļ��ʱ�䣬����ơ�ȱʡ����£�������Ե�ֵΪ60��
        UInt32  fSafePlayDurationInSecs;	//qtssPrefsSafePlayDuration ��������������Լ�ʹ�õĴ����������������ͨ������ƽ������������ͼ�Ͽ����½������ӵĿͻ���ֱ��ƽ�������½����ɽ��ܵ�ˮƽ��Ȼ��������ͻ��������ӵ�ʱ����������ָ���������������򲻻�Ͽ������ӡ�����������ֵ�趨Ϊ�����������������Ͽ��ͻ����ӡ�ȱʡ����£�������Ե�ֵΪ600��
        
        UInt32  fErrorRollIntervalInDays;	//qtssPrefsErrorRollInterval ����������־�ļ��ļ��ʱ�䣬����ơ�ȱʡ����£�������Ե�ֵΪ0����ʾ������������־�ļ���
        UInt32  fErrorLogBytes;		//qtssPrefsMaxErrorLogSize ������־�����ߴ磬���ֽ��㡣ֵΪ����ʾ������û�жԴ����ޡ�ȱʡ����£�������Ե�ֵΪ256000��
        UInt32  fErrorLogVerbosity;		//qtssPrefsMaxErrorLogSize ������־�����ߴ磬���ֽ��㡣ֵΪ����ʾ������û�жԴ����ޡ�ȱʡ����£�������Ե�ֵΪ256000��
        Bool16  fScreenLoggingEnabled;	//qtssPrefsScreenLogging ���������Ա�����Ϊ�棨������ÿ�д�����־���ᱻд�뵽�ն˴����С���ע�⣬���ϣ��������Щ������־������������ͨ������������������ͨ��true-d ѡ��ʹ֮����ǰ̨����ģʽ��ȱʡ����£�������Ե�ֵΪ�棨 true����
        Bool16  fErrorLogEnabled;		//qtssPrefsErrorLogEnabled �������������Ϊ�棨true�����Լ��������־���ܡ�ȱʡ����£�������Ե�ֵΪ�棨true����
        
        SInt32  fDropAllPacketsTimeInMsec;		//qtssPrefsDropVideoAllPacketsDelayInMsec �����Ƶ���ݰ����������ָ����ʱ���ڣ��Ժ���ƣ�û�б����ͳ�ȥ����������ͻὫ������������������ڷ������Ĵ򱡣�thinning���㷨�С�ȱʡ����£�������Ե�ֵΪ1750��
        SInt32  fDropAllVideoPacketsTimeInMsec;		//qtssPrefsDropVideoAllPacketsDelayInMsec �����Ƶ���ݰ����������ָ����ʱ���ڣ��Ժ���ƣ�û�б����ͳ�ȥ����������ͻὫ������������������ڷ������Ĵ򱡣�thinning���㷨�С�ȱʡ����£�������Ե�ֵΪ1750��
        SInt32  fThinAllTheWayTimeInMsec;		//qtssPrefsStartThinningDelayInMsec ������ݰ����������ָ����ʱ���û�е�����������ʼ�򱡡�ȱʡ����£�������Ե�ֵΪ0��
        SInt32  fAlwaysThinTimeInMsec;		//qtssPrefsAlwaysThinDelayInMsec ������ݰ��ӳٵ�ʱ��ﵽ�������ָ���ĺ��������������ͻῪʼ�򱡡���������Ƿ��������㷨��һ���֡�ȱʡ����£�������Ե�ֵΪ750��
        SInt32  fStartThinningTimeInMsec;		//qtssPrefsStartThickingDelayInMsec ������ݰ��ӳٵ�ʱ��ﵽ�������ָ���ĺ��������������ͻῪʼ����thicking������������Ƿ��������㷨��һ���֡�ȱʡ����£�������Ե�ֵΪ250��
        SInt32  fStartThickingTimeInMsec;	    //qtssPrefsStartThickingDelayInMsec ������ݰ��ӳٵ�ʱ��ﵽ�������ָ���ĺ��������������ͻῪʼ����thicking������������Ƿ��������㷨��һ���֡�ȱʡ����£�������Ե�ֵΪ250��
        SInt32  fThickAllTheWayTimeInMsec;		//qtssPrefsThickAllTheWayDelayInMsec ������ݰ����������ָ����ʱ���ڷ�������ֵ��ʾ��ǰ��������ý���Ʒ����ȫ�ָ�����������Ƿ��������㷨��һ���֡�ȱʡ����£�������Ե�ֵΪ-2000��
        UInt32  fQualityCheckIntervalInMsec;		//qtssPrefsStartQualityCheckIntervalInMsec ����������Ƿ�򱡲��ڱ�Ҫʱ���е����ļ��ʱ�䣬�Ժ���ơ���������Ƿ��������㷨��һ���֡�ȱʡ����£�������Ե�ֵΪ1000��

        UInt32  fMinTCPBufferSizeInBytes;		//qtssPrefsMinTCPBufferSizeInBytes TCP�׽ӿڷ��ͻ��������Ա��趨����С�ߴ磬���ֽڼơ�ȱʡ����£�������Ե�ֵΪ8192��
        UInt32  fMaxTCPBufferSizeInBytes;		//qtssPrefsMaxTCPBufferSizeInBytes TCP�׽ӿڷ��ͻ��������Ա��趨�����ߴ磬���ֽڼơ�ȱʡ����£�������Ե�ֵΪ200000��
        Float32 fTCPSecondsToBuffer;			//qtssPrefsTCPSecondsToBuffer ��TCP֮�Ͻ�����ý�����ʱ�����ͻ������ĳߴ����ݵ�Ӱ��λ�ʽ��е���������������ݵ�Ӱ��λ�ʶ�TCP���ͻ����������趨��ʹ�����������ָ����ʱ�䳤�ȣ�����ƣ���Ӧ��������ƥ�䡣ȱʡ����£�������Ե�ֵΪ.5��

        Bool16  fDoReportHTTPConnectionAddress;		//qtssPrefsDoReportHTTPConnectionAddress ���ͻ���λ��һ��round-robin DNS�ĺ���ʱ����Ҫ֪������ͻ�������Ļ�����IP��ַ��������Ը�֪��������ͨ��HTTP����RTSPͨ��ʱ����Ҫ��HTTP GET�����б����Լ���IP��ַ��ȱʡ����£�������Ե�ֵΪ�٣�false����
        Bool16  fAppendSrcAddrInTransport;		//qtssPrefsSrcAddrInTransport ��������������Ϊ�棨true������������Ὣ��Դ��ַ���뵽�������ݰ�ͷ���С�����ĳЩ����֪��Դ��ַ�����磬��������Ǻܱ�Ҫ�ġ�ȱʡ����£�������Ե�ֵΪ�٣�false����

        UInt32  fSmallWindowSizeInK;		//qtssPrefsSmallWindowSizeInK ��Reliable UDPЭ���У��������ָ���������ڵ�λ�ʵ�Ӱ�Ĵ��ڳߴ磨��K�ֽڼƣ�������û��ָ�����ڳߴ�Ŀͻ�������������ʹ��������Ե�ֵ��ȱʡ����£�������Ե�ֵΪ24��
		UInt32	fMediumWindowSizeInK;		//qtssPrefsMediumWindowSizeInK ��Reliable UDPЭ���У��������ָ�������е�λ�ʵ�Ӱ�Ĵ��ڳߴ磬��K�ֽڼơ�����û��ָ�����ڳߴ�Ŀͻ��ˣ�����������ʹ��������Ե�ֵ��ȱʡ����£�������Ե�ֵΪ48��
        UInt32  fLargeWindowSizeInK;		//qtssPrefsLargeWindowSizeInK ��Reliable UDPЭ���У����ڳߴ磨��K�ֽڼƣ����ڸ�λ�ʵĵ�Ӱ������û��ָ�����ڳߴ�Ŀͻ��ˣ�����������ʹ��������Ե�ֵ��ȱʡ����£�������Ե�ֵΪ64��
        UInt32  fWindowSizeThreshold;		//qtssPrefsWindowSizeThreshold ��Reliable UDP���ɿ���UDP���У�����ͻ�û��ָ���Լ��Ĵ��ڳߴ磬��λ�ʵ����������ָ����ֵ����Kλ����ƣ�ʱ������������ʹ��qtssPrefsSmallWindowSizeInK���Ե�ֵ��Ϊ���ڵĳߴ硣ȱʡ����£�������Ե�ֵΪ200��
		UInt32	fWindowSizeMaxThreshold;
			//qtssPrefsWindowSizeMaxThreshold �������ָ�����ڲ���Reliable UDP ����Ĵ��ڳߴ磬���ֽڼơ�
				//���λ�ʴ���qtssPrefsWindowSizeMaxThreshold����ָ����ֵ���򴰿ڳߴ�ᱻ����ΪqtssPrefsLargeWindowSizeInK����ֵ��
				//���λ�ʴ���qtssPrefsWindowSizeThreshold����ֵ��С�ڻ����qtssPrefsWindowSizeMaxThreshold����ֵ���򴰿ڳߴ�ͻᱻ����ΪqtssPrefsMediumWindowSizeInKָ����ֵ��
				//���λ��С�ڻ����qtssPrefsWindSizeThreshold���Ե�ֵ���򴰿ڳߴ�ͱ��趨ΪqtssPrefsSmallWindowSizeInK���Ե�ֵ��ȱʡ����£�������Ե�ֵΪ1000��


        UInt32  fMaxRetransDelayInMsec;		//qtssPrefsMaxRetransDelayInMsec ��Reliable UDPЭ���У����´�������ݰ������͵�ʱ�̺�ʵ�ʷ���ʱ��֮��������ʱ�䣬����ơ����ֵԽ�ͣ������Խ˳��������Ҳ�ή�ͷ����������ܡ�ȱʡ����£�������Ե�ֵΪ500��
        Bool16  fIsAckLoggingEnabled;		//qtssPrefsAckLoggingEnabled �������ʹ����������UDPӦ��ʹ������ݰ�����ϸ��¼��ȱʡ����£�������Ե�ֵΪ�٣�false����
        UInt32  fRTCPPollIntervalInMsec;		//qtssPrefsRTCPPollIntervalInMsec ��������Ѿ����ٱ�ʹ�ã�ѡ���Ѿ�������RTCP�Ĺ����ˡ�
        UInt32  fRTCPSocketRcvBufSizeInK;		//qtssPrefsRTCPSockRcvBufSizeInK ���ڽ���RTCP���ݰ���UDP�׽ӿڵĽ��ջ������ߴ硣�����������Ҫ�㹻��������RTCPӦ��ʱ��ߵ���������ȱʡ����£�������Ե�ֵΪ768��
        Bool16  fIsSlowStartEnabled;		//qtssPrefsReliableUDPSlowStart �������������Ϊ�棨true��������Reliable UDP�������������ԡ��������UDP�������������ܻ���Ϊ�Կ��ô�����ƴ����������ʼ�׶ζ����ʳ�ߡ�����UDP������������ܵ���λ�ʵĹ���˥����Ҳ��Ϊ"��"����ȱʡ����£�������Ե�ֵΪ�棨true����
        UInt32  fSendIntervalInMsec;		//qtssPrefsSendInterval ����������ͻ��˷������ݰ�ǰ׼���ȴ�����̼��ʱ�䡣�Ժ���ơ�ȱʡ����£�������Ե�ֵΪ50��
        UInt32  fMaxSendAheadTimeInSecs;
        Bool16  fauto_delete_sdp_files;		//qtssPrefsAutoDeleteSPDFiles ����һ������ʹ�õ�Ԥ�����ԣ��������������Ϊ��API�ļ����ԡ�
        QTSS_AuthScheme fAuthScheme;		//qtssPrefsAuthenticationScheme �������������Ϊ��ϣ��������ʹ�õ���֤���ԡ���ǰ֧�ֵ�ֵ��basic����������digest��ժҪ������none���ޣ���ȱʡ����£�������Ե�ֵΪdigest��
        UInt32  fsdp_file_delete_interval_seconds; //qtssPrefsDeleteSDPFilesInterval ���������SDP�ļ��ļ��ʱ�䣬����ơ�����������������޸��ڵ�ǰ�������ʱ��Ч��ȱʡ����£�������Ե�ֵΪ10��������Ҳά��һ���ڲ��ļ��ʱ�䣬ֵΪ1��
        Bool16  fAutoStart;		//qtssPrefsAutoStart ��������Ѿ���ʱ��Ӧ�������趨Ϊ�٣�false����
        Bool16  fReliableUDP;		//qtssPrefsReliableUDP ��������������Ϊ�棨true�����������������Ӧ�ͻ�������ʹ��Reliable UDP����Э�顣ȱʡ����£�������Ե�ֵΪ�棨true����
        Bool16  fReliableUDPPrintfs;		//qtssPrefsReliableUDPPrintfs �������������Ϊ�棨true��ʱ���������ڶϿ��ͻ�����ʱ��Reliable UDP��ͳ����Ϣ��ӡ��stdout�ϡ�������Թ�����ǰ�������з�����ʱ����ʹ��-d��������ѡ�ͳ����Ϣ����URL������ӵ�����ڣ���С��ӵ�����ڣ������С���Լ�ƽ����RTT����������֡����Ŀ���Լ����������ӳ����ݰ�����Ŀ��ȱʡ����£�������Ե�ֵΪ�٣� false����
        Bool16  fEnableRTSPErrMsg;		//qtssPrefsEnableRTSPErrorMessage ��������������Ϊ�棨true�������������Ϊ�����RTSP���󸽼�һ�������ַ���������������Ϣ��ȱʡ����£�������Ե�ֵΪ�٣�false����
        Bool16  fEnableRTSPDebugPrintfs;		//qtssPrefsEnableRTSPDebugPrintfs �������������Ϊ�棨true��ʱ����������ѽ����RTSP����������RTSP��Ӧ��ӡ��stdout�ϡ�������Թ�����ǰ�������з�����ʱ����ʹ��-d��������ѡ�ȱʡ����£�������Ե�ֵΪ�٣� false����
        Bool16  fEnableRTSPServerInfo;		//qtssPrefsEnableRTSPServerInfo ��������������Ϊ�棨true������������Ὣ��������Ϣ��ӵ�RTSP���ݰ���ͷ����Щ��Ϣ������������ƽ̨���汾���Լ����ࣨbuild���š�ȱʡ����£�������Ե�ֵΪ�棨true����
        UInt32  fNumThreads;		//qtssPrefsRunNumThreads ����������ֵΪ���㣬�������������ָ���������̣߳�������RTSP��RTP�������򣬷�������Ϊÿ������������һ���߳�������RTSP��RTP����ȱʡ����£�������Ե�ֵΪ0��
        Bool16  fEnableMonitorStatsFile;				//qtssPrefsEnableMonitorStatsFile �������������Ϊ�棨true��ʱ����������ѷ�����ͳ����Ϣд�뵽����ļ��У����ļ������ⲿ�ļ�س����ȡ��ȱʡ����£�������Ե�ֵΪ�٣�false����
        UInt32  fStatsFileIntervalSeconds;		//qtssPrefsMonitorStatsFileIntervalSec ��������ͳ����Ϣд�����ļ��ļ��ʱ�䡣ȱʡ����£�������Ե�ֵΪ10��
	
		Float32	fOverbufferRate;		//qtssPrefsOverbufferRate ���������������ֵ������Ȼ����ʡ��������ֵ��Ҫ����������ˡ�ȱʡ����£�������Ե�ֵΪ2.0��
		
        Bool16  fEnablePacketHeaderPrintfs;		//qtssPrefsEnablePacketHeaderPrintfs ��������������Ϊ�棨true������������Ὣ�����RTP��RTCP���ݰ���ͷ��Ϣ��ӡ��stdout�ϡ�������Թ�����ǰ�������з�����ʱ����ʹ��-d��������ѡ����õĴ�ӡѡ����μ�qtssPrefsPacketHeaderPrintfOptions���ԡ�ȱʡ����£�������Ե�ֵΪ�� false��
        UInt32  fPacketHeaderPrintfOptions;		//qtssPrefsPacketHeaderPrintfOptions ��qtssPrefsEnabledPacketHeaderPrintfs����Ϊ�棨true��ʱ������������ڱ�ʶӦ�ô�ӡ��Щ���ݰ���ͷ��Ϣ������ֵ��ѡ���÷ֺţ�;�����ָȱʡ����£�������Ե�ֵΪ���п��õ�ѡ���rtp;rr;sr;app;ack;����ʾ��ӡRTP����rtp����RTCP���շ����棨rr����RTCP���ͷ����棨sr����RTCPӦ�ó������app�����Լ�Reliable UDP RTPӦ�����ack����
        Bool16  fCloseLogsOnWrite;			//qtssPrefsCloseLogsOnWrite �����������趨Ϊ�棨true�������������ÿ��д�����ݺ��ر���־�ļ��� ȱʡ����£�������Ե�ֵΪ�٣�false����
        
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
