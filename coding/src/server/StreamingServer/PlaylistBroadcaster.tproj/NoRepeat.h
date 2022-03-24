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
   NoRepeat.h
 */


#ifndef __no_repeat__
#define __no_repeat__


#include "PLDoubleLinkedList.h"
#include "SimplePlayListElement.h"

#include "OSHeaders.h"


class NoRepeat : public PLDoubleLinkedList<SimplePlayListElement> {

    public: 
		//构造函数, 设置最大的允许结点数
        NoRepeat( UInt32 numNoRepeats );

        virtual ~NoRepeat();

        //该名字的结点是否在列表中
        bool IsInList( char* name ); // return true if name is in list, false if not

		//把特定名字的结点入到列表中,包括两个步骤:
		//   1. 为新结点申请空间
		//   2. 把该结点加入到列表
        bool AddToList( char* name );// return true if could be added to list, no dupes allowd

		//把结点加入列表
        PLDoubleLinkedListNode<SimplePlayListElement>* AddToList( PLDoubleLinkedListNode<SimplePlayListElement>* node );

    protected:
		//按名字比较
        static bool     CompareNameToElement( PLDoubleLinkedListNode<SimplePlayListElement>*node, void *name );
        UInt32          mMaxElements;		//设置最大的允许结点数

};

#endif
