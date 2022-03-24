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

LogFile.h

#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <afxmt.h>

#define MAX_ROW 5000

class CLog
{
public:
 void Disable();
 void Enable();
 CLog();
 ~CLog();
 void WriteLogFile(const char* fmt, ... );
protected:
 int NewLogFile(); 
 BOOL IsChangeDay();
 CString GetCurntTime();
private:
 int m_filehandle;
 int m_row;
 int m_dirnum;
 int m_filenum;
 BOOL m_needlog;
 char m_sContent[1024*1024];
 CString m_strContent;

 CString m_sCurtTime;
 CString m_sYestady;
 CString m_sToday;
 CString m_sCurtDir;
 CString m_sLogDir;
 CString m_sLogDayDir;

 CCriticalSection m_cs;
};

 

实现代码  LogFile.cpp


#include "stdafx.h"
#include "LogFile.h"

#include <direct.h>

static char  log_directory[]="ivr_log";

CLog::CLog()
{
 m_row = 0;
 m_filehandle = 0;
 m_dirnum = 0;
 m_filenum = 0;
 m_sCurtTime = _T("");
 m_sToday = _T("");
 m_sYestady = _T("");
 m_sCurtDir = _T("");
 m_sLogDir = _T("");
 m_needlog = FALSE;

 GetCurrentDirectory(_MAX_PATH,m_sCurtDir.GetBuffer(_MAX_PATH));
 m_sCurtDir.ReleaseBuffer();

 CString sDirName;
 CFileFind f;
 BOOL bHasLogDir = FALSE;
 BOOL bFind = f.FindFile(m_sCurtDir + "\\*.*");

 while (bFind)
 {
  bFind = f.FindNextFile();
  if (f.IsDots()) continue;
  if (f.IsDirectory())
  {
   sDirName = f.GetFileName();
   sDirName.MakeLower();
   if (sDirName == (CString)log_directory)
   {
    bHasLogDir = TRUE;
    break;
   }
  }
 }
 m_sLogDir = m_sCurtDir + (CString)"\\" + (CString)log_directory;
 if (!bHasLogDir)
 {
  _mkdir(m_sLogDir);
 }

 CString strTime = GetCurntTime ();

 m_sToday = strTime.Mid(0,10);
}

CLog::~CLog()
{
 _close(m_filehandle);
}

int CLog::NewLogFile()
{
 CString strLogfileName;

 if (m_filenum > 0)
 {
  _close(m_filehandle);
 }
 strLogfileName = m_sLogDayDir + (CString)"\\" + m_sCurtTime + (CString)".txt";
 if((m_filehandle=_open(strLogfileName,_O_WRONLY| _O_CREAT|O_TRUNC, _S_IREAD | _S_IWRITE)) == -1)
 {
  printf("cannot create log file:%s\n",strLogfileName);
  return -1;
 }
 m_filenum++;
 return 0;
}

BOOL CLog::IsChangeDay()
{
 m_sCurtTime  = GetCurntTime();
 m_sToday = m_sCurtTime.Mid(0,10);
 
 if (m_sToday != m_sYestady)
 {
  m_sYestady = m_sToday;
  m_sLogDayDir = m_sLogDir + (CString)"\\" + m_sToday;
  _mkdir(m_sLogDayDir);
  return TRUE;
 }
 return FALSE;
}

void CLog::WriteLogFile(const char* fmt, ... )
{
 if (m_needlog)
 {
  m_cs.Lock();

  if (m_row >= MAX_ROW ||  IsChangeDay())
  {
   if (NewLogFile() == -1)
   {
    m_cs.Unlock();
    return;
   }
   m_row = 0;
  }
  
  va_list v_args;
  va_start(v_args,fmt);
  vsprintf( m_sContent, fmt, v_args );
  va_end( v_args );
  
  CString strTime;
  CTime cTime = CTime::GetCurrentTime();
  strTime = _T("20") + cTime.Format("%y-%m-%d %H:%M:%S");
/*
  char sTime[32];
  time_t now = time(NULL);
  tm *tm_time = localtime(&now);    //注意，localtime非线程安全
  strftime(sTime,sizeof(sTime),"20%y-%m-%d %H:%M:%S",tm_time);
*/
  m_strContent = (CString)"[" + strTime + (CString)"]" + (CString)m_sContent + "\n";

  _write(m_filehandle,m_strContent,m_strContent.GetLength());
  m_row++;

  m_cs.Unlock();
 }
}

CString CLog::GetCurntTime()
{
 CString strTime;

 CTime cTime = CTime::GetCurrentTime();
 strTime = _T("20") + cTime.Format("%y-%m-%d-%H-%M-%S");

 return strTime;
}

void CLog::Enable()
{
 m_needlog = TRUE;
}

void CLog::Disable()
{
 m_needlog = FALSE;
}
