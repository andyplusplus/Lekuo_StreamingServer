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

 */
/*
    File:       TimeoutTask.h

    Contains:   Just like a normal task, but can be scheduled for timeouts. Unlike
                IdleTask, which is VERY aggressive about being on time, but high
                overhead for maintaining the timing information, this is a low overhead,
                low priority timing mechanism. Timeouts may not happen exactly when
                they are supposed to, but who cares?
                    
    
    
    
*/

#ifndef __TIMEOUTTASK_H__
#define __TIMEOUTTASK_H__


#include "StrPtrLen.h"
#include "IdleTask.h"

#include "OSThread.h"
#include "OSQueue.h"
#include "OSMutex.h"
#include "OS.h"

#define TIMEOUT_DEBUGGING 0 //messages to help debugging timeouts

//TimeoutTaskThread, 实际上并不是一个线程, 而是一个特别的任务对象
class TimeoutTaskThread : public IdleTask
{
    public:
    
        //All timeout tasks get timed out from this thread
                    TimeoutTaskThread() : IdleTask(), fMutex() {this->SetTaskName("TimeoutTask");}
        
		virtual     ~TimeoutTaskThread()
		{
			//_WZD_ ExitThread _Start
			int test = 0;
			test;
			//_WZD_ ExitThread _End
		}

    private:
        
        //this thread runs every minute and checks for timeouts
        enum
        {
            kIntervalSeconds = 60   //该线程每个钟运行一次，并检查timeouts //UInt32
        };

        virtual SInt64          Run();
        OSMutex                 fMutex;
        OSQueue                 fQueue;
        
        friend class TimeoutTask;
};

class TimeoutTask
{
    //TimeoutTask is not a derived object off of Task, to add flexibility as to how this object can be utilitized
	//TimeoutTask不是从Task继存而来，增加该类以使用的灵活性
    
    public:
    
        //Call Initialize before using this class
		// 使用该类以前调用初始化, 新建TimeoutTaskThread ，并sThread->Signal(Task::kStartEvent);
		// 把TimeoutTaskThread运行起来，以便能定期运动Run函数
        static void Initialize();

		//_WZD_ADD_Start
		static void UnInitialize();
		//_WZD_ADD_End


        //Pass in the task you'd like to send timeouts to. 
        //Also pass in the timeout you'd like to use. By default, the timeout is 0 (NEVER).
		//传入timeout发向的任务，同时传入timeout值，默认为0 (NEVER).
        TimeoutTask(Task* inTask, SInt64 inTimeoutInMilSecs = 60);//构造任务项，加入线程的任务队列
        ~TimeoutTask();
        
        //MODIFIERS

        // Changes the timeout time, also refreshes the timeout
		// 改变timeout值，并刷新timeout
        void        SetTimeout(SInt64 inTimeoutInMilSecs);
        
        // Specified task will get a Task::kTimeoutEvent if this
        // function isn't called within the timeout period
		// 如果该函数在timeout期间没有调用，指定任务将收到Task::kTimeoutEvent 
        void        RefreshTimeout() { fTimeoutAtThisTime = OS::Milliseconds() + fTimeoutInMilSecs; Assert(fTimeoutAtThisTime > 0); }
        
        void        SetTask(Task* inTask) { 
			fTask = inTask; 
		}
    private:
    
        Task*       fTask;				//该超时任务对应的任务对象
        SInt64      fTimeoutAtThisTime;	//绝对超时时间
        SInt64      fTimeoutInMilSecs;	//相对超时时间
        //for putting on our global queue of timeout tasks
        OSQueueElem fQueueElem;			//以下表项用于放到timeout任务的全局队列中
        
        static TimeoutTaskThread*   sThread;	//对应的TimeoutTaskThread
        
        friend class TimeoutTaskThread;
};
#endif //__TIMEOUTTASK_H__

