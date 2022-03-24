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
    File:       QTSSReflectorModule.h

    Contains:   QTSS API module 
                    
    

*/

#ifndef _QTSSRELAYMODULE_H_
#define _QTSSRELAYMODULE_H_

#include "QTSS.h"

//C和C++对函数的处理方式是不同的.extern "C"是使C++能够调用C写作的库文件的一个手段，
//如果要对编译器提示使用C的方式来处理函数的话，那么就要使用extern "C"来说明

extern "C"
{
    EXPORT QTSS_Error QTSSRelayModule_Main(void* inPrivateArgs);
} //Windows下的DLL、Linux下的库或是C++中调用C的库，均可能用到import/export

#endif //_QTSSRELAYMODULE_H_
