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
    File:       Task.h

    Contains:   Tasks are objects that can be scheduled. To schedule a task, you call its
                signal method, and pass in an event (events are bits and all events are defined
                below).
                
                Once Signal() is called, the task object will be scheduled. When it runs, its
                Run() function will get called. In order to clear the event, the derived task
                object must call GetEvents() (which returns the events that were sent).
                
                Calling GetEvents() implicitly "clears" the events returned. All events must
                be cleared before the Run() function returns, or Run() will be invoked again
                immediately.
因为服务器从整体上采用了异步的运行模式，这就需要一种用于事件通信的机制。举例来说：一个RTSP连接对应的Socket端口监测到网络上有数据到达，此时必须有一个模块（或代码）被通知（notify）去处理这些数据。为此，DSS定义了Task及其相关类作为实现这一通信机制的核心。

    在Task.h/cpp文件中，定义了三个主要的类，分别是：任务线程池类（TaskThreadPool Class）、任务线程类（TaskThread Class）以及任务类（Task Class）。

    每个Task对象有两个主要的方法：Signal和Run。当服务器希望发送一个事件给某个Task对象时，就会调用Signal()方法；而Run()方法是在Task对象获得处理该事件的时间片后运行的，服务器中的大部分工作都是在不同Task对象的Run()函数中进行的。每个Task对象的目标就是利用很小的且不会阻塞的时间片完成服务器指定某个工作。

任务线程类是上文介绍的OSThread类的一个子类，代表专门用于运行任务类的一个线程。在每个任务线程对象内部都有一个OSQueue_Blocking类型的任务队列，存储该线程需要执行的任务。后面的分析可以看到，服务器调用一个任务的Signal函数，实际上就是将该任务加入到某个任务线程类的任务队列中去。另外，为了统一管理这些任务线程，DSS还开发了任务线程池类，该类负责生成、删除以及维护内部的任务线程列表。图4描述了任务类的运行。

*/

#ifndef __TASK_H__
#define __TASK_H__

#include "OSQueue.h"
#include "OSHeap.h"
#include "OSThread.h"
#include "OSMutexRW.h"
#include "myPrintf.h"   //Added by WuZengDe, 2007.11.21 把输出结果打印到Windows Debug窗口
#ifdef 	CLIENT_CommonUtilitiesLib
	#define TASK_DEBUG 0
#else
	#define TASK_DEBUG 0
#endif

class  TaskThread;

class Task
{
    public:
        
        typedef unsigned int EventFlags;

        //EVENTS
        //here are all the events that can be sent to a task
        enum
        {
            kKillEvent =    0x1 << 0x0, //these are all of type "EventFlags"
            kIdleEvent =    0x1 << 0x1,
            kStartEvent =   0x1 << 0x2,
            kTimeoutEvent = 0x1 << 0x3,
       
          //socket events
            kReadEvent =        0x1 << 0x4, //All of type "EventFlags"
            kWriteEvent =       0x1 << 0x5,
           
           //update event
            kUpdateEvent =      0x1 << 0x6, 
			kTeardownEvent =    0x1 << 0x8	//WZD_ADD
        };
        
        //CONSTRUCTOR / DESTRUCTOR
        //You must assign priority at create time.
                                Task();
        virtual                 ~Task() {}

        //return:
        // >0-> invoke me after this number of MilSecs with a kIdleEvent
        // 0 don't reinvoke me at all.
        //-1 delete me
        //Suggested practice is that any task should be deleted by returning true from the
        //Run function. That way, we know that the Task is not running at the time it is
        //deleted. This object provides no protection against calling a method, such as Signal,
        //at the same time the object is being deleted (because it can't really), so watch
        //those dangling references!
        virtual SInt64          Run() = 0;
        
        //Send an event to this task.
        void                    Signal(EventFlags eventFlags);
        void                    GlobalUnlock();     
        Bool16                  Valid(); // for debugging
		char            fTaskName[48];
		void            SetTaskName(char* name);
        
    protected:
    
        //Only the tasks themselves may find out what events they have received
        EventFlags              GetEvents();
        
