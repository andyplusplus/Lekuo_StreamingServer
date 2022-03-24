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
    File:       Task.cpp

    Contains:   implements Task class
                    
    
*/

#include "Task.h"
#include "OS.h"
#include "OSMemory.h"
#include "atomic.h"
#include "OSMutexRW.h"

//_WZD_Signal_Start
#define _WZD_Signal_ 1
//_WZD_Signal_End

unsigned int    Task::sThreadPicker = 0;
OSMutexRW       TaskThreadPool::sMutexRW;
static char* sTaskStateStr="live_"; //Alive

Task::Task()
:   fEvents(0), fUseThisThread(NULL), fWriteLock(false), fTimerHeapElem(), fTaskQueueElem()
{
#if DEBUG
    fInRunCount = 0;
#endif
    this->SetTaskName("unknown");

	fTaskQueueElem.SetEnclosingObject(this);
	fTimerHeapElem.SetEnclosingObject(this);
}

void Task::SetTaskName(char* name) 
{
    if (name == NULL) 
        return;
   
   ::strncpy(fTaskName,sTaskStateStr,sizeof(fTaskName));
   ::strncat(fTaskName,name,sizeof(fTaskName));
   fTaskName[sizeof(fTaskName) -1] = 0; //terminate in case it is longer than ftaskname.
   
}

Bool16 Task::Valid()
{
    if  (   (this->fTaskName == NULL)
         || (0 != ::strncmp(sTaskStateStr,this->fTaskName, 5))
         )
     {
        if (TASK_DEBUG) qtss_printf(" Task::Valid Found invalid task = %ld\n", this);
        
        return false;
     }
    
    return true;
}

Task::EventFlags Task::GetEvents()
{
    //Mask off every event currently in the mask except for the alive bit, of course,
    //which should remain unaffected and unreported by this call.
    EventFlags events = fEvents & kAliveOff;
    (void)atomic_sub(&fEvents, events);

	//_WZD_Signal_Start
	#if _WZD_Signal_
		if (events !=kIdleEvent)
		{
			qtss_printf("  Task::GetEvents()==> ");
			printEvent(events);
		}
	#endif
	//_WZD_Signal_End

    return events;
}

void Task::Signal(EventFlags events)
{

	//_WZD_Signal_Start
	#if _WZD_Signal_
			printEvent(events);
	#endif
	//_WZD_Signal_End


    if (!this->Valid())
        return;
        
    //Fancy no mutex implementation. We atomically mask the new events into
    //the event mask. Because atomic_or returns the old state of the mask,
    //we only schedule this task once.
    events |= kAlive;
    EventFlags oldEvents = atomic_or(&fEvents, events);
    if ((!(oldEvents & kAlive)) && (TaskThreadPool::sNumTaskThreads > 0))  //如果该事件是Alive且，有服务线程TaskThreadPool::sNumTaskThreads
    {
        if (fUseThisThread != NULL)
            //任务需要放置在特定线程的队列中
         {
            if (TASK_DEBUG) if (fTaskName[0] == 0) ::strcpy(fTaskName, " corrupt task");
            if (TASK_DEBUG) qtss_printf("  Task::Signal enque TaskName=%s fUseThisThread=%lu q elem=%lu enclosing=%lu\n\n", fTaskName, (UInt32) fUseThisThread, (UInt32) &fTaskQueueElem, (UInt32) this);
            fUseThisThread->fTaskQueue.EnQueue(&fTaskQueueElem);
        }
        else
        {
            //找到一个线程来放置任务
            unsigned int theThread = atomic_add(&sThreadPicker, 1);
            theThread %= TaskThreadPool::sNumTaskThreads;
            if (TASK_DEBUG) if (fTaskName[0] == 0) ::strcpy(fTaskName, " corrupt task");
            if (TASK_DEBUG) qtss_printf("  Task::Signal enque TaskName=%s thread=%lu q elem=%lu enclosing=%lu\n\n", fTaskName, (UInt32)TaskThreadPool::sTaskThreadArray[theThread],(UInt32) &fTaskQueueElem,(UInt32) this);
            TaskThreadPool::sTaskThreadArray[theThread]->fTaskQueue.EnQueue(&fTaskQueueElem);
        }
    }
    else
		//不存在该事件对应的任务
        if (TASK_DEBUG) qtss_printf("  Task::Signal sent to dead TaskName=%s  q elem=%lu  enclosing=%lu\n\n",  fTaskName, (UInt32) &fTaskQueueElem, (UInt32) this);
        

}


