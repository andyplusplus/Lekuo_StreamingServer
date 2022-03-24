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
     playlist_SDPGen.cpp
 */
#include "playlist_utils.h"
#include "playlist_SDPGen.h"
#include "playlist_broadcaster.h"
#include "QTRTPFile.h"
#include "OS.h"
#include "SocketUtils.h"
#include "SDPUtils.h"
#include "OSArrayObjectDeleter.h"

//在*aSDPFile的currentFilePos的位置写入*chars
short SDPGen::AddToBuff(char *aSDPFile, short currentFilePos, char *chars)
{
    short charLen = (short) strlen(chars);
    short newPos = currentFilePos + charLen;
    
    if (newPos <= eMaxSDPFileSize)
    {   memcpy(&aSDPFile[currentFilePos],chars,charLen); // only the chars not the \0
        aSDPFile[currentFilePos +charLen] = '\0';
    }   
    else
    {   newPos =  (-newPos);    
    }
    currentFilePos = newPos;
    
    return currentFilePos;
}

//生成一个随机的时间
UInt32 SDPGen::RandomTime(void)
{
    SInt64 curTime = 0;
	curTime = PlayListUtils::Milliseconds();

    curTime += rand() ;
    return (UInt32) curTime;
}


//该函数由文件外返回文件的路径
	// '*pathName为SDP文件的名称, 如..\VideoRelayServer\Playlists\PlayList100K\PlayList100K.dsp
	// '*result为会话名称
	// '*maxResult 为 *result的最大长度, 长度为64
	//  返回..\VideoRelayServer\Playlists\PlayList100K\////
short  SDPGen::GetNoExtensionFileName(char *pathName, char *result, short maxResult)
{
    char *start;
    char *end;
    char *sdpPath = pathName;
    short pathNameLen = (short) strlen(pathName);
    short copyLen = 0;
    
    
    do 
    {
        start = strrchr(sdpPath, ePath_Separator);
        if(start  == NULL ) // no path separator
        {   start = sdpPath;
            copyLen =   pathNameLen;
            break;
        } 
        
        start ++; // move start to one past the separator
        end = strrchr(sdpPath, eExtension_Separator);       
        if (end == NULL) // no extension
        {   copyLen = (short) strlen(start) + 1;
            break;
        }
        
        // both path separator and an extension
        short startLen = (short) strlen(start);
        short endLen = (short) strlen(end);
        copyLen = startLen - endLen;
        
    } while (false);
    
    if (copyLen > maxResult)
        copyLen = maxResult;
        
    memcpy(result, start, copyLen); 
    result[copyLen] = '\0';
    
    return copyLen;
}
            

