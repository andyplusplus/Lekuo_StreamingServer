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

/*  File:       playlist_broadcaster.cpp

    Contains:   

*/

#include "playlist_broadcaster.h"
#include "OS.h"
#include "OSThread.h"
#include "BroadcasterSession.h"

#include <string.h>

//初始化成员变量
QTFileBroadcaster::QTFileBroadcaster() 
{
    fRTPFilePtr = NULL;
    fMovieSDPParser = NULL;
    fBasePort = 0;
    fDebug = false;
    fDeepDebug = false;
    fQuitImmediatePtr = NULL;
// transmit time trackers
    fLastTransmitTime = 0.0;
    
    fStreamStartTime  = 0;
    fMovieStartTime  = 0;
    fMovieEndTime = 0;
    fMovieIntervalTime = 0;
    fMovieTimeDiffMilli = 0;
    
    fMovieStart = false;
    fSendTimeOffset = 0.0;
    fMovieDuration = 0.0;
    fMovieTracks = 0;
    fMappedMovieTracks = 0;
    fNumMoviesPlayed = 0;
    fPlay = true;
    fSend = true;
    fBroadcastDefPtr = NULL;
    // current movie parameters parsed from SDP
    ::strcpy(fCurrentMovieName, "-");
    ::strcpy(fCurrentMovieCopyright, "-");
    ::strcpy(fCurrentMovieComment, "-");
    ::strcpy(fCurrentMovieAuthor, "-");
    ::strcpy(fCurrentMovieArtist, "-");
    ::strcpy(fCurrentMovieAlbum, "-");
}

//析构函数, 关闭日志文件
QTFileBroadcaster::~QTFileBroadcaster()
{ 
    LogFileClose(); 
    if (fRTPFilePtr != NULL)
    {   delete fRTPFilePtr; 
    } 
}

