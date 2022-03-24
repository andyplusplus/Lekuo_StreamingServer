
/*
    File:       RsOSQueue.h

    Contains:   implements RsOSQueue class
                    
    
*/

#ifndef _RsOSQueue_H_
#define _RsOSQueue_H_

//#include "OSCond.h"

#define RsOSQueueTESTING 0

class RsOSQueue;

void RsAssert(int inCondition);

class RsOSQueueElem {
    public:

        RsOSQueueElem(void* enclosingObject = NULL) : fNext(NULL), fPrev(NULL), fQueue(NULL),
                                                    fEnclosingObject(enclosingObject) 
		{
			fQueue = fQueue;
		}


        virtual ~RsOSQueueElem() { 
			RsAssert(fQueue == NULL); 
		}



        unsigned short IsMember(const RsOSQueue& queue) { return (&queue == fQueue); }	//通过判断父链表是否相同确定是否是队列的成员
        unsigned short IsMemberOfAnyQueue()     { return fQueue != NULL; }				//判断是否已经加入链
        void* GetEnclosingObject()  { return fEnclosingObject; }
        void SetEnclosingObject(void* obj) { fEnclosingObject = obj; }

        RsOSQueueElem* Next() { return fNext; }
        RsOSQueueElem* Prev() { return fPrev; }
        RsOSQueue* InQueue()  { return fQueue; }
        inline void Remove();

    private:

        RsOSQueueElem*    fNext;
        RsOSQueueElem*    fPrev;
        RsOSQueue *       fQueue;				//指向父链表
        void*           fEnclosingObject;		// RsOSQueueElem所容纳的元素

        friend class    RsOSQueue;
};

//该类为一个队列类，这是一个变通的链表类
class RsOSQueue {
    public:
        RsOSQueue();
        ~RsOSQueue() {}

        void            EnQueue(RsOSQueueElem* object);
        RsOSQueueElem*    DeQueue();

        RsOSQueueElem*    GetHead()		{ if (fLength > 0) return fSentinel.fPrev; return NULL; }
        RsOSQueueElem*    GetTail()		{ if (fLength > 0) return fSentinel.fNext; return NULL; }
        unsigned long     GetLength()	{ return fLength; }
        
        void            Remove(RsOSQueueElem* object);

#if RsOSQueueTESTING
        static unsigned short       Test();
#endif

    protected:

        RsOSQueueElem     fSentinel;	//该元素的fPrev以及fNext分别指向链表头和链表尾
        unsigned long          fLength;	//链表长度
};

//对链表类执行迭代操作，这是一个变通的迭代类
class RsOSQueueIter
{
    public:
        RsOSQueueIter(RsOSQueue* inQueue) : fQueueP(inQueue), fCurrentElemP(inQueue->GetHead()) {}
        RsOSQueueIter(RsOSQueue* inQueue, RsOSQueueElem* startElemP ) : fQueueP(inQueue)
            {
                if ( startElemP )
                {   
					RsAssert( startElemP->IsMember(*inQueue ) );
                    fCurrentElemP = startElemP;
                
                }
                else
                    fCurrentElemP = NULL;
            }
        ~RsOSQueueIter() {}
        
        void            Reset() { fCurrentElemP = fQueueP->GetHead(); }
        
        RsOSQueueElem*    GetCurrent() { return fCurrentElemP; }
        void   Next();
        
        unsigned short          IsDone() { return fCurrentElemP == NULL; }
        
    private:
    
        RsOSQueue*        fQueueP;
        RsOSQueueElem*    fCurrentElemP;
};
// 
// //这是一个基于阻塞的队列类
//class RsOSQueue_Blocking
// {
//     public:
//         RsOSQueue_Blocking() {}
//         ~RsOSQueue_Blocking() {}
//         
//         RsOSQueueElem*    DeQueueBlocking(OSThread* inCurThread, SInt32 inTimeoutInMilSecs);
//         RsOSQueueElem*    DeQueue();//will not block
//         void            EnQueue(RsOSQueueElem* obj);
//         
//         OSCond*         GetCond()   { return &fCond; }
//         RsOSQueue*        GetQueue()  { return &fQueue; }
//         
//     private:
// 
//         OSCond              fCond;
//         OSMutex             fMutex;
//         RsOSQueue           fQueue;
// };
// 

void    RsOSQueueElem::Remove()
{
    if (fQueue != NULL)
        fQueue->Remove(this);
}

#endif //_RsOSQueue_H_
