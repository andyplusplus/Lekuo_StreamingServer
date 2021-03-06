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
    File:       QTSSRollingLog.h

    Contains:   A log toolkit, log can roll either by time or by size, clients
                must derive off of this object ot provide configuration information. 

   该类是一个日志工具类, 日志可通过时间, 或通过日志大小来记录, 客户必须继存该类, 以提供一些配置信息

*/

#ifndef __QTSS_ROLLINGLOG_H__
#define __QTSS_ROLLINGLOG_H__

#include <stdio.h>
#include <time.h>
#ifndef __Win32__
#include <sys/time.h>
#endif
#include "OSHeaders.h"
#include "OSMutex.h"
#include "Task.h"

const Bool16 kAllowLogToRoll = true;

class QTSSRollingLog : public Task
{
    public:
    
        //pass in whether you'd like the log roller to log errors.
        QTSSRollingLog();
        
        //
        // Call this to delete. Closes the log and sends a kill event
        void    Delete()
            { CloseLog(false); this->Signal(Task::kKillEvent); }
        
        //
        // Write a log message, 写入一个日志
        void    WriteToLog(char* inLogData, Bool16 allowLogToRoll);
        
        //log rolls automatically based on the configuration criteria,
        //but you may roll the log manually by calling this function.
        //Returns true if no error, false otherwise
        Bool16  RollLog();//滚动日志, 把原有的日志关闭, 并重命名该日志, 打开新的日志

        //
        // Call this to open the log file and begin logging     
        void EnableLog( Bool16 appendDotLog = true);	//打开日志文件, 并开始写日志
        
                //
        // Call this to close the log
        // (pass leaveEnabled as true when we are temporarily closing.)
        void CloseLog( Bool16 leaveEnabled = false);//关闭日志

        //
        //mainly to check and see if errors occurred
        Bool16  IsLogEnabled();
        
        //master switch
        Bool16  IsLogging() { return fLogging; }
        void  SetLoggingEnabled( Bool16 logState ) { fLogging = logState; }
        
        //General purpose utility function
        //returns false if some error has occurred
        static Bool16   FormatDate(char *ioDateBuffer, Bool16 logTimeInGMT);//格式化日期
        
        // Check the log to see if it needs to roll
        // (rolls the log if necessary)
        Bool16          CheckRollLog();//检查是否需要滚动日志
        
        // Set this to true to get the log to close the file between writes.
        static void		SetCloseOnWrite(Bool16 closeOnWrite);  //如果为true, 则每次写日志以后, 关闭文件

        enum
        {
            kMaxDateBufferSizeInBytes = 30, //UInt32
            kMaxFilenameLengthInBytes = 31  //UInt32
        };
    
    protected:

        //
        // Task object. Do not delete directly
        virtual ~QTSSRollingLog();

        //Derived class must provide a way to get the log & rolled log name
        virtual char* GetLogName() = 0;
        virtual char* GetLogDir() = 0;
        virtual UInt32 GetRollIntervalInDays() = 0;//0 means no interval
        virtual UInt32 GetMaxLogBytes() = 0;//0 means unlimited
                    
        //to record the time the file was created (for time based rolling)
        virtual time_t  WriteLogHeader(FILE *inFile);	//写入日志头: 主要是写入日志创建时间
        time_t          ReadLogHeader(FILE* inFile);	//返回读出的日志创建时间

    private:
    
        //
        // Run function to roll log right at midnight   
        virtual SInt64      Run();   //定期检查是否需要滚动日志

        FILE*           fLog;
        time_t          fLogCreateTime;
        char*           fLogFullPath;		//带名字的路径
        Bool16          fAppendDotLog;		//表示是否要在文件名后增加".log"
        Bool16          fLogging;
        Bool16          RenameLogFile(const char* inFileName);	//重命名日志文件
        Bool16          DoesFileExist(const char *inPath);		//文件是否存在
        static void     ResetToMidnight(time_t* inTimePtr, time_t* outTimePtr);	//获得对应时间的午夜时间值
        char*           GetLogPath(char *extension);			//根据扩展名返回带名字的路径
        
        // To make sure what happens in Run doesn't also happen at the same time
        // in the public functions.
        OSMutex         fMutex;
};

#endif // __QTSS_ROLLINGLOG_H__

