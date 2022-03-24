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
    File:       UDPSocketPool.h

    Contains:   Object that creates & maintains UDP socket pairs in a pool.
创建以及维护池中UDP socket pair的对象
    
*/

#ifndef __UDPSOCKETPOOL_H__
#define __UDPSOCKETPOOL_H__

#include "UDPDemuxer.h"
#include "UDPSocket.h"
#include "OSMutex.h"
#include "OSQueue.h"

class UDPSocketPair;

class UDPSocketPool
{
    public:
    
        UDPSocketPool() : fMutex() {}
        virtual ~UDPSocketPool() {}
        
        //Skanky access to member data
        OSMutex*    GetMutex()          { return &fMutex; }
        OSQueue*    GetSocketQueue()    { return &fUDPQueue; }
        
        //从pool中获得一个UDP socket
        //inIPAddr = 你希望该pair绑定的IP address.
        //inPort = 你希望该pair绑定的port. 或者设置为0, 表示你对此无所谓
        //inSrcIPAddr = srcIP address of incoming packets for the demuxer.
        //inSrcPort = src port of incoming packets for the demuxer.
        //如果没有合适的地址对满足要求,返回NULL
        UDPSocketPair*  GetUDPSocketPair(UInt32 inIPAddr, UInt16 inPort,
                                            UInt32 inSrcIPAddr, UInt16 inSrcPort);
        
        //When done using a UDP socket pair retrieved via GetUDPSocketPair, you must
        //call this function. Doing so tells the pool which UDP sockets are in use,
        //keeping the number of UDP sockets allocated at a minimum.
        void ReleaseUDPSocketPair(UDPSocketPair* inPair);

        UDPSocketPair*  CreateUDPSocketPair(UInt32 inAddr, UInt16 inPort);
        
    protected:
    
        //Because UDPSocket is a base class, and this pool class is intended to be
        //a general purpose class for all types of UDP sockets (reflector, standard),
        //there must be a virtual fuction for actually constructing the derived UDP sockets
		//由于UDPSocketPool是一个基类,该类目的是为各种UDP socket提供一种操作,因此派生类必须实现以下的两个函数
        virtual UDPSocketPair*  ConstructUDPSocketPair() = 0;
        virtual void            DestructUDPSocketPair(UDPSocketPair* inPair) = 0;
        
        virtual void            SetUDPSocketOptions(UDPSocketPair* /*inPair*/) {}
    
    private:
    
        enum		//refer to ClientSession.h  for kLowestUDPPort_Client, this two value is related
        {
            kLowestUDPPort = 6970,  //UInt16	//Other is unsed in ClientSession.h
            kHighestUDPPort = 36969 //UInt16	//2008.06.14 changed from 65535 to 36969
        };
    
        OSQueue fUDPQueue;		//UDP socket的队列
        OSMutex fMutex;
};

class UDPSocketPair
{
    public:
        
        UDPSocketPair(UDPSocket* inSocketA, UDPSocket* inSocketB)
            : fSocketA(inSocketA), fSocketB(inSocketB), fRefCount(0), fElem() {fElem.SetEnclosingObject(this);}
        ~UDPSocketPair() {}
    
        UDPSocket*  GetSocketA() { return fSocketA; }
        UDPSocket*  GetSocketB() { return fSocketB; }
        
    private:
    
        UDPSocket*  fSocketA;		//Socket A, RTP 用于发送数据
        UDPSocket*  fSocketB;		//Socket B, RTCP用于接收数据
        UInt32      fRefCount;		//对该端口对的引用计数器
        OSQueueElem fElem;			//队列元素
        
        friend class UDPSocketPool;
};
#endif // __UDPSOCKETPOOL_H__

