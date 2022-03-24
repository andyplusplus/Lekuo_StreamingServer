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
    File:       EventContext.cpp

    Contains:   Impelments object in .h file
作为一个典型的网络服务器，DSS源代码中的Socket编程部分是其精华之一。DSS定义了一系列Socket类用于屏蔽不同平台在TCP/UDP编程接口和使用方法
上的差异。DSS中的Socket类一般都采用异步模式的（即非阻塞的），而且能够向对应的Task对象传信（Signal），这点我们在上一节介绍过。

 Socket类中具有代表性的类是：EventContext、EventThread、Socket、UDPSocket、TCPSocket
 以及TCPListenerSocket等等，它们之间的继承关系见图5。

在eventcontext.h/.cpp文件中，定义了两个类：EventContext类和EventThread类。 
 Event Context提供了检测Unix式的文件描述符（Socket就是一种文件描述符）产生的事件
 （通常是EV_RE 或 EV_WR）的能力，同时还可以传信指定的任务。EventThread类是OSThread类的子类，
 它本身很简单，只是重载了OSThread的纯虚函数Entry()，用以监控所有的Socket端口是否有数据到来，
 其代码分析如下：事件线程只有一个成员变量
*/

#include "EventContext.h"
#include "OSThread.h"
#include "atomic.h"

#include <fcntl.h>
#include <errno.h>

#ifndef __Win32__
#include <unistd.h>
#endif

#if MACOSXEVENTQUEUE
#include "tempcalls.h" //includes MacOS X prototypes of event queue functions
#endif

#define EVENT_CONTEXT_DEBUG 0

#if EVENT_CONTEXT_DEBUG
#include "OS.h"
#endif

#ifdef __Win32__
unsigned int EventContext::sUniqueID = WM_USER; // See commentary in RequestEvent
#else
unsigned int EventContext::sUniqueID = 1;
#endif

EventContext::EventContext(int inFileDesc, EventThread* inThread)
:   fFileDesc(inFileDesc),
    fUniqueID(0),
    fUniqueIDStr((char*)&fUniqueID, sizeof(fUniqueID)),
    fEventThread(inThread),
    fWatchEventCalled(false),
    fAutoCleanup(true)
{
	fUniqueID = fUniqueID; //test
}


void EventContext::InitNonBlocking(int inFileDesc)
{

    fFileDesc = inFileDesc;
    
#ifdef __Win32__
    u_long one = 1;
    int err = ::ioctlsocket(fFileDesc, FIONBIO, &one);				//设置为非阻塞模式
		// The ioctlsocket function controls the I/O mode of a socket，
		//  int ioctlsocket(
		//    SOCKET s,         //Descriptor identifying a socket
		//    long cmd,          //Command to perform on the socket s
		//    u_long FAR *argp   //Pointer to a parameter for cmd
		//  );
#else
    int flag = ::fcntl(fFileDesc, F_GETFL, 0);
    int err = ::fcntl(fFileDesc, F_SETFL, flag | O_NONBLOCK);
#endif
    AssertV(err == 0, OSThread::GetErrno());
}

//从fEventThread->fRefTable的引用表中删除EventContext，关闭fFileDesc Socket，
//并且把fFileDesc设置为kInvalidFileDesc，fUniqueID为0
void EventContext::Cleanup()
{
    int err = 0;
    
    if (fFileDesc != kInvalidFileDesc)
    {
        //if this object is registered in the table, unregister it now
        if (fUniqueID > 0)
        {
            fEventThread->fRefTable.UnRegister(&fRef);

#if !MACOSXEVENTQUEUE
            select_removeevent(fFileDesc);//The eventqueue / select shim requires this
	#ifdef __Win32__
				qtss_printf("TobeTemp  closesocket(fFileDesc) = %d \n", fFileDesc);
				err = ::closesocket(fFileDesc);

	#endif

#else
            //On Linux (possibly other UNIX implementations) you MUST NOT close the fd before removing the fd from the select mask, and having the select function wake up to register this fact. If you close the fd first, bad things may happen, like the socket not getting unbound from the port & IP addr.
            //So, what we do is have the select thread itself call close. This is triggered by calling removeevent.
            err = ::close(fFileDesc);
#endif      
        }
        else
#ifdef __Win32__
			qtss_printf("TobeTemp  closesocket(fFileDesc) = %d \n", fFileDesc);
			err = ::closesocket(fFileDesc);
 			if(err =! 0)	//_WZD_ADD_  2008.06.21
			{
				int theError = WSAGetLastError();
				theError = theError;
			}
#else
            err = ::close(fFileDesc);
#endif
    }

    fFileDesc = kInvalidFileDesc;
    fUniqueID = 0;

    AssertV(err == 0, OSThread::GetErrno());//we don't really care if there was an error, but it's nice to know
}


