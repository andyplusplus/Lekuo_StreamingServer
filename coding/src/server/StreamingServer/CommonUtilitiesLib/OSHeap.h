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
    File:       OSHeap.h

    Contains:   Implements a heap
                    
	该文件包含OPHeap类以及OSHeapElem类，OPHeap维护了一个OSHeapElem数组，OSHeapElem类的对象包括三个成员变量
    见代码分析.doc
*/

#ifndef _OSHEAP_H_
#define _OSHEAP_H_

#define _OSHEAP_TESTING_ 0

#include "OSCond.h"
#include "assert.h"

class OSHeapElem;

class OSHeap
{
    public:
    
        enum
        {
            kDefaultStartSize = 1024 //UInt32
        };

      
        OSHeap(UInt32 inStartSize = kDefaultStartSize);
        ~OSHeap() { 
			if (fHeap != NULL)
			{
				//   _WZD_MEM_Start
				UInt32 theSize = CurrentHeapSize();
				assert (theSize == 0);
				//qtss_printf("The HeapSize is %d\n", theSize);
				//   _WZD_MEM_End
				delete fHeap; //[] 
			}
		}
        
        //ACCESSORS
        UInt32      CurrentHeapSize() { return fFreeIndex - 1; }
        OSHeapElem* PeekMin() { if (CurrentHeapSize() > 0) return fHeap[1]; return NULL; }
        
        //MODIFIERS
        
        //These are the two primary operations supported by the heap
        //abstract data type. both run in log(n) time.
        void            Insert(OSHeapElem*  inElem);
        OSHeapElem*     ExtractMin() { return Extract(1); }
        //removes specified element from the heap
        OSHeapElem*     Remove(OSHeapElem* elem);
        
#if _OSHEAP_TESTING_
        //returns true if it passed the test, false otherwise
        static Bool16       Test();
#endif
    
    private:
    
        OSHeapElem*     Extract(UInt32 index);
    
#if _OSHEAP_TESTING_
        //verifies that the heap is in fact a heap
        void            SanityCheck(UInt32 root);
#endif
    
        OSHeapElem**    fHeap;		
        UInt32          fFreeIndex;			//Index 0 作为元素对象
        UInt32          fArraySize;
};

class OSHeapElem
{
    public:
        OSHeapElem(void* enclosingObject = NULL)
            : fValue(0), fEnclosingObject(enclosingObject), fCurrentHeap(NULL) {}
        ~OSHeapElem() {}
        
        //This data structure emphasizes performance over extensibility If it were properly object-oriented, the compare routine would
        //be virtual. However, to avoid the use of v-functions in this data structure, I am assuming that the objects are compared using a 64 bit number.
        //相对于可扩展性来说，该数据结构更强调性能。如果适当地做面向对象处理，比较过程
        //可做成虚函数。然而，为避免在此数据结构中采用v函数，确信对象采用64 bit数字表示。

        void    SetValue(SInt64 newValue) { fValue = newValue; }
        SInt64  GetValue()              { return fValue; }
        void*   GetEnclosingObject()    { return fEnclosingObject; }
		void	SetEnclosingObject(void* obj) { fEnclosingObject = obj; }
        Bool16  IsMemberOfAnyHeap()     { return fCurrentHeap != NULL; }
        
    private:
    
        SInt64  fValue;
        void* fEnclosingObject;
        OSHeap* fCurrentHeap;
        
        friend class OSHeap;
};
#endif //_OSHEAP_H_