void Task::GlobalUnlock()    
{   
    if (this->fWriteLock)
    {   this->fWriteLock = false;   
        TaskThreadPool::sMutexRW.Unlock();
    }                                               
}


//作为OSThread的子类，TaskThread重载了Entry函数，一旦TaskThread的对象被实例化，便运行该函数。
//	Entry()函数的主要任务就是调用WaitForTask()函数监测任务队列，如果发现新任务，就在规定时间内执行；否则，就被阻塞。
void TaskThread::Entry()
{
    Task* theTask = NULL;
    
    while (true) 
    {
        theTask = this->WaitForTask();

        // 监测是否有需要执行的任务，如果有就返回该任务；否则阻塞           // WaitForTask returns NULL when it is time to quit
        if (theTask == NULL || false == theTask->Valid() )
            return;
                    
        Bool16 doneProcessingEvent = false;	//尚未处理事件
        
        while (!doneProcessingEvent)        //该循环一直运行到结束
        {
            //If a task holds locks when it returns from its Run function,
            //that would be catastrophic and certainly lead to a deadlock
#if DEBUG
            Assert(this->GetNumLocksHeld() == 0);
            Assert(theTask->fInRunCount == 0);
            theTask->fInRunCount++;
#endif
            theTask->fUseThisThread = NULL; // 对任务的调度必须独立地请求一个线程 // Each invocation of Run must independently request a specific thread.
            SInt64 theTimeout = 0;  //Task中Run函数的返回值，重要
                                    //核心部分：运行任务，根据返回值判断任务进度
            if (theTask->fWriteLock)
            {   //如果任务中有写锁，需要使用写互斥量，否则可能造成死锁
                OSMutexWriteLocker mutexLocker(&TaskThreadPool::sMutexRW);
                if (TASK_DEBUG) qtss_printf("\nTaskThread::Entry run global locked TaskName=%s CurMSec=%.3f thread=%ld task=%ld\n", theTask->fTaskName, OS::StartTimeMilli_Float() ,(SInt32) this,(SInt32) theTask);
                
                theTimeout = theTask->Run();	//运行任务，得到返回值
                theTask->fWriteLock = false;
            }
            else	//使用读互斥量, 第一次fWriteLock为False
            {
                OSMutexReadLocker mutexLocker(&TaskThreadPool::sMutexRW);
                if (TASK_DEBUG) qtss_printf("\nTaskThread::Entry run TaskName=%s CurMSec=%.3f thread=%ld task=%ld\n", theTask->fTaskName, OS::StartTimeMilli_Float(), (SInt32) this,(SInt32) theTask);

                theTimeout = theTask->Run();	//运行任务，得到返回值
            
            }
#if DEBUG
            Assert(this->GetNumLocksHeld() == 0);
            theTask->fInRunCount--;
            Assert(theTask->fInRunCount == 0);
#endif          
		//1、返回负数，表明任务已经完全结束，删除任务对象
            if (theTimeout < 0)   
            {
                if (TASK_DEBUG) 
                {
                    qtss_printf("TaskThread::Entry delete TaskName=%s CurMSec=%.3f thread=%ld task=%ld\n", theTask->fTaskName, OS::StartTimeMilli_Float(), (SInt32) this, (SInt32) theTask);
                     
                    theTask->fUseThisThread = NULL;
                    //从队列和堆中删除任务
                    if (NULL != fHeap.Remove(&theTask->fTimerHeapElem)) 
                        qtss_printf("TaskThread::Entry task still in heap before delete\n");
                    
                    if (NULL != theTask->fTaskQueueElem.InQueue())
                        qtss_printf("TaskThread::Entry task still in queue before delete\n");
                    
                    theTask->fTaskQueueElem.Remove();
                    
                    if (theTask->fEvents &~ Task::kAlive)
                        qtss_printf ("TaskThread::Entry flags still set  before delete\n");

                    (void)atomic_sub(&theTask->fEvents, 0);
                     
                    ::strncat (theTask->fTaskName, " deleted", sizeof(theTask->fTaskName) -1);
                }
                theTask->fTaskName[0] = 'D'; //mark as dead
                delete theTask;
                theTask = NULL;
                doneProcessingEvent = true;

            }
		//2、返回0，表明任务希望在下次传信时被再次立即执行
            else if (theTimeout == 0)
            {
                //We want to make sure that 100% definitely the task's Run function WILL be invoked when another thread calls Signal. We also want to make sure
                //that if an event sneaks in right as the task is returning from Run() (via Signal) that the Run function will be invoked again.
                doneProcessingEvent = compare_and_store(Task::kAlive, 0, &theTask->fEvents);
                if (doneProcessingEvent)
                    theTask = NULL; 
            }
		//3、返回正数，表明任务希望在等待theTimeout时间后再次执行
            else
            {   //将该任务加入到Heap中，并且纪录它希望等待的时间。Entry()函数将通过waitfortask()函数进行检测，如果等待的时间到了，就再次运行该任务
                //note that if we get here, we don't reset theTask, so it will get passed into WaitForTask
                if (TASK_DEBUG) qtss_printf("TaskThread::Entry insert TaskName=%s in timer heap thread=%lu elem=%lu task=%ld timeout=%.2f\n", theTask->fTaskName,  (UInt32) this, (UInt32) &theTask->fTimerHeapElem,(SInt32) theTask, (float)theTimeout / (float) 1000);
                theTask->fTimerHeapElem.SetValue(OS::Milliseconds() + theTimeout);
                fHeap.Insert(&theTask->fTimerHeapElem);
                (void)atomic_or(&theTask->fEvents, Task::kIdleEvent);
                doneProcessingEvent = true;
            }
        
        
			#if TASK_DEBUG
			SInt64  yieldStart = OS::Milliseconds();
			#endif
        
			this->ThreadYield();
			#if TASK_DEBUG
			SInt64  yieldDur = OS::Milliseconds() - yieldStart;
			static SInt64   numZeroYields;
        
			if ( yieldDur > 1 )
			{
				if (TASK_DEBUG) qtss_printf( "TaskThread::Entry time in Yield %i, numZeroYields %i\n", (long)yieldDur, (long)numZeroYields );
				numZeroYields = 0;
			}
			else
				numZeroYields++;
			#endif
        
        }//while (!doneProcessingEvent)
    }//外面的大循环while (true)
}


