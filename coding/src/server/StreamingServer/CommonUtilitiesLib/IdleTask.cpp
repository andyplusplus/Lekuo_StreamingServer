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
    File:       IdleTask.cpp

    Contains:   IdleTasks are identical to normal tasks (see task.h) with one exception:
    
                You can schedule them for timeouts. If you call SetIdleTimer
                on one, after the time has elapsed the task object will receive an
                OS_IDLE event. 
                
    

*/

#include "IdleTask.h"
#include "OSMemory.h"
#include "OS.h"

//IDLETASKTHREAD IMPLEMENTATION:
IdleTaskThread*     IdleTask::sIdleThread = NULL;

//该函数把任务加入堆，并把任务的关键字(时间)设置为msec以后的某个时刻，然后把任务加入堆
void IdleTaskThread::SetIdleTimer(IdleTask *activeObj, SInt64 msec)
{
    //note: OSHeap doesn't support a random remove, so this function
    //won't change the timeout value if there is already one set
	//注: 由于OSHeap不支持随机移动，所以如果timeout值已经设定，该函数不改变timeout的值
    if (activeObj->fIdleElem.IsMemberOfAnyHeap())
        return;
    activeObj->fIdleElem.SetValue(OS::Milliseconds() + msec);
    
    {
        OSMutexLocker locker(&fHeapMutex);
        fIdleHeap.Insert(&activeObj->fIdleElem);
    }
     //此处把IdleTask插入堆，并传信fHeapCond，Entry()函数接收信号，并取出IdleTask
     //然后Entry函数传信IdleTask
    fHeapCond.Signal();
}


//从Heap中移去Idle任务对象，IdleTask的析构函数调用以下函数，从而移去未处理的对象
void IdleTaskThread::CancelTimeout(IdleTask* idleObj)
{
    Assert(idleObj != NULL);
    OSMutexLocker locker(&fHeapMutex);
    fIdleHeap.Remove(&idleObj->fIdleElem);  
}

//Entry函数对Heap中的数据进行处理
void IdleTaskThread::Entry()
{
    OSMutexLocker locker(&fHeapMutex);

    //循环取出所有需执行的任务，直到堆中的所有任务都已经执行完毕
    while (true)
    {

		//_WZD_ADD_Start
		if (IsStopRequested())
		{
			return;
		}
		//_WZD_ADD_End

    //1. 如果没有可处理任务，则阻塞并等待
        //if there are no events to process, block.
        if (fIdleHeap.CurrentHeapSize() == 0)
            fHeapCond.Wait(&fHeapMutex);
        SInt64 msec = OS::Milliseconds();
    //2. 只要元素的timeout时间已经到了，则把元素弹出heap，并执行IdleTask的Task::kIdleEvent        
        //pop elements out of the heap as long as their timeout time has arrived
        while ((fIdleHeap.CurrentHeapSize() > 0) && (fIdleHeap.PeekMin()->GetValue() <= msec))
        {
            IdleTask* elem = (IdleTask*)fIdleHeap.ExtractMin()->GetEnclosingObject();
            Assert(elem != NULL);
            elem->Signal(Task::kIdleEvent);
        }
                        
        //we are done sending idle events. If there is a lowest tick count, then we need to sleep until that time.
    //3. 已经发送了idle事件, 如果有一个最小的时钟值, 则体眠到该时刻.
        if (fIdleHeap.CurrentHeapSize() > 0)
        {
            SInt64 timeoutTime = fIdleHeap.PeekMin()->GetValue();
            //because sleep takes a 32 bit number
            timeoutTime -= msec;
            Assert(timeoutTime > 0);
            UInt32 smallTime = (UInt32)timeoutTime;
            fHeapCond.Wait(&fHeapMutex, smallTime);
        }
    }   
}

void IdleTask::Initialize()
{
    if (sIdleThread == NULL)
    {
        sIdleThread = NEW IdleTaskThread();
        sIdleThread->Start();
    }
}

//_WZD_ADD_Start'
void IdleTask::WZD_SignalIdleThreadExit()
{
	if (sIdleThread)
	{
		sIdleThread->SendStopRequest();
		sIdleThread->SignalHeapCond();
		Sleep(10);
	}
}

void IdleTask::UnInitialize()
{
	if (sIdleThread)
	{
		sIdleThread->SendStopRequest();
		Sleep(10);
		delete sIdleThread;
		sIdleThread	= NULL;
	}
}
//_WZD_ADD_End


IdleTask::~IdleTask()
{
    //clean up stuff used by idle thread routines

	Assert(sIdleThread != NULL);
    
    OSMutexLocker locker(&sIdleThread->fHeapMutex);

    //Check to see if there is a pending timeout. If so, get this object
    //out of the heap
    if (fIdleElem.IsMemberOfAnyHeap())
        sIdleThread->CancelTimeout(this);

}



