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
    
    QTRTPFile               *fRTPFilePtr ;		//ָ��RTP�ļ���ָ��

    //SDPFileParser��fStreamSDPParser��ʲô��ͬ, ��MapMovieToStream(...)��ΪʲôҪ����ӳ��
	SDPFileParser           fStreamSDPParser;	//SDP�ļ��Ľ�����	
    SDPFileParser           *fMovieSDPParser;	//SDP������				
		//��ֵ��SetUpAMovie(char *theMovieFileName)�н���
		//�ڴ˴���ʼ��fMovieSDPParser->ParseSDP(sdpBuffer); ����sdpBuffer��QTRTPFile��̬���� 
    SocketList              fSocketlist;		//����ΪArrayList<UDPSocketPair>, UDPSocketPair����
    MediaStreamList         fMediaStreamList;	//MediaStream���������
    int                     fBasePort;			//����ַ
    bool                    fDebug;
    bool                    fDeepDebug;
    bool                    *fQuitImmediatePtr;		//�Ƿ������˳�, �ò���ָ��sQuitImmediate

// ��ʱ���йص�һЩ���� transmit time trackers
    Float64                 fLastTransmitTime;
    
    SInt64                  fStreamStartTime;		//��������ʱ��
    SInt64                  fMovieStartTime;		//��ʼ����Movie��ʱ��, ��int QTFileBroadcaster::Play(...)�г�ʼ��
    SInt64                  fMovieEndTime;
    SInt64                  fMovieIntervalTime;
    SInt64                  fMovieTimeDiffMilli;
//  ......    ��Movie�йص�һЩ��Ϣ
    bool                    fMovieStart;
    Float64                 fSendTimeOffset;
    Float64                 fMovieDuration;
    int                     fMovieTracks;			//Movie��track��
    int                     fMappedMovieTracks;
    UInt64                  fNumMoviesPlayed;
        //���غ� "id" ��ͬ��PayLoad
    PayLoad *               FindPayLoad(short id, ArrayList<PayLoad> *PayLoadListPtr);
		//ȷ��movieMediaTypePtr�Լ�streamMediaTypePtr�Ƿ񶼰���payloadIDΪmoviePayLoadID��PayLoad
    bool                    CompareRTPMaps(TypeMap *movieMediaTypePtr, TypeMap *streamMediaTypePtr, short id);	
		//ȷ��movieMediaTypePtr�Ƿ����streamMediaTypePtr������ý������
    bool                    CompareMediaTypes(TypeMap *movieMediaTypePtr, TypeMap *streamMediaTypePtr);
		//1. ��.mov�ļ�����SDP�ļ�, ��ý����ز���
		//2. ����SDP�ļ�, ������SDP�ļ���track��
    UInt32                  GetSDPTracks(QTRTPFile *newRTPFilePtr);
		//�ú����Ĳ���Ϊ: theMovieFileName = "C:\Program Files\VideoRelayServer\Movies\sample_100kbit.mov"
		//�����Ĺ���:
		// 1. ����newRTPFilePtr, ��sample_100kbit.mov��ʼ������newRTPFilePtr
		// 2. ��newRTPFilePtr��ʼ��fMovieTracks, 
		// 3. ����fMovieSDPParser, ��.mov�ļ�����SDP�ļ�, ��ý����ز���, ����SDP�ļ�, ������SDP�ļ���track��
		// 4. ����Movie����֮���ӳ��
		// 5. ΪfRTPFilePtr����Track�Լ���
    int                     SetUpAMovie(char *movieFileName);
		//1. ��fMovieSDPParser->fSDPMediaList������TypeMap���뵽QTRTPFile *newRTPFilePtr
		//2. ������newRTPFilePtr���������
    int                     AddTrackAndStream(QTRTPFile *newRTPFilePtr);
		// 1. N*N�Ƚ�fMovieSDPParser�Լ�fStreamSDPParser�Ľ�Ŀ��
		// 2. �Ƚ�ý������, �����ͬ, ��movieMediaTypePtr->fMediaStreamPtr = streamMediaTypePtr->fMediaStreamPtr;
		// 3. ����ƥ��Ľ����
    int                     MapMovieToStream();
		//�ú�����һ������, mTimeFile, ֵΪ"..\VideoRelayServer\Playlists\PlayList100K\PlayList100K.current"
		//1. ���PlayList100K.current
		//2. �Ѳ�����Ϣд�뵽socket
    int                     Play(char *mTimeFile);
    Float64                 SleepInterval(Float64 sleepTime) { return Sleep( (Float64) (PlayListUtils::Milliseconds() - fMovieStartTime) + sleepTime); };
		//����, ����Ϣ�������������SR����
    Float64                 Sleep(Float64 transmitTime);
    void                    SetDebug(bool debug) {fDebug = debug;};
    void                    SetDeepDebug(bool debug) {fDeepDebug = debug;};
    PLBroadcastDef          *fBroadcastDefPtr;		//ָ����Ӧ��PLBroadcastDef����, ��ֵ��Setup������������
public:
        QTFileBroadcaster();	//��ʼ����Ա����
        ~QTFileBroadcaster();	//��������				
    
		//�ɴ��󷵻ش������
static  int     EvalErrorCode(QTRTPFile::ErrorCode err);
		
		//��broadcastDefPtr�Լ�SDP����ز�����ֵ��QTFileBroadcaster
        int     SetUp(PLBroadcastDef *broadcastDefPtr, bool *quitImmediatePtr);
		//����Mov�ļ�, �����Ÿ��ļ�	
        int     PlayMovie(char *movieFileName, char *currentFile);
        int     GetMovieTrackCount() { return fMovieTracks; };
        int     GetMappedMovieTrackCount() { return fMappedMovieTracks; };
		//�߳�����
        void    MilliSleep(SInt32 sleepTimeMilli);
        bool    fPlay;
        bool    fSend;
    
    enum {  eClientBufferSecs = 0,
            eMaxPacketQLen = 200
         };
    
	//������Ϣ��ID
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
    
	//��SDP�ļ�����������һЩ�ı���Ϣ, �йص�ǰmovie��һЩ��Ϣ
    char	fCurrentMovieName[256];
    char	fCurrentMovieCopyright[256];
    char	fCurrentMovieComment[256];
    char	fCurrentMovieAuthor[256];
    char	fCurrentMovieArtist[256];
    char	fCurrentMovieAlbum[256];
    
private:

};

#endif //playlist_broadcaster_H