//把broadcastDefPtr以及SDP的相关参数赋值给QTFileBroadcaster
int QTFileBroadcaster::SetUp(PLBroadcastDef *broadcastDefPtr, bool *quitImmediatePtr)
{
    int result = -1;
    int numStreams = 0;
    fQuitImmediatePtr = quitImmediatePtr;
    PlayListUtils::Initialize();
    do 
    {
        if (! broadcastDefPtr) 
        { result = eParam;          break; };
            
        if (!broadcastDefPtr->mSDPFile || 0 == broadcastDefPtr->mSDPFile[0] ) 
        { result = eSDPFileInvalidName; break; };
        
        int nameLen = strlen(broadcastDefPtr->mSDPFile);
        if (nameLen > 255)  
        { result = eSDPFileInvalidName; break; };   
                
        if (0 ==  nameLen)  
        { result = eSDPFileInvalidName; break; };   

        if (broadcastDefPtr->mTheSession)
        {   if (!broadcastDefPtr->mDestSDPFile)
            { result = eNetworkSDPFileNameInvalidMissing; break; };
                
            if (!broadcastDefPtr->mDestSDPFile[0])
            { result = eNetworkSDPFileNameInvalidMissing; break; };
                
            if (!::strcmp(broadcastDefPtr->mDestSDPFile, "no_name"))
            { result = eNetworkSDPFileNameInvalidMissing; break; }; 
                        
//          if ('/' == broadcastDefPtr->mDestSDPFile[0])
//          { result = eNetworkSDPFileNameInvalidBadPath; break; };
        }
//1. 读入SDP文件        
        result = fStreamSDPParser.ReadSDP(broadcastDefPtr->mSDPFile);
            
        if (result != 0)
        {   if (result < 0) { result = eSDPFileNotFound;    break; };
            if (result > 0) { result = eSDPFileInvalid;     break; };
        }   
        

        fBroadcastDefPtr = broadcastDefPtr;
        if (broadcastDefPtr->mTheSession == NULL)
        {   if (!broadcastDefPtr->mBasePort) { result = eSDPFileNoPorts; break; };
            
            int portLen = strlen(broadcastDefPtr->mBasePort);
            if (0 ==  portLen) { result = eDescriptionInvalidDestPort;      break; };   
            if (portLen > 5  )  { result = eDescriptionInvalidDestPort; break; };

            int basePort = atoi(broadcastDefPtr->mBasePort);
            if  ( basePort > 65531 ) { result = eDescriptionInvalidDestPort;                break; };
            if  ( basePort < 5004 )  { result = eDescriptionInvalidDestPort;                break; };

        }
//2. 设置相关参数            
        numStreams = fStreamSDPParser.GetNumTracks();       
        if (numStreams == 0) { result = eSDPFileNoMedia;    break; };
        
        UDPSocketPair *socketArrayPtr = fSocketlist.SetSize(numStreams);
        if (socketArrayPtr == NULL) { result = eMem;        break; };
   //2.1 以下66行代码把SDP中的端口绑定到流的端口上
        // Bind SDP file defined ports to active stream ports
        { 
            UInt16          streamIndex = 0;
            UInt16          rtpPort = 0;
            UInt16          rtcpPort = 0;
            TypeMap*        mediaTypePtr;
            char            sdpIPAddress[32];
            SimpleString* ipStringPtr = fStreamSDPParser.GetIPString();
            
            if ( (NULL == ipStringPtr) || (ipStringPtr->fLen >= 32) )
            {   
                result = eSDPFileInvalid;               
                break; 
            }
                
            memcpy(sdpIPAddress,ipStringPtr->fTheString,ipStringPtr->fLen);
            sdpIPAddress[ipStringPtr->fLen] = '\0';

		//2.1.1 对每个socket进行绑定操作
            UDPSocketPair *aSocketPair = fSocketlist.Begin();
            Bool16 setupUDP = true;
			while (aSocketPair != NULL)
            {   
                mediaTypePtr = fStreamSDPParser.fSDPMediaList.SetPos(streamIndex);
                
                if (mediaTypePtr == NULL) 
                {   result = eSDPFileInvalid;   
                    break;  
                }
                                    
                if (broadcastDefPtr->mTheSession != NULL)
                {
                    mediaTypePtr->fPort = broadcastDefPtr->mTheSession->GetStreamDestPort(streamIndex);
                    //qtss_printf("QTFileBroadcaster::SetUp streamIndex=%u port=%d\n",streamIndex,mediaTypePtr->fPort);
                
                    if (BroadcasterSession::kTCPTransportType == broadcastDefPtr->mTheSession->GetTransportType())
                    {   aSocketPair->SetRTSPSession(broadcastDefPtr->mTheSession, (UInt8) streamIndex * 2);
                        setupUDP = false;
                    }
                    else
                    {   setupUDP = true;
                    }
                }
                //1.取出TTL, RTP, RTCP端口等参数
                if (setupUDP)
                {
                    SInt16 ttl = (SInt16) atoi(broadcastDefPtr->mTTL);
                    if  ( ( ttl > 255 ) || ( ttl < 1 ) )
                    {   result = eSDPFileInvalidTTL;    break; 
                    };

                    if (mediaTypePtr->fPort == 0) 
                    {  
                        result = eSDPFileInvalidPort;   
                        break;  
                    }
    
                    rtpPort = mediaTypePtr->fPort;
                    rtcpPort = rtpPort + 1;
                    
                 //2.绑定本地地址以及完程地址 
                    result = aSocketPair->OpenAndBind(rtpPort,rtcpPort,sdpIPAddress);
                    if (result != 0) 
                    { 
                        result = eFailedBind; 
                        break; 
                    }
                 //3.如果是组播地址, 设置组播选项
                    (void) aSocketPair->SetMultiCastOptions(ttl);

                }
                
                aSocketPair = fSocketlist.Next();
                streamIndex++;
            }
            
            if (result != 0) 
                break;
        }

            
        MediaStream *mediaArrayPtr = fMediaStreamList.SetSize(numStreams);
        if (mediaArrayPtr == NULL) 
        { 
            result = eMem; 
            break; 
        }
        //设置每个媒体流的属性
        for (int i = 0; i < numStreams; i ++)
        {   UDPSocketPair   *socketPairPtr = fSocketlist.SetPos(i);
            MediaStream     *mediaStreamPtr = fMediaStreamList.SetPos(i);
            TypeMap         *streamMediaTypePtr = fStreamSDPParser.fSDPMediaList.SetPos(i);
            
            if (socketPairPtr && mediaStreamPtr && streamMediaTypePtr)
            {
                mediaStreamPtr->fData.fSocketPair = socketPairPtr;
                streamMediaTypePtr->fMediaStreamPtr = mediaStreamPtr;
                mediaStreamPtr->fData.fStreamMediaTypePtr = streamMediaTypePtr;
            }
            else 
            { 
                result = eMem;
                break; 
            }
        }
        
        //设置媒体流的SSRC, 启动时间等
        fMediaStreamList.SetUpStreamSSRCs();
        fStreamStartTime = PlayListUtils::Milliseconds(); 
        fMediaStreamList.StreamStarted(fStreamStartTime);
        result = 0;
        LogFileOpen();
        
    } while (false);
    
    return result;
}