//并时把当前EventContext替换fromContext，包括
//		1. 把fromContext设置为kInvalidFileDesc，
//		2. 并将fromContext其从fEventThread->fRefTable中删除，

void EventContext::SnarfEventContext( EventContext &fromContext )
{   // 当实现基于TCPSocket的HTTP Proxy tunnels, 我们需要维护该context
    //+ show that we called watchevent
    //		copy the unique id
    //		set our fUniqueIDStr to the unique id
    //		copy the eventreq
    //		find the old event object
    //		show us as the object in the fRefTable
    //			we take the OSRef from the old context, point it at our context
    //
    //TODO - this whole operation causes a race condition for Event posting
    //     way up the chain we need to disable event posting or copy the posted events afer this op completes
    
    fromContext.fFileDesc = kInvalidFileDesc;
    
    fWatchEventCalled = fromContext.fWatchEventCalled; 
    fUniqueID = fromContext.fUniqueID;
    fUniqueIDStr.Set((char*)&fUniqueID, sizeof(fUniqueID)),
    
    ::memcpy( &fEventReq, &fromContext.fEventReq, sizeof( struct eventreq  ) );

    fRef.Set( fUniqueIDStr, this );
    fEventThread->fRefTable.Swap(&fRef);						
    fEventThread->fRefTable.UnRegister(&fromContext.fRef);		//从哈希表中删除原对象
}


//该函数把EventContext添加到EventThread的引用表中
void EventContext::RequestEvent(int theMask)
{
#if DEBUG
    fModwatched = true;
#endif

    // The first time this function gets called, we're supposed to call watchevent. Each subsequent time, call modwatch. That's the way the MacOS X event queue works.
    // 第一次调用该函数时，调用watchevent，随后每次调用该函数，调用modwatch    
    if (fWatchEventCalled)
    {
			fEventReq.er_eventbits = theMask;
		#if MACOSXEVENTQUEUE
			if (modwatch(&fEventReq, theMask) != 0)		
		#else
			if (select_modwatch(&fEventReq, theMask) != 0)		//windows
		#endif  
			AssertV(false, OSThread::GetErrno());
    }
    else
    {
        //allocate a Unique ID for this socket, and add it to the ref table
			#ifdef __Win32__
        //
        // Kind of a hack. On Win32, the way that we pass around the unique ID is by making it the message ID of our Win32 message 
		//(see win32ev.cpp). Messages must be >= WM_USER. Hence this code to restrict the numberspace of our UniqueIDs. 
        //为该socket分配唯一的ID，并添加到ref table，类似于hack. 在Win32, 我们传递unique ID通过把其作为Win32的message ID
        // (see win32ev.cpp). 消息必须>= WM_USER. 因此该代码限制了UniqueIDs的名字空间 

		//1. 设置fUniqueID
					if (!compare_and_store(8192, WM_USER, &sUniqueID))  // Fix 2466667: message IDs above a
						fUniqueID = (PointerSizedInt)atomic_add(&sUniqueID, 1);         // level are ignored, so wrap at 8192
					else
						fUniqueID = (PointerSizedInt)WM_USER;
			#else
					if (!compare_and_store(10000000, 1, &sUniqueID))
						fUniqueID = (PointerSizedInt)atomic_add(&sUniqueID, 1);
					else
						fUniqueID = 1;
			#endif
		//2. 把当前事件加入引用表fEventThread->fRefTable
					fRef.Set(fUniqueIDStr, this);
					fEventThread->fRefTable.Register(&fRef);
            
					//fill out the eventreq data structure
		//3. 填充eventreq数据结构，第一次调用需要填充数据结构
					::memset( &fEventReq, '\0', sizeof(fEventReq));
					fEventReq.er_type = EV_FD;
					fEventReq.er_handle = fFileDesc;		//在该对象的InitNonBlocking(osSocket)函数中已经设置为socket描述符
					fEventReq.er_eventbits = theMask;		//该事件对应的操作
					fEventReq.er_data = (void*)fUniqueID;	//标识该Context的唯一ID

					fWatchEventCalled = true;
			#if MACOSXEVENTQUEUE
					if (watchevent(&fEventReq, theMask) != 0)
			#else
					//调用以下函数建立窗口，设置侦听事件，并且socket有事件时，主动通知窗口
					if (select_watchevent(&fEventReq, theMask) != 0)
			#endif  
						//this should never fail, but if it does, cleanup.
						AssertV(false, OSThread::GetErrno());
            
    }
}



