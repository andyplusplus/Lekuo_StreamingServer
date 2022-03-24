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
Event context�ṩ��һ�������ԣ���UNIX file descriptor (ͨ����EV_RE or EV_WR)��ȡ�¼���������һ������
    
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
    	//���캯��: ��������Ҫ��context���յ�EventThread���Լ���contextӦ�õ�fd
        EventContext(int inFileDesc, EventThread* inThread);
        virtual ~EventContext() { 
			if (fAutoCleanup) this->Cleanup(); 
		}
        
        //
        // InitNonBlocking
        //
        // Sets inFileDesc to be non-blocking. Once this is called, the EventContext object "owns" the file descriptor, and will close it
        // when Cleanup is called. This is necessary because of some weird select() behavior. DON'T CALL CLOSE ON THE FD ONCE THIS IS CALLED!!!!
        // ����inFileDescΪ��������һ���ú��������ã�EventContext����ӵ��file descriptor, ���ҵ�Cleanup�����Ժ�رգ���������select() �������Ϊ
        void            InitNonBlocking(int inFileDesc);

        //
        // Cleanup. Will be called by the destructor, but can be called earlier
        //�����������ã�Ҳ���ڴ�ǰ����
		//��fEventThread->fRefTable�����ñ���ɾ��EventContext���ر�fFileDesc Socket��
		//���Ұ�fFileDesc����ΪkInvalidFileDesc��fUniqueIDΪ0
        void            Cleanup();

        //
        // Arms this EventContext. Pass in the events you would like to receive
        //����������յ��¼�����
        void            RequestEvent(int theMask = EV_RE);

        
        //
        // Provide the task you would like to be notified
        // ����ϣ����֪ͨ������
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
        // ��ʵ�ֻ���TCPSocket��HTTP Proxy tunnels, ������Ҫά����context
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
        // ��file descriptor����һ���¼�ʱ�����������ã�Ĭ�ϵĲ����Ǵ���������񡣵������ɱ�����
        // ��ǰ��������һ��Task::kReadEvent

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

        int             fFileDesc;				//�¼��̶߳�Ӧ��Socket������

    private:

        struct eventreq fEventReq;				//�¼�����ṹ 

        //
		//  ��fUniqueIDStr:thisΨһ��ʶ�ö���
        OSRef           fRef;					//��Context�����ã�����ӵ�fEventThread->fRefTable
        PointerSizedInt fUniqueID;				//��ʶ��Context��ΨһID,�ñ�����ֵͨ��EventContext::RequestEvent(int theMask)���,��СΪWM_User+1
        StrPtrLen       fUniqueIDStr;			//��ʶ��Context���ַ���ID,�ñ�����fUniqueID�Ĺ�ϵ�ǳ�΢��,������Ptrֻ��ȡfUniqueID�ĵ�ַ,��ǿ��ת��,������ֵ�����fUniqueIDһ��

		//
        EventThread*    fEventThread;			//Context��Ӧ���¼��߳�
        Bool16          fWatchEventCalled;		//״̬����,��ʼΪFalse,��ΪFalseʱ,������ִ������fUniqueID, ��������,������select_watchevent(...); ��ΪTRUE,��ֱ�ӵ���select__modwatch(...),��������fUniqueID
        int             fEventBits;				//û��ʹ�õĳ�Ա����
        Bool16          fAutoCleanup;			//�Ƿ����Ŷ������٣�ִ��Cleanup()

        Task*           fTask;					//ϣ�������ŵ�����
#if DEBUG
        Bool16          fModwatched;
#endif
        
        static unsigned int sUniqueID;			//��Windowsϵͳ����ֵΪWM_USER
        
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