//
char *SDPGen::Process(  char *sdpFileName,	//SDP的文件名
                        char * basePort,	//5004
                        char *ipAddress,	//127.0.0.1
                        char *anSDPBuffer,	//b=AS:94..a=range:npt=0-  70.00000..m=video 0 RTP/AVP 96..b=AS:79..a=rtpmap: X-SV3V-ES/90000..a=control:trackID=3..m=audio 0 RTP/AVP 97..b=AS:14..a=rtpmap:97 X-QDM/22050/2..a=control:trackID=4..a=x-bufferdelay:4.97...
                        char *startTime,	//0
                        char *endTime,		//0
                        char *isDynamic,	//Enabled
                        char *ttl,			//1
                        int *error			//-1
                        )
{//220行代码
    char *resultBuf = NULL;
    short currentPos = 0;
    short trackID = 1;
    *error = -1;
    do
    {   
        fSDPFileContentsBuf = new char[eMaxSDPFileSize];
         // SDP required RFC 2327
        //    v=  (protocol version)
        //    o=<username> <session id = random time> <version = random time *> <network type = IN> <address type = IP4> <local address>
        //    s=  (session name)
        //    c=IN IP4 (destinatin ip address)
        // * see RFC for recommended Network Time Stamp (NTP implementation not required)

        //    v=  (protocol version)
        {   char version[] = "v=0\r\n";
        
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, version);
            if (currentPos < 0) break;
        }
            
        //    o=<username> <session id = random time> <version = random time *> <network type = IN> <address type = IP4> <address>
        {   char *userName = "QTSS_Play_List";
            UInt32 sessIDAsRandomTime = RandomTime();
            UInt32 versAsRandomTime = RandomTime();
            char  ownerLine[255];

            Assert(SocketUtils::GetIPAddrStr(0) != NULL);
            Assert(SocketUtils::GetIPAddrStr(0)->Ptr != NULL);
            qtss_sprintf(ownerLine, "o=%s %lu %lu IN IP4 %s\r\n",userName ,sessIDAsRandomTime,versAsRandomTime,SocketUtils::GetIPAddrStr(0)->Ptr);
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, ownerLine);
            if (currentPos < 0) break;
        }
        

        //    s=  (session name)
        {   enum { eMaxSessionName = 64};
            char newSessionName[eMaxSessionName];
            short nameSize = 0;
            char  sessionLine[255];
            nameSize = GetNoExtensionFileName(sdpFileName, newSessionName, eMaxSessionName);
            if (nameSize < 0) break;
        
            qtss_sprintf(sessionLine, "s=%s\r\n", newSessionName);
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, sessionLine);
            if (currentPos < 0) break;
        }
        
        //    c=IN IP4 (destinatin ip address)
        {   
            char  sdpLine[255];
            if (SocketUtils::IsMulticastIPAddr(ntohl(inet_addr(ipAddress))))
                qtss_sprintf(sdpLine, "c=IN IP4 %s/%s\r\n", ipAddress,ttl);
            else
                 qtss_sprintf(sdpLine, "c=IN IP4 %s\r\n", ipAddress);
           currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, sdpLine);
            if (currentPos < 0) break;
        }
        //    a=x-broadcastcontrol:
        {
            char  controlLine[255];
            if ( 0 == ::strcmp( "enabled", isDynamic) )
                qtss_sprintf(controlLine, "a=x-broadcastcontrol:RTSP\r\n");          
            else
                qtss_sprintf(controlLine, "a=x-broadcastcontrol:TIME\r\n");          
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, controlLine);
            if (currentPos < 0) break;
        }
                
        //    t = 0 0
        {
            char  timeLine[255];
            UInt32 startTimeNTPSecs = strtoul(startTime, NULL, 10);
            UInt32 endTimeNTPSecs = strtoul(endTime, NULL, 10);
            qtss_sprintf(timeLine, "t=%lu %lu\r\n", startTimeNTPSecs, endTimeNTPSecs);           
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, timeLine);
            if (currentPos < 0) break;
        }

		//以下函数解析anSDPBuffer参数传过来的值, 则值QTRTP类传过来, 描述了mov文件的一些信息
        {//解析anSDPBuffer, 从mov文件读出的参数
            SimpleString resultString;
            SimpleString sdpBuffString(anSDPBuffer);
    
            short numLines = 0;
            char *found = NULL;
            
            enum  {eMaxLineLen = 1024};
            char aLine[eMaxLineLen];
            ::memset(aLine, 0, eMaxLineLen);
            
            int portCount = atoi(basePort);
            
            
            
            SimpleParser sdpParser;
            while ( sdpParser.GetLine(&sdpBuffString,&resultString) ) 
            {//以下代码解析sdpBuffString的每个行, 并对每一行进行处理
                numLines ++;
                if (resultString.fLen > 1024) continue;
              //1. 把字条符串拷贝给aLine
                memcpy(aLine,resultString.fTheString,resultString.fLen);
                aLine[resultString.fLen] = '\0';
                
                int newBuffSize = sdpBuffString.fLen - (resultString.fLen);
                char *newBuffPtr = &resultString.fTheString[resultString.fLen];
                
                sdpBuffString.SetString(newBuffPtr, newBuffSize);   

              //2. 略过某些命令的处理
                char firstChar = aLine[0];
                { // we are setting these so ignore any defined                     
                    if (firstChar == 'v') continue;// (protocol version)
                    if (firstChar == 'o') continue; //(owner/creator and session identifier).
                    if (firstChar == 's') continue; //(session name)
                    if (firstChar == 'c') continue; //(connection information - optional if included at session-level)
                }
              //3. 关闭Range选项, 打开媒体行选项
                {   // no longer important as a play list broadcast
                    // there may be others that should go here.......
                    if (firstChar == 't') continue;// (time the session is active)              
                    if (firstChar == 'r') continue;// (zero or more repeat times)

                    // found =  strstr(aLine, "a=cliprect"); // turn this off
                    // if (found != NULL) continue;
                    
                    found = strstr(aLine, "a=control:trackID"); // turn this off
                    if (!fKeepTracks)
                    {   
                        if (fAddIndexTracks)
                        {
                            if (found != NULL) 
                            {   char mediaLine[eMaxLineLen];                                            
                                qtss_sprintf(mediaLine,"a=control:trackID=%d\r\n",trackID);
                                currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, mediaLine); // copy rest of line starting with the transport protocol
                                trackID ++;
                            }
                        }
                        if (found != NULL) continue;
                    }
                    
                    
                    found = strstr(aLine,  "a=range"); // turn this off
                    if (found != NULL) continue;
                    
                    // found = strstr(aLine,  "a=x"); // turn this off - 
                    // if (found != NULL) continue;
                }
              //4. 处理媒体行, 并在媒体存入端口.  
                { // handle the media line and put in the port value past the media type
                    found = strstr(aLine,"m=");  //(media name and transport address)
                    if (found != NULL)
                    {   
                        char *startToPortVal = strtok(aLine," ");
                        strtok(NULL," "); // step past the current port value we put it in below
                        if (found != NULL) 
                        {   char mediaLine[eMaxLineLen];                
                            char *protocol = strtok(NULL,"\r\n"); // the transport protocol
                            
                            qtss_sprintf(mediaLine,"%s %d %s\r\n",startToPortVal,portCount,protocol);
                            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, mediaLine); // copy rest of line starting with the transport protocol
                            if (portCount != 0)
                                portCount += 2; // set the next port value ( this port + 1 is the RTCP port for this port so we skip by 2)
                            continue;
                        }
                    }
                }

			  //5. 加入回车符
                {   // this line looks ok so just get it and make sure it has a carriage return + new line at the end
                    // also remove any garbage that might be there
                    // this is a defensive measure because the hinter may have bad line endings like a single \n or \r or 
                    // there might also be chars after the last line delimeter and before a \0 so we get rid of it.
                    
                    short lineLen = (short) strlen(aLine);
                    
                    // get rid of trailing characters
                    while (lineLen > 0 && (NULL == strchr("\r\n",aLine[lineLen]))  )
                    {   aLine[lineLen] = '\0';
                        lineLen --;
                    }
                    
                    // get rid of any line feeds and carriage returns
                    while (lineLen > 0 && (NULL != strchr("\r\n",aLine[lineLen])) )
                    {   aLine[lineLen] = '\0';
                        lineLen --;
                    }
                    aLine[lineLen + 1] = '\r';
                    aLine[lineLen + 2] = '\n';
                    aLine[lineLen + 3] = 0;
                    currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, aLine); // copy this line   
                }
            }//while ( sdpParser.GetLine(&sdpBuffString,&resultString) ), 解析每一行

          //缓冲延时  buffer delay 
            if (fClientBufferDelay > 0.0)
            {   
                char  delayLine[255];
                qtss_sprintf(delayLine, "a=x-bufferdelay:%.2f\r\n", fClientBufferDelay);
                currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, delayLine);
                if (currentPos < 0) break;
            }
        }//解析anSDPBuffer, 从mov文件读出的参数
    

        StrPtrLen theSDPStr(fSDPFileContentsBuf);
        SDPContainer rawSDPContainer;	//建立SDPContainer对象
        if (!rawSDPContainer.SetSDPBuffer( &theSDPStr ))	//设置SDPContainer对象, 并解析该对象
        {    delete [] fSDPFileContentsBuf;
             fSDPFileContentsBuf = NULL;
             return NULL; 
        }
    
        SDPLineSorter sortedSDP(&rawSDPContainer);
        resultBuf = sortedSDP.GetSortedSDPCopy(); // return a new copy of the sorted SDP
        delete [] fSDPFileContentsBuf; // this has to happen after GetSortedSDPCopy
        fSDPFileContentsBuf = resultBuf;
       
        *error = 0;
        
    } while (false);  //该循环包含整个子程序
        
    return resultBuf;
}

