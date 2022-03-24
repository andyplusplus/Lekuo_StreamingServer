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
    File:       OSRef.cpp

    Contains:   Implementation of OSRef class. 
                    

*/

#include "OSRef.h"

#include <errno.h>

UInt32  OSRefTableUtils::HashString(StrPtrLen* inString)
{
    Assert(inString != NULL);
    Assert(inString->Ptr != NULL);
    Assert(inString->Len > 0);
    
    //make sure to convert to unsigned here, as there may be binary
    //data in this string
    UInt8* theData = (UInt8*)inString->Ptr;
    
    //divide by 4 and take the characters at quarter points in the string,
    //use those as the basis for the hash value
    UInt32 quarterLen = inString->Len >> 2;
    return (inString->Len * (theData[0] + theData[quarterLen] +
            theData[quarterLen * 2] + theData[quarterLen * 3] +
            theData[inString->Len - 1]));
}

//如果发现同一Item，返回EPERM
OS_Error OSRefTable::Register(OSRef* inRef)
{
    Assert(inRef != NULL);
#if DEBUG
    Assert(!inRef->fInATable);
#endif
    Assert(inRef->fRefCount == 0);			//首先确保注册对象的引用计数器为0 
    
    OSMutexLocker locker(&fMutex);

    // Check for a duplicate. In this function, if there is a duplicate,
    // return an error, don't resolve the duplicate
    OSRefKey key(&inRef->fString);			//然后检查是否已经有该哈希表项
    OSRef* duplicateRef = fTable.Map(&key);
    if (duplicateRef != NULL)
        return EPERM;
        
    // There is no duplicate, so add this ref into the table
#if DEBUG
    inRef->fInATable = true;
#endif
    fTable.Add(inRef);					//如果没有哈希表项，则把引用加入到列表中
    return OS_NoErr;
}

//该函数的总体实现与以上函数类似，只不过先以inRef->fString作为关键字查找OSRef，如果返回不为空，
//则把返回找到的对象，则把引用加入到OSRefTable。反之把OSRef加入到OSRefTable中
OSRef* OSRefTable::RegisterOrResolve(OSRef* inRef)
{
    Assert(inRef != NULL);
#if DEBUG
    Assert(!inRef->fInATable);
#endif
    Assert(inRef->fRefCount == 0);
    
    OSMutexLocker locker(&fMutex);

    // Check for a duplicate. If there is one, resolve it and return it to the caller
    OSRef* duplicateRef = this->Resolve(&inRef->fString);
    if (duplicateRef != NULL)
        return duplicateRef;

    // There is no duplicate, so add this ref into the table
#if DEBUG
    inRef->fInATable = true;
#endif
    fTable.Add(inRef);
    return NULL;
}
//UnRegister函数用于从fTable中删除引用表项，首先检查表项的引用计数是否大于refCount，如果是，
//则等待引用计数的减少，然后删除该引用。
void OSRefTable::UnRegister(OSRef* ref, UInt32 refCount)
{
    Assert(ref != NULL);
    OSMutexLocker locker(&fMutex);

    //make sure that no one else is using the object
    while (ref->fRefCount > refCount)
        ref->fCond.Wait(&fMutex);	//确保没有使用该对象
    
#if DEBUG
    OSRefKey key(&ref->fString);
    if (ref->fInATable)
        Assert(fTable.Map(&key) != NULL);
    ref->fInATable = false;
#endif
    
    //ok, we now definitely have no one else using this object, so
    //remove it from the table
    fTable.Remove(ref);				//从哈希表中删除该对象
}

//该函数与UnRegister函数类似，只不过当引用计数是否大于refCount时，立即返回false，反之，从哈希表中删除表项，并返回true
Bool16 OSRefTable::TryUnRegister(OSRef* ref, UInt32 refCount)
{
    OSMutexLocker locker(&fMutex);
    if (ref->fRefCount > refCount)
        return false;
    
    // At this point, this is guarenteed not to block, because
    // we've already checked that the refCount is low.
    this->UnRegister(ref, refCount);
    return true;
}


OSRef*  OSRefTable::Resolve(StrPtrLen* inUniqueID)
{
    Assert(inUniqueID != NULL);
    OSRefKey key(inUniqueID);

    //this must be done atomically wrt the table
    OSMutexLocker locker(&fMutex);
    OSRef* ref = fTable.Map(&key);
    if (ref != NULL)
    {
        ref->fRefCount++;
        Assert(ref->fRefCount > 0);
    }
    return ref;
}
//以下函数减少引用计数，然后触发条件
void    OSRefTable::Release(OSRef* ref)
{
    Assert(ref != NULL);
    OSMutexLocker locker(&fMutex);
    ref->fRefCount--;
    // fRefCount is an unsigned long and QTSS should never run into
    // a ref greater than 16 * 64K, so this assert just checks to
    // be sure that we have not decremented the ref less than zero.
    Assert( ref->fRefCount < 1048576L );
    //make sure to wakeup anyone who may be waiting for this resource to be released
    ref->fCond.Signal();
}

//以下函数用新的表项替换现有的表项。
void    OSRefTable::Swap(OSRef* newRef)
{
    Assert(newRef != NULL);
    OSMutexLocker locker(&fMutex);
    
    OSRefKey key(&newRef->fString);
    OSRef* oldRef = fTable.Map(&key);
    if (oldRef != NULL)
    {
        fTable.Remove(oldRef);
        fTable.Add(newRef);
#if DEBUG
        newRef->fInATable = true;
        oldRef->fInATable = false;
        oldRef->fSwapCalled = true;
#endif
    }
    else
        Assert(0);
}