//返回和 "id" 相同的PayLoad
//该函数由CompareRTPMaps(...)调用
PayLoad * QTFileBroadcaster::FindPayLoad(short id, ArrayList<PayLoad> *PayLoadListPtr)
{
    PayLoad *thePayLoadPtr = PayLoadListPtr->Begin();
    while (thePayLoadPtr)
    {
        if (thePayLoadPtr->payloadID == id) 
            break;
            
        thePayLoadPtr = PayLoadListPtr->Next();
    }
    return thePayLoadPtr;
}

//确定movieMediaTypePtr以及streamMediaTypePtr是否都包含payloadID为moviePayLoadID的PayLoad
bool QTFileBroadcaster::CompareRTPMaps(TypeMap *movieMediaTypePtr, TypeMap *streamMediaTypePtr, short moviePayLoadID)
{
    bool found = false;
        
    do
    {
		//从movieMediaTypePtr找到ID为moviePayLoadID的PayLoad
        PayLoad *moviePayLoadPtr = FindPayLoad(moviePayLoadID, &movieMediaTypePtr->fPayLoadTypes);
        if (!moviePayLoadPtr) break;
        
        PayLoad *streamPayLoadPtr = streamMediaTypePtr->fPayLoadTypes.Begin();
        //查找streamMediaTypePtr是否有这样的PayLoad
		while (streamPayLoadPtr)
        {
            if (moviePayLoadPtr->timeScale == streamPayLoadPtr->timeScale) 
            {   found =  SimpleParser::Compare(&moviePayLoadPtr->payLoadString,&streamPayLoadPtr->payLoadString, false );
            }
            
            if (found) 
            {   moviePayLoadPtr->payloadID = streamPayLoadPtr->payloadID; // map movie ID to match stream id
                break;
            }
            streamPayLoadPtr = streamMediaTypePtr->fPayLoadTypes.Next();
        }
            
        
    } while (false);
    return found;
}

//确定movieMediaTypePtr是否包含streamMediaTypePtr的所有媒体类型
bool QTFileBroadcaster::CompareMediaTypes(TypeMap *movieMediaTypePtr, TypeMap *streamMediaTypePtr)
{
    bool found = false;
    
    found = SimpleParser::Compare(&movieMediaTypePtr->fTheTypeStr,&streamMediaTypePtr->fTheTypeStr, false);
    if (found)
    {   
        found = false; 
        
        short *movieIDPtr = movieMediaTypePtr->fPayLoads.Begin();
        while (movieIDPtr && !found)
        {
            short *streamIDPtr = streamMediaTypePtr->fPayLoads.Begin();
            while (streamIDPtr && !found)
            {
                
                if (*movieIDPtr >= 96)		//???为什么是96
                    found = CompareRTPMaps(movieMediaTypePtr, streamMediaTypePtr, *movieIDPtr);
                else
                    found = (*streamIDPtr == *movieIDPtr) ? true : false;
            
                streamIDPtr = streamMediaTypePtr->fPayLoads.Next();     
            }

            movieIDPtr = movieMediaTypePtr->fPayLoads.Next();
        }
    }
    
    
    return found;
}

