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
    
    enum {switchEntries, switchTime, switchBytes};		//�ֱ��ǽӱ�����, �����Ƿ��л���־, ��ʱ�����, ���ֽ�������
    
    enum {gLogMaxBytes = 100000};
    enum {gLogMaxEntries = 1000};
    enum {gLogMaxMilliSecs = 3 * 60 * 1000};


    static int gLogSwitchSetting = switchEntries;	//�ֱ��ǽӱ�����, �����Ƿ��л���־, ��ʱ�����, ���ֽ�������
    static long gLogEntries  = 0;		//�Ѿ���¼����־��
    static SInt64 gLogTimeStart = 0;	//��ʼ��¼��־��ʱ��
    static long gLogNumBytes = 0;		//��¼�����ֽ���		
    static gLogNumPackets = 0;			//�ܰ���
    static char gTempStr[256];			//��ʱ�ַ���
    static FILE *gLogFile_1 = 0;		//��־�ļ�1���
    static FILE *gLogFile_2 = 0;		//��־�ļ�2
    static FILE *gLogFileCurrent = 0;	//��ǰ��־�ļ�
    static int gFileNum = 1;			//��ǰ���ڼ�¼�ĸ��ļ�, 1��ʾ��¼��ǰ�ļ�
    
    static bool gLogStarted = false;	//�Ƿ��Ѿ���ʼ��¼��־��
    static char gLogBuff[gBuffSize];	//logд��Ļ���
    static int gBuffUsed = 0;			//�Ѿ�ʹ�õ�Buffer��С


//����־�ļ�
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

//�ر���־�ļ�
void    LogFileClose(void)
{
    fclose(gLogFile_1);
    fclose(gLogFile_2);
    gLogFileCurrent = gLogFile_1 = gLogFile_2 = 0;
    
}

//���������Ѿ���¼����־��, �����Ƿ���Ҫ�л���־, �����ظþ���
bool TimeToSwitch(int len)
{
    bool timeToSwitch = false;
    if (!gLogStarted) return timeToSwitch;
      
    switch( gLogSwitchSetting )		//�ֱ��ǽӱ�����, �����Ƿ��л���־, ��ʱ�����, ���ֽ�������
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

//������д����־
void WriteToLog(void *data, int len)
{
    if (gLogFileCurrent && data && len)
    {
        bool timetoswitch =  TimeToSwitch(len);	//�Ƿ���Ҫ�л���־
        if ( timetoswitch )		//�л���־
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
        fwrite(data, sizeof(char), len, gLogFileCurrent);	//��
        fflush(gLogFileCurrent);        
    }
}

//�Ѱ�д����־
void WritePacketToLog(void *data, int len)
{
    gLogNumPackets ++;
    LogUInt("Packet:", (UInt32) gLogNumPackets,"\n");
    LogBuffer();			//��gLogBuff�е�����д����־
    WriteToLog(data, len);
}

//������д�뻺��
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

//��gLogBuff�е�����д����־, �����gLogBuff
void LogBuffer(void)
{   
    WriteToLog(gLogBuff, strlen(gLogBuff) );
    *gLogBuff =0;
    gBuffUsed = strlen(gLogBuff);
}

//��ӡ��������
void PrintLogBuffer(bool log)
{
    qtss_printf(gLogBuff);
    if (log) LogBuffer();
    *gLogBuff =0;
    gBuffUsed = strlen(gLogBuff);
}

//�������ַ���д��gLogBuff
void LogNum(char *str1,char *str2,char *str3)
{
    int size = strlen(str1) + strlen(str2) + strlen(str3);
    if ( size < gBuffMaxStr ) 
        qtss_sprintf(gLogBuff, "%s%s%s%s",gLogBuff, str1, str2,str3); 
   
   gBuffUsed = strlen(gLogBuff);
}

//д�븡������ֵ
void  LogFloat(char *str, float num, char *str2)
{
    qtss_sprintf(gTempStr,"%f",num);
    LogNum(str,gTempStr,str2);
}

//д����ֵ
void LogInt(char *str, long num, char *str2)
{
    qtss_sprintf(gTempStr,"%ld",num);
    LogNum(str,gTempStr,str2);
}

//д����ֵ
void LogUInt (char *str, unsigned long num, char *str2)
{
    qtss_sprintf(gTempStr,"%lu",num);
    LogNum(str,gTempStr,str2);
}

//д���ַ���
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
//��������
UInt32 PlayListUtils::Random()
{

    UInt32 seed = 1664525L * sRandomNum + 1013904223L; //1013904223 is prime .. Knuth D.E.      
    ::srand( seed );

    sRandomNum = ::rand();

    return sRandomNum;
}
