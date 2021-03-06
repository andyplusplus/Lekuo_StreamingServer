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


/*    File:       QTSSMemoryDeleter.h
      Contains:   Auto object for deleting memory allocated by QTSS API callbacks, such as QTSS_GetValueAsString
      删除内存的自动对象，用于删除QTSS API分配的内存，如QTSS_GetValueAsString，当该对象删除时，关联的内存自动删除
*/

#ifndef __QTSS_MEMORY_DELETER_H__
#define __QTSS_MEMORY_DELETER_H__

#include "MyAssert.h"
#include "QTSS.h"

template <class T>
class QTSSMemoryDeleter
{
    public:
        QTSSMemoryDeleter(T* victim) : fT(victim)  {}
        ~QTSSMemoryDeleter() { QTSS_Delete(fT); }
        
        void ClearObject() { fT = NULL; }

        void SetObject(T* victim) 
        {
            Assert(fT == NULL);
            fT = victim; 
        }
        T* GetObject() { return fT; }
        
        operator T*() { return fT; }
    
    private:
    
        T* fT;
};


typedef QTSSMemoryDeleter<char> QTSSCharArrayDeleter;
//_WZD_ADD_Start  //tobe
//template <class T>
//class WZDArrayDeleter
//{
//public:
//	WZDArrayDeleter(T* victim) : fT(victim)  {}
//	~WZDArrayDeleter() { delete [] fT; }
//
//	void ClearObject() { fT = NULL; }
//
//	void SetObject(T* victim) 
//	{
//		Assert(fT == NULL);
//		fT = victim; 
//	}
//
//	T* GetObject() { return fT; }
//
//	operator T*() { return fT; }
//
//private:
//
//	T* fT;
//};
//
//typedef WZDArrayDeleter<char> QTSSCharArrayDeleter;
//_WZD_ADD_End



#endif //__QTSS_MEMORY_DELETER_H__