// 1. N*N比较fMovieSDPParser以及fStreamSDPParser的节目表
// 2. 比较媒体类型, 如果相同, 则movieMediaTypePtr->fMediaStreamPtr = streamMediaTypePtr->fMediaStreamPtr;
// 3. 返回匹配的结果数
int QTFileBroadcaster::MapMovieToStream()
{
    int             result = -1;
    bool            matches = false;
    ArrayList<bool> map;
    bool            *isMappedPtr;
    int             masterPos = 0;  
    int             mappedTracks = 0;
    
    map.SetSize(fStreamSDPParser.fSDPMediaList.Size()); 
    
    isMappedPtr = map.Begin();
    //1.初始化map中的元素为False
    while (isMappedPtr)
    {   *isMappedPtr = false;
        isMappedPtr = map.Next();
    }
    
	//2.对SDP中对应所有媒体列表, 执行以下的递归调用
    TypeMap *movieMediaTypePtr = fMovieSDPParser->fSDPMediaList.Begin();
    
    while (movieMediaTypePtr)
    {//
        TypeMap *streamMediaTypePtr = fStreamSDPParser.fSDPMediaList.Begin();
        
        while (streamMediaTypePtr)
        {	
			//
            matches = CompareMediaTypes(movieMediaTypePtr, streamMediaTypePtr);
            
            if (matches)
            {
                masterPos = fStreamSDPParser.fSDPMediaList.GetPos();
                isMappedPtr = map.SetPos(masterPos);
                if (isMappedPtr == NULL) 
                    break;
                
                if (false == *isMappedPtr) 
                {   
                    movieMediaTypePtr->fMediaStreamPtr = streamMediaTypePtr->fMediaStreamPtr;
                    *isMappedPtr = true;
                    mappedTracks++;
                    break; 
                }
            }
            streamMediaTypePtr = fStreamSDPParser.fSDPMediaList.Next();
        }//Stream
        movieMediaTypePtr = fMovieSDPParser->fSDPMediaList.Next();
    }//Movie, while (movieMediaTypePtr)

    result = mappedTracks;

    return result;

}


//1. 由.mov文件生成SDP文件, 和媒体相关部分
//2. 解析SDP文件, 并返回SDP文件的track数
UInt32 QTFileBroadcaster::GetSDPTracks(QTRTPFile *newRTPFilePtr)
{
    char*   sdpBuffer;
    int     bufferLen = 0;
    UInt32  result = 0;
    //构造SDP文件
    sdpBuffer = newRTPFilePtr->GetSDPFile(&bufferLen);
	//
    fMovieSDPParser->ParseSDP(sdpBuffer);
    result = fMovieSDPParser->GetNumTracks();

    return result;
}

//1. 把fMovieSDPParser->fSDPMediaList的所有TypeMap加入到QTRTPFile *newRTPFilePtr
//2. 并设置newRTPFilePtr的相关属性
int QTFileBroadcaster::AddTrackAndStream(QTRTPFile *newRTPFilePtr)
{
    TypeMap*    movieMediaTypePtr = fMovieSDPParser->fSDPMediaList.Begin();
    UInt32      trackID;
    char*       cookie;
    int         err = -1;
    
    while (movieMediaTypePtr)
    {
        if (movieMediaTypePtr->fMediaStreamPtr != NULL)
        {
            trackID = movieMediaTypePtr->fTrackID;
            if (trackID == 0) break;
            
            
            movieMediaTypePtr->fMediaStreamPtr->fData.fMovieMediaTypePtr = movieMediaTypePtr;
            movieMediaTypePtr->fMediaStreamPtr->fData.fRTPFilePtr = newRTPFilePtr;
            movieMediaTypePtr->fMediaStreamPtr->fSend = fSend;

            cookie = (char *) movieMediaTypePtr->fMediaStreamPtr;
            //增加一个track
			err = newRTPFilePtr->AddTrack(trackID, false);
            if (err != 0)
                break;
            //设置Cookie, 在这里Cookie是指向MediaStream的指针
            newRTPFilePtr->SetTrackCookies(trackID, cookie, 0);     
			//把SSRC设置为0
            newRTPFilePtr->SetTrackSSRC(trackID, (UInt32) 0); // set later
            //
            err = newRTPFilePtr->Seek(0.0);
            if (err != QTRTPFile::errNoError)
                break;

        }   
    
        movieMediaTypePtr = fMovieSDPParser->fSDPMediaList.Next();
    }
    
    return err;
}

