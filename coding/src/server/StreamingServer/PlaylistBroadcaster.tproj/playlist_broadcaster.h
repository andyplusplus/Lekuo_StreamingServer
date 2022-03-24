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

/*  File:       playlist_broadcaster.h

    Contains:   

*/

#ifndef playlist_broadcaster_H
#define playlist_broadcaster_H

#include <stdio.h>
#include <stdlib.h>
#include "SafeStdLib.h"
#include <string.h>
#include <errno.h>
#include "OSHeaders.h"
#include "playlist_utils.h"
#include "playlist_elements.h"
#include "playlist_lists.h"
#include "playlist_parsers.h"
#include "QTRTPFile.h"
#include "PLBroadcastDef.h"


#ifndef __Win32__
    #include <sys/types.h>
    #include <fcntl.h>
#endif

static PlayListUtils gUtils;


class QTFileBroadcaster 
{

protected:
    
    QTRTPFile               *fRTPFilePtr ;		//指向RTP文件的指针

    //SDPFileParser和fStreamSDPParser有什么不同, 在MapMovieToStream(...)中为什么要进行映射
	SDPFileParser           fStreamSDPParser;	//SDP文件的解析器	
    SDPFileParser           *fMovieSDPParser;	//SDP解析器				
		//该值在SetUpAMovie(char *theMovieFileName)中建立
		//在此处初始化fMovieSDPParser->ParseSDP(sdpBuffer); 其中sdpBuffer由QTRTPFile动态生成 
    SocketList              fSocketlist;		//定义为ArrayList<UDPSocketPair>, UDPSocketPair数组
    MediaStreamList         fMediaStreamList;	//MediaStream的数组对象
    int                     fBasePort;			//基地址
    bool                    fDebug;
    bool                    fDeepDebug;
    bool                    *fQuitImmediatePtr;		//是否立即退出, 该参数指向sQuitImmediate

// 和时间有关的一些变量 transmit time trackers
    Float64                 fLastTransmitTime;
    
    SInt64                  fStreamStartTime;		//流的启动时间
    SInt64                  fMovieStartTime;		//开始播放Movie的时间, 在int QTFileBroadcaster::Play(...)中初始化
    SInt64                  fMovieEndTime;
    SInt64                  fMovieIntervalTime;
    SInt64                  fMovieTimeDiffMilli;
//  ......    和Movie有关的一些信息
    bool                    fMovieStart;
    Float64                 fSendTimeOffset;
    Float64                 fMovieDuration;
    int                     fMovieTracks;			//Movie的track数
    int                     fMappedMovieTracks;
    UInt64                  fNumMoviesPlayed;
        //返回和 "id" 相同的PayLoad
    PayLoad *               FindPayLoad(short id, ArrayList<PayLoad> *PayLoadListPtr);
		//确定movieMediaTypePtr以及streamMediaTypePtr是否都包含payloadID为moviePayLoadID的PayLoad
    bool                    CompareRTPMaps(TypeMap *movieMediaTypePtr, TypeMap *streamMediaTypePtr, short id);	
		//确定movieMediaTypePtr是否包含streamMediaTypePtr的所有媒体类型
    bool                    CompareMediaTypes(TypeMap *movieMediaTypePtr, TypeMap *streamMediaTypePtr);
		//1. 由.mov文件生成SDP文件, 和媒体相关部分
		//2. 解析SDP文件, 并返回SDP文件的track数
    UInt32                  GetSDPTracks(QTRTPFile *newRTPFilePtr);
		//该函数的参数为: theMovieFileName = "C:\Program Files\VideoRelayServer\Movies\sample_100kbit.mov"
		//函数的功能:
		// 1. 建立newRTPFilePtr, 用sample_100kbit.mov初始化建立newRTPFilePtr
		// 2. 由newRTPFilePtr初始化fMovieTracks, 
		// 3. 建立fMovieSDPParser, 由.mov文件生成SDP文件, 和媒体相关部分, 解析SDP文件, 并返回SDP文件的track数
		// 4. 建立Movie和流之间的映射
		// 5. 为fRTPFilePtr增加Track以及流
    int                     SetUpAMovie(char *movieFileName);
		//1. 把fMovieSDPParser->fSDPMediaList的所有TypeMap加入到QTRTPFile *newRTPFilePtr
		//2. 并设置newRTPFilePtr的相关属性
    int                     AddTrackAndStream(QTRTPFile *newRTPFilePtr);
		// 1. N*N比较fMovieSDPParser以及fStreamSDPParser的节目表
		// 2. 比较媒体类型, 如果相同, 则movieMediaTypePtr->fMediaStreamPtr = streamMediaTypePtr->fMediaStreamPtr;
		// 3. 返回匹配的结果数
    int                     MapMovieToStream();
		//该函数带一个参数, mTimeFile, 值为"..\VideoRelayServer\Playlists\PlayList100K\PlayList100K.current"
		//1. 输出PlayList100K.current
		//2. 把播放信息写入到socket
    int                     Play(char *mTimeFile);
    Float64                 SleepInterval(Float64 sleepTime) { return Sleep( (Float64) (PlayListUtils::Milliseconds() - fMovieStartTime) + sleepTime); };
		//休眠, 该休息函数会给流发送SR报告
    Float64                 Sleep(Float64 transmitTime);
    void                    SetDebug(bool debug) {fDebug = debug;};
    void                    SetDeepDebug(bool debug) {fDeepDebug = debug;};
    PLBroadcastDef          *fBroadcastDefPtr;		//指向相应的PLBroadcastDef对象, 该值由Setup函数参数传入
public:
        QTFileBroadcaster();	//初始化成员变量
        ~QTFileBroadcaster();	//析构函数				
    