//注意，如果Task的Run()函数返回值TimeOut为正数，意味着该任务是一个周期性的工作，例如发送数据的视频泵（pump），
//需要每隔一定时间就发出一定量的视频数据，直至整个节目结束。为此，在第38~43行，将该任务加入到堆fHeap中去，
//并且标记该任务下次运行的时间为TimeOut毫秒之后。将来通过调用WaitForTask()函数就能检测到该任务是否到达规定的运行时间，WaitForTask()函数的代码如下
Task* TaskThread::WaitForTask()
{
    while (true)//一直运行到结束
    {
		//_WZD_ ExitThread _Start
//         if (OSThread::GetCurrent()->IsStopRequested())
// 		{
//             return NULL;
// 		}
		//_WZD_ ExitThread _End

			SInt64 theCurrentTime = OS::Milliseconds();
		   //如果堆中有任务，且任务已经到执行时间，返回该任务。 PeekMin函数见OSHeap.h，窃听堆中第一个元素（但不取出）
			if ((fHeap.PeekMin() != NULL) && (fHeap.PeekMin()->GetValue() <= theCurrentTime))
			{  
				if (TASK_DEBUG) qtss_printf("TaskThread::WaitForTask found timer-task=%s thread %lu fHeap.CurrentHeapSize(%lu) taskElem = %lu enclose=%lu\n",((Task*)fHeap.PeekMin()->GetEnclosingObject())->fTaskName, (UInt32) this, fHeap.CurrentHeapSize(), (UInt32) fHeap.PeekMin(), (UInt32) fHeap.PeekMin()->GetEnclosingObject());
				return (Task*)fHeap.ExtractMin()->GetEnclosingObject();//从堆中取出第一个任务返回
			}
		   //如果堆中有任务，但是尚未到执行时间，计算需要等待的时间,  if there is an element waiting for a timeout, figure out how long we should wait.
			SInt64 theTimeout = 0;
			if (fHeap.PeekMin() != NULL)
				theTimeout = fHeap.PeekMin()->GetValue() - theCurrentTime;
			Assert(theTimeout >= 0);
        
			//
			// Make sure we can't go to sleep for some ridiculously short period of time.  
			// Do not allow a timeout below 10 ms without first verifying reliable udp 1-2mbit live streams. 
			// Test with streamingserver.xml pref reliablUDP printfs enabled and look for packet loss and check client for  buffer ahead recovery.
			if (theTimeout < 10) theTimeout = 10;
            
			//wait... 等待theTimeout时间后从堆中取出任务返回
			OSQueueElem* theElem = fTaskQueue.DeQueueBlocking(this, (SInt32) theTimeout);
			if (theElem != NULL)
			{    
				if (TASK_DEBUG) qtss_printf("TaskThread::WaitForTask found signal-task=%s thread %lu fTaskQueue.GetLength(%lu) taskElem = %lu enclose=%lu\n", ((Task*)theElem->GetEnclosingObject())->fTaskName,  (UInt32) this, fTaskQueue.GetQueue()->GetLength(), (UInt32)  theElem,  (UInt32)theElem->GetEnclosingObject() );
				return (Task*)theElem->GetEnclosingObject();
			}

        //
        // If we are supposed to stop, return NULL, which signals the caller to stop
        if (OSThread::GetCurrent()->IsStopRequested())
		{
            return NULL;
		}
    }   
}

