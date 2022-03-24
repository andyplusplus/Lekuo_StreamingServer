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
 *  ev.h
   网络事件相关的处理
 */

#ifndef _SYS_EV_H_
#define _SYS_EV_H_

#if !defined(__Win32__) && !defined(__solaris__) && !defined(__sgi__) && !defined(__osf__) && !defined(__hpux__)
    #include <sys/queue.h>
#endif

//
//该结构在win32ev.cpp的int select__modwatch(struct eventreq *req, int which)中有原始应用
//
struct eventreq {	//以下标注摘自void EventContext::RequestEvent(
  int      er_type;		//始终设置为EV_FD
#define EV_FD 1			// file descriptor
  int      er_handle;	//指向socket，实际为socket描述符
  void    *er_data;		//指向socket的唯一传递的消息内容, 实际上常常设置为标识该Context的唯一ID (&fUniqueID),该值和select__waitevent中指向获得的消息的消息体的相同的
  int      er_rcnt;		
  int      er_wcnt;
  int      er_ecnt;
  int      er_eventbits;//设置为事件的掩码,在RequestEvent(int theMask)中使用
#define EV_RE  1	//读取数据
#define EV_WR  2	//写入socket数据
#define EV_EX  4	//未使用		
#define EV_RM  8	//未使用
};

typedef struct eventreq *er_t;

#ifdef _KERNEL

#define EV_RBYTES 0x1
#define EV_WBYTES 0x2
#define EV_RWBYTES (EV_RBYTES|EV_WBYTES)
#define EV_RCLOSED 0x4
#define EV_RCONN  0x8
#define EV_ERRORS  0x10
#define EV_WCLOSED 0x20
#define EV_WCONN   0x40
#define EV_OOBD    0x80
#define EV_OOBM    0x100

struct eventqelt {
  TAILQ_ENTRY(eventqelt)  ee_slist;
  TAILQ_ENTRY(eventqelt)  ee_plist;
  struct eventreq  ee_req;
  struct proc *    ee_proc;
  u_int            ee_flags;
#define EV_QUEUED 1
  u_int            ee_eventmask;
  struct socket   *ee_sp;
};

#endif /* _KERNEL */

#if !MACOSXEVENTQUEUE


//请求事件
//   1.等待窗口创建
//   2.设置事件类型
//   3.等等Socket网络事件，然后给窗口发一个消息
int select_watchevent(struct eventreq *req, int which);


//请求事件
//   1.等待窗口创建
//   2.设置事件类型
//   3.等等Socket网络事件，然后给窗口发一个消息
int select_modwatch(struct eventreq *req, int which);


int select_waitevent(struct eventreq *req, void* onlyForMOSX);
void select_startevents();
int select_removeevent(int which);
//_WZD_ ExitThread _Start
	//#ifdef SOCKET
		int WZD_Signal_EventThread_To_Exit();
		int WZD_WSAAsyncSelect(SOCKET s, unsigned int wMsg, long lEvent);
	//#endif
//_WZD_ ExitThread _End

#endif

#endif /* _SYS_EV_H_ */
