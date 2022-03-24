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
    File:       OSBufferPool.cpp

    Contains:   Fast access to fixed size buffers.

    通过队列实现固定大小的Buffer的快速访问
	
    Written By: Denis Serenyi
    
*/

#include "OSBufferPool.h"
#include "OSMemory.h"

//如果队列中有数据, 则从队列中取出数据, 否则新建一个数据对象, 并返回该对象
void*   OSBufferPool::Get()
{
    OSMutexLocker locker(&fMutex);
    if (fQueue.GetLength() == 0)
    {
        fTotNumBuffers++;
        char* theNewBuf = NEW char[fBufSize + sizeof(OSQueueElem)];

		//_WZD_Comment_Off_Start	//2008.05.30 Since I Comment off "inline void* operator new(size_t, void* ptr) { return ptr;}" in OsMemory.h, there is error in the follow sentence, so i comment off the follow sentence
        //(void)new (theNewBuf) OSQueueElem(theNewBuf + sizeof(OSQueueElem));  // We need to construct a Queue Element, but we don't actually need to use it in this function, so to avoid a compiler warning just don't assign the result to anything.
		//_WZD_Comment_Off_End
        return theNewBuf + sizeof(OSQueueElem);
    }
    return fQueue.DeQueue()->GetEnclosingObject();
}

//把数据存入队列
void OSBufferPool::Put(void* inBuffer)
{
    OSMutexLocker locker(&fMutex);
    fQueue.EnQueue((OSQueueElem*)((char*)inBuffer - sizeof(OSQueueElem)));		//类的数据存贮对象是如何组织的
}