TaskThread** TaskThreadPool::sTaskThreadArray = NULL;
UInt32       TaskThreadPool::sNumTaskThreads = 0;

Bool16 TaskThreadPool::AddThreads(UInt32 numToAdd)
{
    Assert(sTaskThreadArray == NULL);
    sTaskThreadArray = new TaskThread*[numToAdd];
        
    for (UInt32 x = 0; x < numToAdd; x++)
    {
        sTaskThreadArray[x] = NEW TaskThread();
        sTaskThreadArray[x]->Start();
    }
    sNumTaskThreads = numToAdd;
    return true;
}

//_WZD_ADD_Start
void TaskThreadPool::WZD_SignalThreadsExit()
{
    //Tell all the threads to stop
    for (UInt32 x = 0; x < sNumTaskThreads; x++)
        sTaskThreadArray[x]->SendStopRequest();
	
    //Because any (or all) threads may be blocked on the queue, cycle through
    //all the threads, signalling each one
    for (UInt32 y = 0; y < sNumTaskThreads; y++)
        sTaskThreadArray[y]->fTaskQueue.GetCond()->Signal();
	
}
//_WZD_ADD_End

void TaskThreadPool::RemoveThreads()
{
    //Tell all the threads to stop
    for (UInt32 x = 0; x < sNumTaskThreads; x++)
        sTaskThreadArray[x]->SendStopRequest();

    //Because any (or all) threads may be blocked on the queue, cycle through
    //all the threads, signalling each one
    for (UInt32 y = 0; y < sNumTaskThreads; y++)
        sTaskThreadArray[y]->fTaskQueue.GetCond()->Signal();
    
    //Ok, now wait for the selected threads to terminate, deleting them and removing
    //them from the queue.
    for (UInt32 z = 0; z < sNumTaskThreads; z++)
        delete sTaskThreadArray[z];
    
	//_WZD_ADD_Start
	delete []sTaskThreadArray; 
	sTaskThreadArray = NULL;
	//_WZD_ADD_End

    sNumTaskThreads = 0;
}


//_WZD_ADD_Start
void Task::printEvent(EventFlags inFlags)
{
	char ioBuffer[128];

	switch (inFlags)
	{
	case kKillEvent:
		sprintf(ioBuffer,"%s" , "kKillEvent");
		break;

	case kIdleEvent:
		sprintf(ioBuffer,"%s" , "kIdleEvent");
		break;

	case kStartEvent:
		sprintf(ioBuffer,"%s" , "kStartEvent");
		break;

	case kTimeoutEvent:
		sprintf(ioBuffer,"%s" , "kTimeoutEvent");
		break;

	case kReadEvent:
		sprintf(ioBuffer,"%s" , "kReadEvent");
		break;

	case kWriteEvent:
		sprintf(ioBuffer,"%s" , "kWriteEvent");
		break;

	case kUpdateEvent:
		sprintf(ioBuffer,"%s" , "kUpdateEvent");
		break;

	case kTeardownEvent:
		sprintf(ioBuffer,"%s" , "kTearDownEvent");
		break;

	default:
		sprintf(ioBuffer,"Des = %d" , inFlags);
		break;

	}
	qtss_printf("++Task::Signal( %s ),  fevents = 0x%lx	this =0x%lx		Name = %s\n", ioBuffer, fEvents, this, fTaskName);
}
//_WZD_ADD_End