		//由错误返回错误代码
static  int     EvalErrorCode(QTRTPFile::ErrorCode err);
		
		//把broadcastDefPtr以及SDP的相关参数赋值给QTFileBroadcaster
        int     SetUp(PLBroadcastDef *broadcastDefPtr, bool *quitImmediatePtr);
		//设置Mov文件, 并播放该文件	
        int     PlayMovie(char *movieFileName, char *currentFile);
        int     GetMovieTrackCount() { return fMovieTracks; };
        int     GetMappedMovieTrackCount() { return fMappedMovieTracks; };
		//线程休眠
        void    MilliSleep(SInt32 sleepTimeMilli);
        bool    fPlay;
        bool    fSend;
    
    enum {  eClientBufferSecs = 0,
            eMaxPacketQLen = 200
         };
    
	//错误信息的ID
    enum ErrorID 
    {   // General errors
         eNoErr             = 0
        ,eParam                     
        ,eMem                       
        ,eInternalError     
        ,eFailedBind
        
        // Setup Errors
        ,eNoAvailableSockets 
        ,eSDPFileNotFound   
        ,eSDPDestAddrInvalid 
        ,eSDPFileInvalid    
        ,eSDPFileNoMedia     
        ,eSDPFileNoPorts    
        ,eSDPFileInvalidPort 
        ,eSDPFileInvalidName 
        ,eSDPFileInvalidTTL
        
        // eMem also
        
        // Play Errors,
        ,eMovieFileNotFound         
        ,eMovieFileNoHintedTracks   
        ,eMovieFileNoSDPMatches     
        ,eMovieFileInvalid          
        ,eMovieFileInvalidName          
        
        ,eNetworkConnectionError    
        ,eNetworkRequestError       
        ,eNetworkConnectionStopped  
        ,eNetworkAuthorization      
        ,eNetworkNotSupported       
        ,eNetworkSDPFileNameInvalidMissing 
        ,eNetworkSDPFileNameInvalidBadPath
        ,eNetworkConnectionFailed
        
        ,eDescriptionInvalidDestPort
    };
    
	//从SDP文件解析出来的一些文本信息, 有关当前movie的一些信息
    char	fCurrentMovieName[256];
    char	fCurrentMovieCopyright[256];
    char	fCurrentMovieComment[256];
    char	fCurrentMovieAuthor[256];
    char	fCurrentMovieArtist[256];
    char	fCurrentMovieAlbum[256];
    
private:

};

#endif //playlist_broadcaster_H
