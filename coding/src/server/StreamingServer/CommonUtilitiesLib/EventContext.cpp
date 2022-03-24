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
��Ϊһ�����͵������������DSSԴ�����е�Socket��̲������侫��֮һ��DSS������һϵ��Socket���������β�ͬƽ̨��TCP/UDP��̽ӿں�ʹ�÷���
�ϵĲ��졣DSS�е�Socket��һ�㶼�����첽ģʽ�ģ����������ģ��������ܹ����Ӧ��Task�����ţ�Signal���������������һ�ڽ��ܹ���

 Socket���о��д����Ե����ǣ�EventContext��EventThread��Socket��UDPSocket��TCPSocket
 �Լ�TCPListenerSocket�ȵȣ�����֮��ļ̳й�ϵ��ͼ5��

��eventcontext.h/.cpp�ļ��У������������ࣺEventContext���EventThread�ࡣ 
 Event Context�ṩ�˼��Unixʽ���ļ���������Socket����һ���ļ����������������¼�
 ��ͨ����EV_RE �� EV_WR����������ͬʱ�����Դ���ָ��������EventThread����OSThread������࣬
 ������ܼ򵥣�ֻ��������OSThread�Ĵ��麯��Entry()�����Լ�����е�Socket�˿��Ƿ������ݵ�����
 �����������£��¼��߳�ֻ��һ����Ա����
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
    int err = ::ioctlsocket(fFileDesc, FIONBIO, &one);				//����Ϊ������ģʽ
		// The ioctlsocket function controls the I/O mode of a socket��
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

//��fEventThread->fRefTable�����ñ���ɾ��EventContext���ر�fFileDesc Socket��
//���Ұ�fFileDesc����ΪkInvalidFileDesc��fUniqueIDΪ0
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


//��ʱ�ѵ�ǰEventContext�滻fromContext������
//		1. ��fromContext����ΪkInvalidFileDesc��
//		2. ����fromContext���fEventThread->fRefTable��ɾ����

void EventContext::SnarfEventContext( EventContext &fromContext )
{   // ��ʵ�ֻ���TCPSocket��HTTP Proxy tunnels, ������Ҫά����context
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
    fEventThread->fRefTable.UnRegister(&fromContext.fRef);		//�ӹ�ϣ����ɾ��ԭ����
}


//�ú�����EventContext��ӵ�EventThread�����ñ���
void EventContext::RequestEvent(int theMask)
{
#if DEBUG
    fModwatched = true;
#endif

    // The first time this function gets called, we're supposed to call watchevent. Each subsequent time, call modwatch. That's the way the MacOS X event queue works.
    // ��һ�ε��øú���ʱ������watchevent�����ÿ�ε��øú���������modwatch    
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
        //Ϊ��socket����Ψһ��ID������ӵ�ref table��������hack. ��Win32, ���Ǵ���unique IDͨ��������ΪWin32��message ID
        // (see win32ev.cpp). ��Ϣ����>= WM_USER. ��˸ô���������UniqueIDs�����ֿռ� 

		//1. ����fUniqueID
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
		//2. �ѵ�ǰ�¼��������ñ�fEventThread->fRefTable
					fRef.Set(fUniqueIDStr, this);
					fEventThread->fRefTable.Register(&fRef);
            
					//fill out the eventreq data structure
		//3. ���eventreq���ݽṹ����һ�ε�����Ҫ������ݽṹ
					::memset( &fEventReq, '\0', sizeof(fEventReq));
					fEventReq.er_type = EV_FD;
					fEventReq.er_handle = fFileDesc;		//�ڸö����InitNonBlocking(osSocket)�������Ѿ�����Ϊsocket������
					fEventReq.er_eventbits = theMask;		//���¼���Ӧ�Ĳ���
					fEventReq.er_data = (void*)fUniqueID;	//��ʶ��Context��ΨһID

					fWatchEventCalled = true;
			#if MACOSXEVENTQUEUE
					if (watchevent(&fEventReq, theMask) != 0)
			#else
					//�������º����������ڣ����������¼�������socket���¼�ʱ������֪ͨ����
					if (select_watchevent(&fEventReq, theMask) != 0)
			#endif  
						//this should never fail, but if it does, cleanup.
						AssertV(false, OSThread::GetErrno());
            
    }
}



//1. ���һ����Ϣ
//2. ����Ϣ�г�ȡsocket
//3. ���ö�Ӧ��socket��������Ϣ
//4. �������Ժ�����ñ��м��ٶ�������ü���
void EventThread::Entry()
{
//�ýṹ������ev.h�У���¼Socket���������ڸ��������Ϸ������¼�, ��ʼ��theCurrentEvent

    struct eventreq theCurrentEvent;
    ::memset( &theCurrentEvent, '\0', sizeof(theCurrentEvent) );
    
    while (true)
    {
        int theErrno = EINTR;

		//�ȴ���theCurrentEvent�����¼�������select__waitevent�����������е�Socket�˿ڣ�ֱ�����¼�����Ϊֹ
        while (theErrno == EINTR)
        {
#if MACOSXEVENTQUEUE
            int theReturnValue = waitevent(&theCurrentEvent, NULL);
#else
            int theReturnValue = select_waitevent(&theCurrentEvent, NULL);	
			//�{��select_waitevent�����O ���е�Socket�˿ڣ�ֱ�����¼��l����ֹ���¼�ͨ����Ϣ����������EventContext::RequestEvent(...)�е�select_watchevent(...)������Ϣ
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
        //socket�������ݵȴ�������wakeup��������Ӧ��Socket�˿�
        if (theCurrentEvent.er_data != NULL)		//�������Ϣ����Ϣ��theMessage.message,Ҳ����socketID
        {
            //The cookie in this event is an ObjectID. Resolve that objectID into a pointer.
            //���¼���cookie��һ���������֣��ö����Ѿ������ñ��У�����Ϊָ��, ͨ���¼��еı�ʶ�ҵ���Ӧ�Ķ���ο�ָ��

            StrPtrLen idStr((char*)&theCurrentEvent.er_data, sizeof(theCurrentEvent.er_data));
            OSRef* ref = fRefTable.Resolve(&idStr);
            if (ref != NULL)
            {
                EventContext* theContext = (EventContext*)ref->GetObject();
#if DEBUG
                theContext->fModwatched = false;
#endif
                theContext->ProcessEvent(theCurrentEvent.er_eventbits);
					//�����¼���Ĭ��ʱֻ�����˶��¼��������fTask->Signal(Task::kReadEvent);
                fRefTable.Release(ref);		//��������,������ɾ������, ��˶�Ӧ����,���һ���¼�ʱ,�������ü���
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