//该函数的参数为: theMovieFileName = "C:\Program Files\VideoRelayServer\Movies\sample_100kbit.mov"
//函数的功能:
// 1. 建立newRTPFilePtr, 用sample_100kbit.mov初始化建立newRTPFilePtr
// 2. 由newRTPFilePtr初始化fMovieTracks, 
// 3. 建立fMovieSDPParser, 由.mov文件生成SDP文件, 和媒体相关部分, 解析SDP文件, 并返回SDP文件的track数
// 4. 建立Movie和流之间的映射
// 5. 为fRTPFilePtr增加Track以及流
int QTFileBroadcaster::SetUpAMovie(char *theMovieFileName)
{
    int err = -1;
    QTRTPFile *newRTPFilePtr = NULL;
    do {
        fMovieTracks = 0;
        fMappedMovieTracks = 0;
        
        if (fMovieSDPParser != NULL) delete fMovieSDPParser;
        if (fRTPFilePtr != NULL) delete fRTPFilePtr;
        
        fMovieSDPParser = NULL;
        fRTPFilePtr = NULL;
        
        if (!theMovieFileName)          { err = eMovieFileInvalidName; break; }     
        int nameLen = strlen(theMovieFileName);                     
        if (nameLen > 255)              { err = eMovieFileInvalidName; break; }
        if (0 == nameLen)               { err = eMovieFileInvalidName; break; }
        
        fMovieSDPParser = new SDPFileParser;
        if (NULL == fMovieSDPParser)    { err = eMem; break;}
        
        newRTPFilePtr = new QTRTPFile();
        if (NULL == newRTPFilePtr)      { err = eMem; break;}

        // 该函数执行QTRTPFile的初始化工作, 首先获得QTFile, 然后进行初始化, 查找HintTrack, 并填入由fFirstTrack维护的列表
        QTRTPFile::ErrorCode result = newRTPFilePtr->Initialize(theMovieFileName);	//由文件名, 初始化newRTPFilePtr, 打开文件, 并填入相关参数
        err = EvalErrorCode(result);
        if (err) break;
        
		//由.mov文件生成SDP文件, 和媒体相关部分, 解析SDP文件, 并返回SDP文件的track数
        fMovieTracks = GetSDPTracks(newRTPFilePtr);
        if (fMovieTracks < 1)           { err = eMovieFileNoHintedTracks; break; }
        
        fMappedMovieTracks = MapMovieToStream();
        if (fMappedMovieTracks < 1)     { err = eMovieFileNoSDPMatches; break; }

        err = AddTrackAndStream(newRTPFilePtr);     
        if (err != 0)                   { err = eMovieFileInvalid; break; }
        
    } while (false);
    
    if (err) 
    {   if (newRTPFilePtr) 
            delete newRTPFilePtr;
        newRTPFilePtr = NULL;
    }
    
    fRTPFilePtr = newRTPFilePtr;
    
    return err;
}

//休眠
void QTFileBroadcaster::MilliSleep(SInt32 sleepTimeMilli)
{
    if (sleepTimeMilli > 0)
    {
    #if __solaris__
        struct timeval tv;
        ::memset(&tv,0,sizeof(tv));
        tv.tv_usec = sleepTimeMilli * 1000;
        ::select(0,NULL,NULL,NULL,&tv);
    #else
        OSThread::Sleep( (UInt32) sleepTimeMilli);
    #endif
    }
}

