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


#include "playlist_utils.h"
#include "OS.h"


#if Log_Enabled
    enum {gBuffSize = 532, gBuffMaxStr = 512};
    
    enum {switchEntries, switchTime, switchBytes};		//分别是接表项数, 决定是否切换日志, 按时间决定, 按字节数决定
    
    enum {gLogMaxBytes = 100000};
    enum {gLogMaxEntries = 1000};
    enum {gLogMaxMilliSecs = 3 * 60 * 1000};


    static int gLogSwitchSetting = switchEntries;	//分别是接表项数, 决定是否切换日志, 按时间决定, 按字节数决定
    static long gLogEntries  = 0;		//已经记录的日志数
    static SInt64 gLogTimeStart = 0;	//开始记录日志的时间
    static long gLogNumBytes = 0;		//记录的总字节数		
    static gLogNumPackets = 0;			//总包数
    static char gTempStr[256];			//临时字符串
    static FILE *gLogFile_1 = 0;		//日志文件1句柄
    static FILE *gLogFile_2 = 0;		//日志文件2
    static FILE *gLogFileCurrent = 0;	//当前日志文件
    static int gFileNum = 1;			//当前正在记录哪个文件, 1表示记录当前文件
    
    static bool gLogStarted = false;	//是否已经开始记录日志了
    static char gLogBuff[gBuffSize];	//log写入的缓存
    static int gBuffUsed = 0;			//已经使用的Buffer大小


//打开日志文件
void    LogFileOpen(void)  
{
    if (!gLogFile_1 && !gLogFile_2)
    {   gLogFile_1 = fopen("logfile_1.txt","w");
        gLogFileCurrent = gLogFile_1;
        *gLogBuff = 0;
        gLogTimeStart =  PlayListUtils::Milliseconds();
        gLogStarted = true;
        gLogEntries = 0;
        gLogTimeStart = 0;
        gLogNumBytes = 0;
        gLogNumPackets = 0;
        gFileNum = 1;
        gBuffUsed = 0;
    }
}

//关闭日志文件
void    LogFileClose(void)
{
    fclose(gLogFile_1);
    fclose(gLogFile_2);
    gLogFileCurrent = gLogFile_1 = gLogFile_2 = 0;
    
}

//根据现在已经记录的日志数, 决定是否需要切换日志, 并返回该决定
bool TimeToSwitch(int len)
{
    bool timeToSwitch = false;
    if (!gLogStarted) return timeToSwitch;
      
    switch( gLogSwitchSetting )		//分别是接表项数, 决定是否切换日志, 按时间决定, 按字节数决定
    {
        case switchEntries:		
            if (gLogEntries >= gLogMaxEntries)
            {   timeToSwitch = true;
                gLogEntries = 1;
            }
            else 
                gLogEntries ++;
        break;
        
        case switchTime:
        {   SInt64 timeNow = PlayListUtils::Milliseconds();
            SInt64 timeThisFile = timeNow - gLogTimeStart;
            if ( timeThisFile >  gLogMaxMilliSecs )
            {   timeToSwitch = true;
                gLogTimeStart = timeNow;
            }
        }
        break;
        
        case switchBytes:
            if (gLogNumBytes > gLogMaxBytes)
            {   timeToSwitch = true;
                gLogNumBytes = 0;
            }
            gLogNumBytes += len;
        break;
    };
     

    return timeToSwitch;

}

