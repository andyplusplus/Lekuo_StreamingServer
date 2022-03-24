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
    File:       win32ev.cpp

    Contains:   WSA implementation of socket event queue functions.
    
    Written By: Denis Serenyi

    
*/

#include "ev.h"
#include "OSHeaders.h"
#include "OSThread.h"
#include "MyAssert.h"

//
// You have to create a window to get socket events? What's up with that?
static HWND sMsgWindow = NULL;
//_WZD_ADD_Start
static int sSystemIsExiting = 0;	//The system is in exiting status
//_WZD_ADD_End


//
LRESULT CALLBACK select_wndproc(HWND inWIndow, UINT inMsg, WPARAM inParam, LPARAM inOtherParam);

void select_startevents()
{
    //
    // This call occurs from the main thread. In Win32, apparently, you
    // have to create your WSA window from the same thread that calls GetMessage.
    // So, we have to create the window from select__waitevent
    return;
}

int select_removeevent(int /*which*/)
{
    //
    // Not needed for WSA.
    return 0;
}

int select_watchevent(struct eventreq *req, int which)
{
    return select_modwatch(req, which);
}

//请求事件
//   1.等待窗口创建
//   2.设置事件类型
//   3.等等Socket网络事件，然后给窗口发一个消息
int select_modwatch(struct eventreq *req, int which)
{
//1.等待窗口创建
	//如果我们的WSAAsyncSelect窗口还没创建，则等待到创建完成。当服务器启动完成后，窗口创建完成。注: 只有当服务器正在启动时，调用select__modwatch才分发生。
    while (sMsgWindow == NULL)
        OSThread::Sleep(10);

//2.设置事件类型	
    // Convert EV_RE and EV_WR to the proper WSA event codes. WSA event codes are more specific than what POSIX provides, so just wait on any kind of read related event for EV_RE, same for EV_WR
	//转换EV_RE以及EV_WR为合适的WSA事件代码，WSA事件代码比POSIX提供的更具体，所以只是等待任何类型的针对EV_RE读事件，对EV_WR来说也是同样的
    long theEvent = 0;
    if (which & EV_RE)
        theEvent |= FD_READ | FD_ACCEPT | FD_CLOSE;
    if (which & EV_WR)
        theEvent |= FD_WRITE | FD_CONNECT;

//3.等等Socket网络事件，然后给窗口发一个消息
    // This is a little bit of a hack, 
    // because we are assuming that the caller is actually putting a UInt32 in the void*, 
    // not a void*, and we are also assuming caller is not using the 0 - WM_USER range of values, 
    // but both of these things are true in the EventContext.cpp code, 
    // and this mechanism of passing around cookies is just too convienent to ignore.
	// 这里有点类拟hack，因为我们假定调用者实际在void*设置了Uint32，并且假定调用者没有使用0 - WM_USER范围以内的值，这两者在EventContext.cpp代码中确定如些。这种传递参数的机制只是太方便而不能忽略。

    unsigned int theMsg = (unsigned int)(req->er_data);
    
    return ::WSAAsyncSelect(req->er_handle, sMsgWindow, theMsg, theEvent);		//theMsg参数主要存在多个socket时使用
	//注：WSAAsyncSelect在Socket有网络事件时，利用windows消息机制，通知对socket进行操作。
	//该函数传递的参数为: WSAAsyncSelect(SOCKET s, HWND hWnd, unsigned int uMsg, long lEvent)。
	//	  s       就是我们感兴趣的套接字；
	//    hWnd    是接收消息通知那个窗口句柄；
	//	  wMsg    指定在发生网络事件时要接受的消息，通常设成比WM_USER大的一个值，以避免消息冲突；
	//	  lEvent  指定了一个位掩码，对应一系列网络事件的组合
    // https://baike.baidu.com/item/WSAAsyncSelect%28%29/10141914?fr=aladdin
}