//1. 获得一个消息
//2. 从消息中抽取socket
//3. 调用对应的socket对象处理消息
//4. 处理完以后从引用表中减少对象的引用计数
void EventThread::Entry()
{
//该结构定义在ev.h中，记录Socket描述符和在该描述符上发生的事件, 初始化theCurrentEvent

    struct eventreq theCurrentEvent;
    ::memset( &theCurrentEvent, '\0', sizeof(theCurrentEvent) );
    
    while (true)
    {
        int theErrno = EINTR;

		//等待从theCurrentEvent返回事件，调用select__waitevent函数监听所有的Socket端口，直到有事件发生为止
        while (theErrno == EINTR)
        {
#if MACOSXEVENTQUEUE
            int theReturnValue = waitevent(&theCurrentEvent, NULL);
#else
            int theReturnValue = select_waitevent(&theCurrentEvent, NULL);	
			//調用select_waitevent函數監聽所有的Socket端口，直到有事件發生為止，事件通过消息发过来，由EventContext::RequestEvent(...)中的select_watchevent(...)激活消息
#endif  
            //Sort of a hack. In the POSIX version of the server, waitevent can return an actual POSIX errorcode.
            if (theReturnValue >= 0)
                theErrno = theReturnValue;
            else
                //theErrno = OSThread::GetErrno();
				//_WZD_ ExitThread _Start		WZD_Event_Thread_Received_WM_QUIT_Message
				{
					qtss_printf("EventThread::Entry() received signal exit event, _WZD_ ExitThread _Start, WZD_Event_Thread_Received_WM_QUIT_Message\n");
					return;	
				}
				//_WZD_ ExitThread _End

        }
        
        AssertV(theErrno == 0, theErrno);
        
        //ok, there's data waiting on this socket. Send a wakeup.
        //socket上有数据等待，发送wakeup，唤醒相应的Socket端口
        if (theCurrentEvent.er_data != NULL)		//即获得消息的消息体theMessage.message,也就是socketID
        {
            //The cookie in this event is an ObjectID. Resolve that objectID into a pointer.
            //该事件的cookie是一个对象名字，该对象已经在引用表中，解析为指针, 通过事件中的标识找到相应的对象参考指针

            StrPtrLen idStr((char*)&theCurrentEvent.er_data, sizeof(theCurrentEvent.er_data));
            OSRef* ref = fRefTable.Resolve(&idStr);
            if (ref != NULL)
            {
                EventContext* theContext = (EventContext*)ref->GetObject();
#if DEBUG
                theContext->fModwatched = false;
#endif
                theContext->ProcessEvent(theCurrentEvent.er_eventbits);
					//处理事件在默认时只生成了读事件的命令，即fTask->Signal(Task::kReadEvent);
                fRefTable.Release(ref);		//减少引用,但并不删除任务, 与此对应的是,获得一个事件时,增加引用计数
            }
        }

#if EVENT_CONTEXT_DEBUG
        SInt64  yieldStart = OS::Milliseconds();
#endif

        this->ThreadYield();

#if EVENT_CONTEXT_DEBUG
        SInt64  yieldDur = OS::Milliseconds() - yieldStart;
        static SInt64   numZeroYields;
        
        if ( yieldDur > 1 )
        {
            qtss_printf( "EventThread time in OSTHread::Yield %i, numZeroYields %i\n", (long)yieldDur, (long)numZeroYields );
            numZeroYields = 0;
        }
        else
            numZeroYields++;
#endif
    }
	return;
}