//把数据写入日志
void WriteToLog(void *data, int len)
{
    if (gLogFileCurrent && data && len)
    {
        bool timetoswitch =  TimeToSwitch(len);	//是否需要切换日志
        if ( timetoswitch )		//切换日志
        {   
            if (gFileNum == 1)
            {
                if (gLogFile_1) fclose(gLogFile_1);
                gLogFile_1 = 0;
                gLogFile_2 = fopen("logfile_2.txt","w");
                gFileNum = 2;

                gLogFileCurrent = gLogFile_2;
                fseek(gLogFileCurrent , 0, SEEK_SET);
            }
            else
            {   
                if (gLogFile_2) fclose(gLogFile_2);
                gLogFile_2 = 0;
                gLogFile_1 = fopen("logfile_1.txt","w");
                gFileNum = 1;
            
                gLogFileCurrent = gLogFile_1;
                fseek(gLogFileCurrent , 0, SEEK_SET);
            }
        }
        fwrite(data, sizeof(char), len, gLogFileCurrent);	//把
        fflush(gLogFileCurrent);        
    }
}

//把包写入日志
void WritePacketToLog(void *data, int len)
{
    gLogNumPackets ++;
    LogUInt("Packet:", (UInt32) gLogNumPackets,"\n");
    LogBuffer();			//把gLogBuff中的数据写入日志
    WriteToLog(data, len);
}

//把数据写入缓存
void WritToBuffer(void *data, int len)
{
    if (data )
    {
        if (len >= gBuffSize)
            len = gBuffSize -1;
        memcpy (gLogBuff, (char *) data, len);
        gLogBuff[gBuffSize] = 0;
    }
   gBuffUsed = len;
}

//把gLogBuff中的数据写入日志, 并清空gLogBuff
void LogBuffer(void)
{   
    WriteToLog(gLogBuff, strlen(gLogBuff) );
    *gLogBuff =0;
    gBuffUsed = strlen(gLogBuff);
}

//打印缓存数据
void PrintLogBuffer(bool log)
{
    qtss_printf(gLogBuff);
    if (log) LogBuffer();
    *gLogBuff =0;
    gBuffUsed = strlen(gLogBuff);
}

//把三个字符串写入gLogBuff
void LogNum(char *str1,char *str2,char *str3)
{
    int size = strlen(str1) + strlen(str2) + strlen(str3);
    if ( size < gBuffMaxStr ) 
        qtss_sprintf(gLogBuff, "%s%s%s%s",gLogBuff, str1, str2,str3); 
   
   gBuffUsed = strlen(gLogBuff);
}

//写入浮点型数值
void  LogFloat(char *str, float num, char *str2)
{
    qtss_sprintf(gTempStr,"%f",num);
    LogNum(str,gTempStr,str2);
}

//写入数值
void LogInt(char *str, long num, char *str2)
{
    qtss_sprintf(gTempStr,"%ld",num);
    LogNum(str,gTempStr,str2);
}

//写入数值
void LogUInt (char *str, unsigned long num, char *str2)
{
    qtss_sprintf(gTempStr,"%lu",num);
    LogNum(str,gTempStr,str2);
}

//写入字符串
void LogStr(char *str)
{
   *gLogBuff = 0;
    int size = strlen(str) + gBuffUsed;
    if ( size <= gBuffMaxStr ) 
        qtss_sprintf(gLogBuff, "%s%s",gLogBuff, str); 
   gBuffUsed = strlen(gLogBuff);
}

#endif


SInt64  PlayListUtils::sInitialMsecOffset = 0;
UInt32  PlayListUtils::sRandomNum = 0;

void PlayListUtils::Initialize()
{
    if (sInitialMsecOffset != 0) return;
    sInitialMsecOffset = OS::Milliseconds(); //Milliseconds uses sInitialMsec to make a 0 offset millisecond so this assignment is valid only once.
    //qtss_printf("sInitialMsecOffset =%qd\n",sInitialMsecOffset);
    ::srand( (UInt16) OS::UnixTime_Secs() );
    sRandomNum =  ::rand();

}


PlayListUtils::PlayListUtils()
{   
}
//获得随机数
UInt32 PlayListUtils::Random()
{

    UInt32 seed = 1664525L * sRandomNum + 1013904223L; //1013904223 is prime .. Knuth D.E.      
    ::srand( seed );

    sRandomNum = ::rand();

    return sRandomNum;
}
