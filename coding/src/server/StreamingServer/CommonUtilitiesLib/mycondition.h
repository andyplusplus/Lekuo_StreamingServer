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
    File:       mycondition.h


    好象和Windows无关
*/

#ifndef _MYCONDITION_H_
#define _MYCONDITION_H_


#include <mach/mach.h>
#include <pthread.h>

#include "mymutex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* mycondition_t;
mycondition_t mycondition_alloc();
void mycondition_free(mycondition_t);

void mycondition_broadcast(mycondition_t);
void mycondition_signal(mycondition_t);
void mycondition_wait(mycondition_t, mymutex_t, int); //timeout as a msec offset from now (0 means no timeout)

#ifdef __cplusplus
}
#endif

#endif