//休眠, 该休息函数会给流发送SR报告
Float64 QTFileBroadcaster::Sleep(Float64 transmitTimeMilli)
{
    Float64 sleepTime;
    Float64 timeToSend;
    Float64 currentTime = (Float64) PlayListUtils::Milliseconds();
    
    if (fMovieStart) 
    {   
        fMovieStart = false;
    }
    

    timeToSend = fMovieStartTime + transmitTimeMilli;
    sleepTime =  timeToSend - currentTime;
                    

    const Float64 maxSleepIntervalMilli = 100.0;
    const Float64 minSleepIntervalMilli = 1.0;
    Float64 intervalTime = sleepTime;
    SInt32 sleepMilli = 0;
    if (intervalTime > maxSleepIntervalMilli)
    {   while (intervalTime >= maxSleepIntervalMilli)
        {   
            intervalTime -= maxSleepIntervalMilli;
            sleepMilli = (SInt32) maxSleepIntervalMilli;
            MilliSleep(sleepMilli);
            fMediaStreamList.UpdateSenderReportsOnStreams();
            //qtss_printf("sleepIntervalMilli %u \n",sleepMilli);
        }
    }
    if (intervalTime >= minSleepIntervalMilli)
    {   sleepMilli = (SInt32) intervalTime;
        MilliSleep(sleepMilli);
        //qtss_printf("sleepMilli %u \n",sleepMilli);
    }
    fMediaStreamList.UpdateSenderReportsOnStreams();
    
    return sleepTime;
}