//该函数由void EventThread::Entry()死循环调用，用于确认是否有数据在socket上等待
//This file is included in CommonUtiliesLib, if Client then _I_CommonUtilitiesLib_CLient_ is defined
int select_waitevent(struct eventreq *req, void* /*onlyForMacOSX*/)
{
	enum WndNameTypes{
		WndName_Server = 0,
		WndName_Client = 1	
	} theWndName = WndName_Server;

	char WndName[][64] = {"LeKuoServerWindow", "LeKuoClientWindow"};
#ifdef _I_CommonUtilitiesLib_CLient_
	theWndName = WndName_Client;
#endif
	//_WZD_ADD_Start     
	if(sSystemIsExiting)
	{
		return WZD_Event_Thread_Received_WM_QUIT_Message;	
	}
	//_WZD_ADD_End
    
	if (sMsgWindow == NULL) 
    {   //如果第一次调用该函数，则执行窗口初始化，我们只是建立最简单的窗口
        // This is the first time we've called this function. Do our window initialization now.
        
        // We basically just want the simplest window possible.
        WNDCLASSEX theWndClass;
        theWndClass.cbSize = sizeof(theWndClass);
        theWndClass.style = 0;
        theWndClass.lpfnWndProc = &select_wndproc;
        theWndClass.cbClsExtra = 0;
        theWndClass.cbWndExtra = 0;
        theWndClass.hInstance = NULL;
        theWndClass.hIcon = NULL;
        theWndClass.hCursor = NULL;
        theWndClass.hbrBackground = NULL;
        theWndClass.lpszMenuName = NULL;
        theWndClass.lpszClassName = WndName[theWndName];//"LeKuoServerWindow";
        theWndClass.hIconSm = NULL;
        
        ATOM theWndAtom = ::RegisterClassEx(&theWndClass);
		DWORD theError = 0;
		if (!theWndAtom)
		{
			theError = ::GetLastError();
		}
        Assert(theWndAtom != NULL);
        if (theWndAtom == NULL)
            ::exit(-1); // Poor error recovery, but this should never happen.
                
        sMsgWindow = ::CreateWindow(    WndName[theWndName],  // Window class name
                                        WndName[theWndName],  // Window title bar
                                        WS_POPUP,   // Window style ( a popup doesn't need a parent )
                                        0,          // x pos
                                        0,          // y pos
                                        CW_USEDEFAULT,  // default width
                                        CW_USEDEFAULT,  // default height
                                        NULL,           // No parent
                                        NULL,           // No menu handle
                                        NULL,           // Ignored on WinNT
                                        NULL);          // data for message proc. Who cares?
        Assert(sMsgWindow != NULL);
        if (sMsgWindow == NULL)
            ::exit(-1);
    }//创建窗口
    
    MSG theMessage;
    
    //
    // Get a message for my goofy window. 0, 0 indicates that we want any message for that window. Convienently, this function blocks until there is a message, so it works
	// 从窗口获取消息， 0, 0 表明希望从该窗口接收任何消息，该函数阻塞到消息到来为止
	//     当select__modwatch接收到指定网络事件时，会给该窗口发消息
    UInt32 theErr = ::GetMessage(&theMessage, sMsgWindow, 0, 0);
    
    if (theErr > 0)
    {
		//获得socket消息
        UInt32 theSelectErr = WSAGETSELECTERROR(theMessage.lParam);
        UInt32 theEvent = WSAGETSELECTEVENT(theMessage.lParam);
        
        req->er_handle = theMessage.wParam; // the wParam is the FD  //wParam参数指定在其上面发生了一个网络事件的套接字。
        req->er_eventbits = EV_RE;         //设置为事件的掩码,在RequestEvent(int theMask)中使用
			// WSA events & socket events don't map... but the server state machines never care what the event is anyway.
		    

        // we use the message # as our way of passing around the user data.
        req->er_data = (void*)(theMessage.message);			 //设置为标识该Context的唯一ID (&fUniqueID)
        
        //
        // We should prevent this socket from getting events until modwatch is called. 
		//    To cancel all notification indicating that Windows Sockets should send no further messages 
		//    related to network events on the socket, lEvent is set to zero.
		// 在ModWatch调用以前阻止socket获得事件，也就是取消select__modwatch中的WSAAsyncSelect
        (void)::WSAAsyncSelect(req->er_handle, sMsgWindow, 0, 0); 
        
        return 0;
    }
    else
    {   //  表示收到Quitmessage， Do we ever get WM_QUIT messages? Can there ever be an error?
        //_WZD_Comment_Off_Start _WZD_ ExitThread _Start   注册以下代码
		//Assert(0);
		//return EINTR;
		//_WZD_Comment_Off_End  _WZD_ ExitThread _End

		//_WZD_ADD_Start
		sSystemIsExiting = 1;
		int theResult = ::DestroyWindow(sMsgWindow);
		if(!theResult)
		{
			DWORD theERROR = GetLastError();
			theERROR = theERROR;
		}
		sMsgWindow = NULL;

		return WZD_Event_Thread_Received_WM_QUIT_Message;
		//_WZD_ADD_End
    }
}


LRESULT CALLBACK select_wndproc(HWND /*inWIndow*/, UINT inMsg, WPARAM /*inParam*/, LPARAM /*inOtherParam*/)
{
    // If we don't return true for this message, window creation will not proceed
    if (inMsg == WM_NCCREATE)
        return TRUE;
    
    // All other messages we can ignore and return 0
    return 0;
}


//_WZD_ ExitThread _Start
int WZD_Signal_EventThread_To_Exit()
{
	int theResult = PostMessage(sMsgWindow, WM_QUIT, 0, 0);
	if(!theResult)
	{
		DWORD theERROR = GetLastError();
		theERROR = theERROR;
		//Assert(!"PostMessage Failed!");	
	}


	return theResult;
}

//refer to MSDN: WSAAsyncSelect, if both wMsg, lEvent means "I dont want to receive any data"
int WZD_WSAAsyncSelect(SOCKET s, unsigned int wMsg, long lEvent)
{
	(void)::WSAAsyncSelect(s, sMsgWindow, wMsg, lEvent);
	return 0;
}

//_WZD_ ExitThread _End
