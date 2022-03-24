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


/* ====================================================================
getopt.h

 ��غ���
	
��ͷ�ļ�
	#include<unistd.h>
���庯��
	int getopt(int argc,char * const argv[ ],const char * optstring);
����˵��
	getopt()�������������в���������argc��argv����main()���ݵĲ������������ݡ�����optstring ������������ѡ���ַ������˺����᷵����argv ����һ����ѡ����ĸ������ĸ���Ӧ����optstring �е���ĸ�����ѡ���ַ��������ĸ�����ð�š�:�������ʾ������صĲ�����ȫ�����optarg ����ָ��˶�����������getopt()�Ҳ������ϵĲ������ӡ������Ϣ������ȫ�����optopt��Ϊ��?���ַ��������ϣ��getopt()ӡ������Ϣ����ֻҪ��ȫ�����opterr��Ϊ0���ɡ�
����ֵ
	����ҵ����ϵĲ����򷵻ش˲�����ĸ����������������ڲ���optstring ��ѡ����ĸ�򷵻ء�?���ַ������������򷵻�-1��

  http://man.chinaunix.net/develop/c&c++/linux_c/function/16.html

 */

#ifndef GETOPT_H
#define GETOPT_H

#ifdef __cplusplus
extern "C" {
#endif

//#ifdef __Win32__

extern char *optarg;
extern int optreset;
extern int optind;
extern int opterr;
extern int optopt;
int getopt(int argc, char* const *argv, const char *optstr);

//#endif /* WIN32 */

#ifdef __cplusplus
}
#endif

#endif /* GETOPT_H */