int SDPGen::Run(  char *movieFilename   //影视文件名
                , char *sdpFilename		//SDP文件名
                , char *basePort		//"5004"
                , char *ipAddress		//"127.0.0.1"
                , char *buff			//SDPBuffer, 即"", 从主函数传递过来的SDP缓存区, 传过来时, 该Buffer为空
                , short buffSize		//SDPBuffer的最大空间
                , bool overWriteSDP		//是否覆盖SDP
                , bool forceNewSDP		//件否强制产生新的SDP
				
				//以下几个参数从PLBroadcastDef结构传递过来, 分别和对应的成员变量相对应
                , char *startTime	//0
                , char *endTime		//0
                , char *isDynamic	//Enabled
                , char *ttl			//1
               )
{
    int result = -1;
    int fdsdp = -1;
    bool sdpExists = false;
    
    do 
    {
        if (!movieFilename) break;
        if (!sdpFilename) break;
        
        if (buff && buffSize > 0) // set buff to 0 length string
            buff[0] = 0;
            
        fdsdp = open(sdpFilename, O_RDONLY, 0);     
        if (fdsdp != -1)
            sdpExists = true;
            
        if (sdpExists && !forceNewSDP)
        {   
            if (!overWriteSDP) 
            {
                if (buff && (buffSize > 0)) 
                {   int count = ::read(fdsdp,buff, buffSize -1);
                    if (count > 0) 
                        buff[count] = 0;
                }
                    
            }

            close(fdsdp);
            fdsdp = -1;
            
            if (!overWriteSDP) 
            {   result = 0;
                break; // leave nothing to do
            }
        }   

  //1. 该函数执行QTRTPFile的初始化工作, 首先获得QTFile, 然后进行初始化, 查找HintTrack, 并填入由fFirstTrack维护的列表    
        QTRTPFile::ErrorCode err = fRTPFile.Initialize(movieFilename);
        result = QTFileBroadcaster::EvalErrorCode(err);
        if( result != QTRTPFile::errNoError ) 
            break;
  //2. 构造SDP文件                 
        // Get the file
        int     sdpFileLength=0;            
        int     processedSize=0;            
        char    *theSDPText = fRTPFile.GetSDPFile(&sdpFileLength);	//构造SDP文件
        
        if( theSDPText == NULL || sdpFileLength <= 0) 
        {   break;
        }
               
        char *processedSDP = NULL;
        processedSDP = Process(sdpFilename, basePort, ipAddress, theSDPText,startTime,endTime,isDynamic, ttl, &result);
        if (result != 0) break;
        
        processedSize = strlen(processedSDP);
        
        if (buff != NULL)
        {   if (buffSize > processedSize )
            {    
                buffSize = processedSize;
            }
            memcpy(buff,processedSDP,buffSize);
            buff[buffSize] = 0;
        }
        
        if (!overWriteSDP && sdpExists) 
        {
            break;
        }
        // Create our SDP file and write out the data           
#ifdef __Win32__
        fdsdp = open(sdpFilename, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, 0664);
#else
        fdsdp = open(sdpFilename, O_CREAT | O_TRUNC | O_WRONLY, 0664);
#endif
        if( fdsdp == -1 ) 
        {   
                        //result = -1;
                        result = -2;
                        break;
        }   
        write(fdsdp, processedSDP, processedSize);  
        result = 0;
        
        // report that we made a file
        fSDPFileCreated = true;
        
        
    } while (false);
    
    if (fdsdp != -1)
    {   result = 0;
        close(fdsdp);
        fdsdp = -1;
    }
    
    return result;

}

