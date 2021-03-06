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
    File:       OSMemory_Modules.cpp
	实现了内在分配等函数
*/

#include "OSMemory.h"
#include "QTSS.h"

#if MEMORY_DEBUGGING

void* operator new(size_t s, char* /*inFile*/, int /*inLine*/)
{
    return QTSS_New (FOUR_CHARS_TO_INT('0', '0', '0', '0'), s);
}

void* operator new[](size_t s, char* /*inFile*/, int /*inLine*/)
{
    return QTSS_New (FOUR_CHARS_TO_INT('0', '0', '0', '0'), s);
}

#else

void* operator new (size_t s)
{
    return QTSS_New (FOUR_CHARS_TO_INT('0', '0', '0', '0'), s);
}

void* operator new[](size_t s)
{
    return QTSS_New (FOUR_CHARS_TO_INT('0', '0', '0', '0'), s);
}

void operator delete(void* mem)
{
    QTSS_Delete(mem);
}

void operator delete[](void* mem)
{
    QTSS_Delete(mem);
}


#endif //__OS_MEMORY_H__