        // ForceSameThread
        //
        // A task, inside its run function, may want to ensure that the same task thread
        // is used for subsequent calls to Run(). This may be the case if the task is holding
        // a mutex between calls to run. By calling this function, the task ensures that the
        // same task thread will be used for the next call to Run(). It only applies to the
        // next call to run.
        void                    ForceSameThread()   {
                                                        fUseThisThread = (TaskThread*)OSThread::GetCurrent();
                                                        Assert(fUseThisThread != NULL);
															#if TASK_DEBUG
                                                        if (fTaskName[0] == 0) ::strcpy(fTaskName, " corrupt task");
                                                        qtss_printf("Task::ForceSameThread fUseThisThread %lu task %s enque elem=%lu enclosing %lu\n", (UInt32)fUseThisThread, fTaskName,(UInt32) &fTaskQueueElem,(UInt32) this);
															#endif
                                                    }
        SInt64                  CallLocked()        {   ForceSameThread();
                                                        fWriteLock = true;
                                                        return (SInt64) 10; // minimum of 10 milliseconds between locks
                                                    }

    private:

		//_WZD_ADD_Start
		void printEvent(EventFlags inFlags);
		//_WZD_ADD_End

        enum
        {
            kAlive =            0x80000000, //EventFlags, again
            kAliveOff =         0x7fffffff
        };

        void            SetTaskThread(TaskThread *thread);
        
        EventFlags      fEvents;
        TaskThread*     fUseThisThread;
        Bool16          fWriteLock;

#if DEBUG
        //The whole premise of a task is that the Run function cannot be re-entered.
        //This debugging variable ensures that that is always the case
        volatile UInt32 fInRunCount;
#endif

        //This could later be optimized by using a timing wheel instead of a heap,
        //and that way we wouldn't need both a heap elem and a queue elem here (just queue elem)
        OSHeapElem      fTimerHeapElem;
        OSQueueElem     fTaskQueueElem;
        
        //Variable used for assigning tasks to threads in a round-robin fashion
        static unsigned int sThreadPicker;
        
        friend class    TaskThread; 
};

//提示：所有的Task对象都将在TaskThread中运行
class TaskThread : public OSThread
{
    public:
    
        //Implementation detail: all tasks get run on TaskThreads.
        
                        TaskThread() :  OSThread(), fTaskThreadPoolElem()
                                        {fTaskThreadPoolElem.SetEnclosingObject(this);}

						virtual         ~TaskThread() 
						{ 
							this->StopAndWaitForThread(); 
							//   _WZD_MEM_Start    2008.03.13  used to clear the memory in queue and Heap
							Task* theTask = NULL;
							OSHeapElem* theHeapElem = NULL;
							while (	theHeapElem = fHeap.ExtractMin())
							{
								theTask = (Task*)theHeapElem->GetEnclosingObject();
								if(theTask)
								{
									theTask->fTaskQueueElem.Remove();
									delete theTask;
								}
							}

							OSQueueElem* theQueueElem;
							while (	theQueueElem = fTaskQueue.DeQueue() )
							{
								theTask = (Task*) theQueueElem->GetEnclosingObject();
								delete theTask;
							}
							//   _WZD_MEM_End
						}
           
    private:
    
        enum
        {
            kMinWaitTimeInMilSecs = 10  //UInt32
        };

        virtual void    Entry();				//从OSThread重载的执行函数，仍然能够被子类重载
        Task*           WaitForTask();			//检测是否有该执行的任务
        
        OSQueueElem     fTaskThreadPoolElem;	//成为线程池的一部分,//对应的线程池对象
        
        OSHeap              fHeap;				//按时间片及其任务构成的堆//纪录任务运行时间的堆，Entry()函数设置继续等待的时间，用于WaitForTask函数确定继续等待的时间
        OSQueue_Blocking    fTaskQueue;			/*关键数据结构：任务队列；在Task的Signal函数中直接调用fTaskQueue对象的EnQueue函数将自己加入任务队列，在WaitForTask()函数中从fTaskQueue弹出任务，*/
        
        
        friend class Task;
        friend class TaskThreadPool;
};

//Because task threads share a global queue of tasks to execute,
//there can only be one pool of task threads. That is why this object
//is static.
class TaskThreadPool {
public:

    //Adds some threads to the pool
    static Bool16   AddThreads(UInt32 numToAdd);
    static void     SwitchPersonality( char *user = NULL, char *group = NULL);

	//_WZD_ADD_Start
    static void     WZD_SignalThreadsExit();	//在调用RemoveThreads()之前最好先调用该函数，然后Sleep(0)
	//_WZD_ADD_End

    static void     RemoveThreads();

    
private:

    static TaskThread**     sTaskThreadArray;
    static UInt32           sNumTaskThreads;
    static OSMutexRW        sMutexRW;
    
    friend class Task;
    friend class TaskThread;
};

#endif