/* changed by emil@popwire.com (see relaod.txt for info) */
//该函数带一个参数, mTimeFile, 值为"..\VideoRelayServer\Playlists\PlayList100K\PlayList100K.current"
//1. 输出PlayList100K.current
//2. 把播放信息写入到socket
int QTFileBroadcaster::Play(char *mTimeFile)
/* ***************************************************** */
{//以下160行代码
    SInt16  err = 0;
    Float64 transmitTime = 0;
    MediaStream *theStreamPtr = NULL;   
    RTpPacket   rtpPacket;
    unsigned int sleptTime;
    SInt32 movieStartOffset = 0; //z
    Bool16      negativeTime = false;
    fMovieDuration = fRTPFilePtr->GetMovieDuration();
    fSendTimeOffset = 0.0;
    fMovieStart = true;
    fNumMoviesPlayed ++;
    
    if (fMovieEndTime > 0) // take into account the movie load time as well as the last movie early end.
    {   UInt64 timeNow = PlayListUtils::Milliseconds();
        fMovieIntervalTime = timeNow - fMovieEndTime;

        SInt32 earlySleepTimeMilli = (SInt32)(fMovieTimeDiffMilli - fMovieIntervalTime);
        earlySleepTimeMilli -= 40; // Don't sleep the entire time we need some time to execute or else we will be late
        if (earlySleepTimeMilli > 0)
        {   OSThread::Sleep( earlySleepTimeMilli);
        }
    }
    
    fMovieStartTime = PlayListUtils::Milliseconds();    
    fMediaStreamList.MovieStarted(fMovieStartTime); 
    
	/* changed by emil@popwire.com (see relaod.txt for info) */
	//1. 输出PlayList100K.current
    if(mTimeFile!=NULL)
    {
        FILE *fTimeFile = NULL;
        struct timeval start, dur, end;
        struct tm tm_start, tm_dur, tm_end, timeResult;

        memset (&start,0, sizeof(start));

        SInt64 timenow = OS::Milliseconds();
        start.tv_sec = (long) OS::TimeMilli_To_UnixTimeSecs(timenow);
        start.tv_usec = (long) ((OS::TimeMilli_To_UnixTimeMilli(timenow) - (start.tv_sec * 1000)) * 1000);

        dur.tv_sec = (long)fMovieDuration;
        dur.tv_usec = (long)((fMovieDuration - dur.tv_sec) * 1000000);
        
        end.tv_sec = start.tv_sec + dur.tv_sec + (long)((start.tv_usec + dur.tv_usec) / 1000000);
        end.tv_usec = (start.tv_usec + dur.tv_usec) % 1000000;
                time_t startSecs = start.tv_sec;
                time_t endSecs = end.tv_sec;
        memcpy(&tm_start, qtss_localtime(&startSecs, &timeResult), sizeof(struct tm));
        memcpy(&tm_end, qtss_localtime(&endSecs, &timeResult), sizeof(struct tm));

        tm_dur.tm_hour = dur.tv_sec / 3600;
        tm_dur.tm_min = (dur.tv_sec % 3600) / 60;
        tm_dur.tm_sec = (dur.tv_sec % 3600) % 60;
        
        // initialize all current movie parameters to unkown ("-").
        
        ::strcpy(fCurrentMovieName, "-");
        ::strcpy(fCurrentMovieCopyright, "-");
        ::strcpy(fCurrentMovieComment, "-");
        ::strcpy(fCurrentMovieAuthor, "-");
        ::strcpy(fCurrentMovieArtist, "-");
        ::strcpy(fCurrentMovieAlbum, "-");

        /* save start time, stop time and length of currently playing song to .current file */
		//1. 把播放的信息写入*.current文件, 以下40行代码
        fTimeFile = fopen(mTimeFile, "a");
        if(fTimeFile)
        {   
            SimpleString *theQTTextPtr = fMovieSDPParser->fQTTextLines.Begin();
            while (theQTTextPtr != NULL)
            {
                char tmp[256];
                ::memcpy(tmp, theQTTextPtr->fTheString, theQTTextPtr->fLen);
                tmp[theQTTextPtr->fLen] = 0;
                // if this SDP parameter is needed for logging then cache it here so
                // we can log it later.
                if (::strstr(theQTTextPtr->fTheString, "a=x-qt-text-nam:")!=NULL)
                    ::strcpy(fCurrentMovieName, &tmp[16]);
                if (::strstr(theQTTextPtr->fTheString, "a=x-qt-text-cpy:")!=NULL)
                    ::strcpy(fCurrentMovieCopyright, &tmp[16]);
                if (::strstr(theQTTextPtr->fTheString, "a=x-qt-text-cmt:")!=NULL)
                    ::strcpy(fCurrentMovieComment, &tmp[16]);
                if (::strstr(theQTTextPtr->fTheString, "a=x-qt-text-aut:")!=NULL)
                    ::strcpy(fCurrentMovieAuthor, &tmp[16]);
                if (::strstr(theQTTextPtr->fTheString, "a=x-qt-text-ART:")!=NULL)
                    ::strcpy(fCurrentMovieArtist, &tmp[16]);
                if (::strstr(theQTTextPtr->fTheString, "a=x-qt-text-alb:")!=NULL)
                    ::strcpy(fCurrentMovieAlbum, &tmp[16]);
                fwrite(theQTTextPtr->fTheString,theQTTextPtr->fLen, sizeof(char),fTimeFile);
                qtss_fprintf(fTimeFile,"\n");
                theQTTextPtr = fMovieSDPParser->fQTTextLines.Next();
            }

            time_t startTime = (time_t) start.tv_sec;
            time_t endTime = (time_t) end.tv_sec;
            char buffer[kTimeStrSize];
            char *timestringStart = qtss_ctime(&startTime, buffer, sizeof(buffer));
            qtss_fprintf(fTimeFile,"b=%02d:%02d:%02d:%06d %ld %s", (int) tm_start.tm_hour, (int) tm_start.tm_min, (int) tm_start.tm_sec, (int)start.tv_usec, (long int) startTime, timestringStart);
            char *timestringEnd = qtss_ctime(&endTime, buffer, sizeof(buffer));
            qtss_fprintf(fTimeFile,"e=%02d:%02d:%02d:%06d %ld %s", (int)tm_end.tm_hour, (int) tm_end.tm_min,(int)  tm_end.tm_sec, (int) end.tv_usec,(long int) endTime, timestringEnd);
            qtss_fprintf(fTimeFile,"d=%02d:%02d:%02d:%06d %d \n", (int) tm_dur.tm_hour, (int) tm_dur.tm_min,(int)  tm_dur.tm_sec, (int) dur.tv_usec, (int)dur.tv_sec);

            fclose(fTimeFile);
        }   
    }
    //2. 循环调用GetNextPacket, 并通过theStreamPtr->Send(&rtpPacket);发送数据包
    while (true) 
    {//以下50行代码
        if (fQuitImmediatePtr && *fQuitImmediatePtr){err = 0; break; } // quit now not an error
        //如果包的列表长度过长, 则进行处理
        if (fBroadcastDefPtr->mTheSession)
        {   //从包队列中取出包
			UInt32 thePacketQLen = 0;
            thePacketQLen = fBroadcastDefPtr->mTheSession->GetPacketQLen();
            SInt64 maxSleep = PlayListUtils::Milliseconds() + 1000; 
			if (thePacketQLen > eMaxPacketQLen)
            {   //qtss_printf("PacketQ too big = %lu \n", (UInt32) thePacketQLen);
                while ( (eMaxPacketQLen/2) < fBroadcastDefPtr->mTheSession->GetPacketQLen())
                {   this->SleepInterval(100.0);
                    if (maxSleep < PlayListUtils::Milliseconds())
                        break;
                }
                //qtss_printf("PacketQ after sleep = %lu \n", (UInt32) fBroadcastDefPtr->mTheSession->GetPacketQLen());
                continue;
            }
        }
        
        //获得下一个包, 并休眠一定的时间, 然后发送该包
        transmitTime = fRTPFilePtr->GetNextPacket(&rtpPacket.fThePacket, &rtpPacket.fLength);
            theStreamPtr = (MediaStream*)fRTPFilePtr->GetLastPacketTrack()->Cookie1;
        err = fRTPFilePtr->Error();
        if (err != QTRTPFile::errNoError)   {err = eMovieFileInvalid; break; } // error getting packet
        if (NULL == rtpPacket.fThePacket)   {err = 0; break; } // end of movie not an error
        if (NULL == theStreamPtr)           {err = eMovieFileInvalid; break; }// an error

        
        transmitTime *= (Float64) PlayListUtils::eMilli; // convert to milliseconds
        if (transmitTime < 0.0 && negativeTime == false) // Deal with negative transmission times
        {   movieStartOffset += (SInt32) (transmitTime / 15.0);
            negativeTime = true;
        }
        sleptTime = (unsigned int) Sleep(transmitTime);
        
        err = theStreamPtr->Send(&rtpPacket);
            
        if (err != 0)  { break; } 
		//发完以后, 调用UpdateStreams()更新所有流, 从所有流的端口上接收RTP包以及RTCP包
        err = fMediaStreamList.UpdateStreams();
        if (err != 0)  { break; } 
        
        if (    (fBroadcastDefPtr != NULL)
            &&  (fBroadcastDefPtr->mTheSession != NULL)
            &&  (fBroadcastDefPtr->mTheSession->GetReasonForDying() != BroadcasterSession::kDiedNormally)   
            )   
         { break; } 
    };  //while (true), 以上50行代码, 不断地发送数据
    
    fMovieEndTime = (SInt64) PlayListUtils::Milliseconds(); 
    fMediaStreamList.MovieEnded(fMovieEndTime);

    // see if the movie duration is greater than the time it took to send the packets.
    // the difference is a delay that we insert before playing the next movie.
    SInt64 playDurationMilli = (SInt64) fMovieEndTime - (SInt64) fMovieStartTime;
    fMovieTimeDiffMilli =  ((SInt64) ( (Float64) fMovieDuration * (Float64) PlayListUtils::eMilli)) - (SInt64) playDurationMilli;
    fMovieTimeDiffMilli-= (movieStartOffset/2);

    return err;
}

