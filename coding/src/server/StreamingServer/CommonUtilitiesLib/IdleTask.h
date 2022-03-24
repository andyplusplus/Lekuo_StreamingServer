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
    File:       IdleTask.h

    Contains:   IdleTasks are identical to normal tasks (see task.h) with one exception:
    
                You can schedule them for timeouts. If you call SetIdleTimer
                on one, after the time has elapsed the task object will receive an
                OS_IDLE event. 

IdleTasks与常规任务相同, 但有一点不同的是: 你可以调度IdleTask来做超时操作. 如果你调用SetIdleTimer函数, 
时间过去以后, 任务对象会接收到OS_IDLE事件.

IdleTask.h定义了IdleTaskThread类以及IdleTask类。在QTSS中，IdleTaskThread类，TaskThread类以及EventThread类都是从
OSThread类继承下来的。这些类维护了OSHeap，OSQueue等数据结构，需要执行的任务放在该数据结构中，线程类在Entry函数中
取出一个任务，并处理该任务，任务的处理通常是调用Signal函数，把任务放入任务队列中，服务器会从任务队列取出任务，并执行该任务。

*/


#ifndef _IDLETASK_H_
#define _IDLETASK_H_

#include "Task.h"

#include "OSThread.h"
#include "OSHeap.h"
#include "OSMutex.h"
#include "OSCond.h"

class IdleTask;

//merely a private implementation detail of IdleTask
class IdleTaskThread : private OSThread
{

	//_WZD_ADD_Start
	void SignalHeapCond() {		//In case the thread block at HeapCond
		fHeapCond.Signal();
		fHeapCond.Signal();
		fHeapCond.Signal();
	}
	//_WZD_ADD_End

private:

    IdleTaskThread() : OSThread(), fHeapMutex() {}
    virtual ~IdleTaskThread() { 
		Assert(fIdleHeap.CurrentHeapSize() == 0); 
	}

    void SetIdleTimer(IdleTask *idleObj, SInt64 msec);	//把任务插入堆中，msec为关键字
    void CancelTimeout(IdleTask *idleObj);				//删除任务
    
    virtual void Entry();	//该函数是最主要的函数
    OSHeap  fIdleHeap;		//维护的一个任务堆		
    OSMutex fHeapMutex;
    OSCond  fHeapCond;
    friend class IdleTask;
};


class IdleTask : public Task
{

public:

    //Call Initialize before using this class
	//使用该类前调用该函数
    static void Initialize();
	//_WZD_ADD_Start
	static void WZD_SignalIdleThreadExit();
    static void UnInitialize();
	//_WZD_ADD_End
    
    IdleTask() : Task(), fIdleElem() { this->SetTaskName("IdleTask"); fIdleElem.SetEnclosingObject(this); }
    
    //This object does a "best effort" of making sure a timeout isn't
    //pending for an object being deleted. In other words, if there is
    //a timeout pending, and the destructor is called, things will get cleaned
    //up. But callers must ensure that SetIdleTimer isn't called at the same
    //time as the destructor, or all hell will break loose.
    //该对象采取"best effort"，确保删除的对象没有未处理的timeout。如果有未处理的timeout，
	//调用析构函数，将释放资源，如调用者必须确保调用destructor时没有同时调用SetIdleTimer, 
    // 否则系统会混乱
    virtual ~IdleTask();
    
    //SetIdleTimer:
    //This object will receive an OS_IDLE event in the following number of milliseconds.
    //Only one timeout can be outstanding, if there is already a timeout scheduled, this does nothing.
    //该对象在msec 毫秒后会收到OS_IDLE事件，同时只有一个timeout站出来，
    //如果已经有一个timeout被调度，该time不做任何工作。
    void SetIdleTimer(SInt64 msec) { sIdleThread->SetIdleTimer(this, msec); }

    //CancelTimeout
    //If there is a pending timeout for this object, this function cancels it.
    //If there is no pending timeout, this function does nothing.
    //Currently not supported because OSHeap doesn't support random remove
	//从Heap中移去Idle任务对象，IdleTask的析构函数调用以下函数，从而移去未处理的对象
    //如果该对象有一个未处理的timeout，该函数取消该对象，如没有未处理timeout，则不做任何处理。
    void CancelTimeout() { sIdleThread->CancelTimeout(this); }

private:

    OSHeapElem fIdleElem;		//该任务封装的堆元素

    //there is only one idle thread shared by all idle tasks.
    static IdleTaskThread*  sIdleThread;    

    friend class IdleTaskThread;
};
#endif
