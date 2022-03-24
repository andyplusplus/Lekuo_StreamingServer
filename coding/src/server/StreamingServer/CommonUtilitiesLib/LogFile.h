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

//LogFile.h  BywuZengDe

#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <afxmt.h>

#define MAX_ROW 50000

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