/* changed by emil@popwire.com (see relaod.txt for info) */
//设置Mov文件, 并播放该文件
int QTFileBroadcaster::PlayMovie(char *movieFileName, char *currentFile)
/* ***************************************************** */
{
    
    int err = eMovieFileInvalidName;
    if (movieFileName != NULL)
    {   
        //设置一个Mov文件
        err = SetUpAMovie(movieFileName);
        //播放该文件 
        if (!err && fPlay)
/* changed by emil@popwire.com (see relaod.txt for info) */
        {    err = Play(currentFile);
/* ***************************************************** */
        }
    }
    return err;
}

//由错误返回错误代码
int QTFileBroadcaster::EvalErrorCode(QTRTPFile::ErrorCode err)
{   
    int result = eNoErr;
    
    switch( err ) 
    {
        case QTRTPFile::errNoError:
        break;

        case QTRTPFile::errFileNotFound:
            result = eMovieFileNotFound;
        break;
        
        case QTRTPFile::errNoHintTracks:
            result = eMovieFileNoHintedTracks;
        break;

        case QTRTPFile::errInvalidQuickTimeFile:
            result = eMovieFileInvalid;
        break;
        
        case QTRTPFile::errInternalError:
            result = eInternalError;
        break;
        
        default:
            result = eInternalError;
    }
    return result;
}
