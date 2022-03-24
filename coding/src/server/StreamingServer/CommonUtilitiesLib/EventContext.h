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
    File:       EventContext.h

    Contains:   An event context provides the intelligence to take an event
                generated from a UNIX file descriptor (usually EV_RE or EV_WR)
                and signal a Task. 
Event context提供了一种智能性，从UNIX file descriptor (通常是EV_RE or EV_WR)获取事件，并传信一个任务。
    
*/

#ifndef __EVENT_CONTEXT_H__
#define __EVENT_CONTEXT_H__

#include "OSThread.h"
#include "Task.h"
#include "OSRef.h"
#include "ev.h"

//enable to trace event context execution and the task associated with the context
#define EVENTCONTEXT_DEBUG 1

class EventThread;

class EventContext
{
    public:
    
        //
        // Constructor. Pass in the EventThread you would like to receive events for this context, and the fd that this context applies to
    	//构造函数: 传入你想要该context接收的EventThread，以及该context应用的fd
        EventContext(int inFileDesc, EventThread* inThread);
        virtual ~EventContext() { 
			if (fAutoCleanup) this->Cleanup(); 
		}
        
        //
        // InitNonBlocking
        //
        // Sets inFileDesc to be non-blocking. Once this is called, the EventContext object "owns" the file descriptor, and will close it
        // when Cleanup is called. This is necessary because of some weird select() behavior. DON'T CALL CLOSE ON THE FD ONCE THIS IS CALLED!!!!
        // 设置inFileDesc为非阻塞，一旦该函数被调用，EventContext对象拥有file descriptor, 并且当Cleanup调用以后关闭，这是由于select() 的奇怪行为
        void            InitNonBlocking(int inFileDesc);

        //
        // Cleanup. Will be called by the destructor, but can be called earlier
        //析构函数调用，也可在此前调用
		//从fEventThread->fRefTable的引用表中删除EventContext，关闭fFileDesc Socket，
		//并且把fFileDesc设置为kInvalidFileDesc，fUniqueID为0
        void            Cleanup();

        //
        // Arms this EventContext. Pass in the events you would like to receive
        //传递你想接收的事件类型
        void            RequestEvent(int theMask = EV_RE);

        
        //
        // Provide the task you would like to be notified
        // 设置希望被通知的任务
        void            SetTask(Task* inTask)
        {  
            fTask = inTask; 
            #if EVENTCONTEXT_DEBUG
            {
                if (fTask== NULL)  
                    qtss_printf("EventContext::SetTask context=%lu task= NULL\n", (UInt32) this); 
                else 
                    qtss_printf("EventContext::SetTask context=%lu task= %lu name=%s\n",(UInt32) this,(UInt32) fTask, fTask->fTaskName); 
            }
			#endif
        }
        
        // when the HTTP Proxy tunnels takes over a TCPSocket, we need to maintain this context too
        // 当实现基于TCPSocket的HTTP Proxy tunnels, 我们需要维护该context
        void            SnarfEventContext( EventContext &fromContext );
        
        // Don't cleanup this socket automatically
        void            DontAutoCleanup() { fAutoCleanup = false; }
        
        // Direct access to the FD is not recommended, but is needed for modules
        // that want to use the Socket classes and need to request events on the fd.
        int             GetSocketFD()       { return fFileDesc; }
        
        enum
        {
            kInvalidFileDesc = -1   //int
        };

    protected:

        //
        // ProcessEvent
        //
        // When an event occurs on this file descriptor, this function
        // will get called. Default behavior is to Signal the associated
        // task, but that behavior may be altered / overridden.
        //
        // Currently, we always generate a Task::kReadEvent
		//
        // 当file descriptor发生一个事件时，函数被调用，默认的操作是传信相关任务。但函数可被重载
        // 当前总量生成一个Task::kReadEvent

		virtual void ProcessEvent(int /*eventBits*/) 
        {   
			#if EVENTCONTEXT_DEBUG
            {
                if (fTask== NULL)  
                    qtss_printf("EventContext::ProcessEvent context=%lu task=NULL\n",(UInt32) this); 
                else 
                    qtss_printf("EventContext::ProcessEvent context=%lu task=%lu TaskName=%s\n",(UInt32)this,(UInt32) fTask, fTask->fTaskName); 
            }
			#endif

            if (fTask != NULL)
                fTask->Signal(Task::kReadEvent); 
        }

        int             fFileDesc;				//事件线程对应的Socket描述符

    private:

        struct eventreq fEventReq;				//事件请求结构 

        //
		//  在fUniqueIDStr:this唯一标识该对象
        OSRef           fRef;					//该Context的引用，并添加到fEventThread->fRefTable
        PointerSizedInt fUniqueID;				//标识该Context的唯一ID,该变量的值通过EventContext::RequestEvent(int theMask)获得,大小为WM_User+1
        StrPtrLen       fUniqueIDStr;			//标识该Context的字符串ID,该变量和fUniqueID的关系非常微妙,变量的Ptr只是取fUniqueID的地址,并强行转换,所以其值问题和fUniqueID一样

		//
        EventThread*    fEventThread;			//Context对应的事件线程
        Bool16          fWatchEventCalled;		//状态变量,初始为False,当为False时,需则需执行申请fUniqueID, 增加引用,并调用select_watchevent(...); 如为TRUE,则直接调用select__modwatch(...),而不申请fUniqueID
        int             fEventBits;				//没有使用的成员变量
        Bool16          fAutoCleanup;			//是否随着对象销毁，执行Cleanup()

        Task*           fTask;					//希望被传信的任务
#if DEBUG
        Bool16          fModwatched;
#endif
        
        static unsigned int sUniqueID;			//对Windows系统，该值为WM_USER
        
        friend class EventThread;
};

class EventThread : public OSThread
{
    public:
    
        EventThread() : OSThread() {}
        virtual ~EventThread() {}
    
    private:
    
        virtual void Entry();
        OSRefTable      fRefTable;
        
        friend class EventContext;
};

#endif //__EVENT_CONTEXT_H__
