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
 *
 *  File:   timescale.c
 *
 *  History:
 *      23-Oct-1998 Umesh Vaishampayan (umeshv@apple.com)
 *          Created.
 */

#include "timestamp.h"
#include <stdlib.h>
#include "SafeStdLib.h"
void
utimescale(struct timescale *tscp)
{
// stop not supported
    char *death = 0;
    *death = 0;
    exit (-1);
#if 0 // old code
    unsigned int theNanosecondNumerator = 0;
    unsigned int theNanosecondDenominator = 0;

        MKGetTimeBaseInfo(NULL, &theNanosecondNumerator, &theNanosecondDenominator, NULL, NULL);
        tscp->tsc_numerator = theNanosecondNumerator / 1000; /* PPC magic number */
        tscp->tsc_denominator = theNanosecondDenominator;
        return;
#endif

}

