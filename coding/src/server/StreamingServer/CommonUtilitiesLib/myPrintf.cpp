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

/***   该函数由吴增德 增加  ADDED by _WUZENGDE_，函数的语义和sprintf相同，只不过把信息输出到Debug窗口
Print_to_Debug.h
myPrintf.cpp
*******************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#define MAXSTR INT_MAX

//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#define _OUTPUT_MYPRINTF_ 1


int myPrintf (	const char *format,    ...      )
{
    int retval = 1; 
	if(_OUTPUT_MYPRINTF_)
	{
		char strBuffer[1024];

        va_list arglist;
        va_start(arglist, format);
		//
        retval = vsprintf(strBuffer,format,arglist);
		va_end( arglist ); 
		//
		//以下把内容输出到Debug窗口
		
		OutputDebugString(strBuffer);
	}
	//
    return(retval);
}


int myPrintf_Nothing (	const char *format,    ...      )
{
	return 1;
}

int printf_Nothing (	const char *format,    ...      )
{
	return 1;
}


//2007.12.02 分析VideoRelayServer的文件的数据包
int WuZengDe_PrintPacketData(unsigned char* inBufferLen, unsigned long inBufferSize)
{
	unsigned char* p = inBufferLen;
	if (inBufferSize > 32)
	{
		printf("0x %2X %2X %2X %2X  %2X %2X %2X %2X  -  %2X %2X %2X %2X  %2X %2X %2X %2X  -  %2X %2X %2X %2X  %2X %2X %2X %2X  -  %2X %2X %2X %2X  %2X %2X %2X %2X \n", 
				p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],  p[10],p[11],p[12],p[13],p[14],p[15],p[16],p[17],p[18],p[19], 
				p[20],p[21],p[22],p[23],p[24],p[25],p[26],p[27],p[28],p[29],p[30],p[31]);

	}
	return 1; 
}   