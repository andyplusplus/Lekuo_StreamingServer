
/*
    File:       RsOSQueue.cpp

    Contains:   implements RsOSQueue class
                    

*/
#include "StdAfx.h"
#include "RsOSQueue.h"

void RsAssert(int inCondition)
{
	if(! inCondition)
	{
		DebugBreak();
	}
};


RsOSQueue::RsOSQueue() : fLength(0)
{
    fSentinel.fNext = &fSentinel;
    fSentinel.fPrev = &fSentinel;
}

void RsOSQueue::EnQueue(RsOSQueueElem* elem)
{
    RsAssert(elem != NULL);
    if (elem->fQueue == this)
        return;
    RsAssert(elem->fQueue == NULL);
    elem->fNext = fSentinel.fNext;
    elem->fPrev = &fSentinel;
    elem->fQueue = this;
    fSentinel.fNext->fPrev = elem;
    fSentinel.fNext = elem;
    fLength++;
}

RsOSQueueElem* RsOSQueue::DeQueue()
{
    if (fLength > 0)
    {
        RsOSQueueElem* elem = fSentinel.fPrev;
        RsAssert(fSentinel.fPrev != &fSentinel);
        elem->fPrev->fNext = &fSentinel;
        fSentinel.fPrev = elem->fPrev;
        elem->fQueue = NULL;
        fLength--;
        return elem;
    }
    else
        return NULL;
}

void RsOSQueue::Remove(RsOSQueueElem* elem)
{
    RsAssert(elem != NULL);
    RsAssert(elem != &fSentinel);
    
    if (elem->fQueue == this)
    {
        elem->fNext->fPrev = elem->fPrev;
        elem->fPrev->fNext = elem->fNext;
        elem->fQueue = NULL;
        fLength--;
    }
}

#if RsOSQueueTESTING
unsigned short RsOSQueue::Test()
{
    RsOSQueue theVictim;
    void *x = (void*)1;
    RsOSQueueElem theElem1(x);
    x = (void*)2;
    RsOSQueueElem theElem2(x);
    x = (void*)3;
    RsOSQueueElem theElem3(x);
    
    if (theVictim.GetHead() != NULL)
        return false;
    if (theVictim.GetTail() != NULL)
        return false;
    
    theVictim.EnQueue(&theElem1);
    if (theVictim.GetHead() != &theElem1)
        return false;
    if (theVictim.GetTail() != &theElem1)
        return false;
    
    RsOSQueueElem* theElem = theVictim.DeQueue();
    if (theElem != &theElem1)
        return false;
    
    if (theVictim.GetHead() != NULL)
        return false;
    if (theVictim.GetTail() != NULL)
        return false;
    
    theVictim.EnQueue(&theElem1);
    theVictim.EnQueue(&theElem2);

    if (theVictim.GetHead() != &theElem1)
        return false;
    if (theVictim.GetTail() != &theElem2)
        return false;
        
    theElem = theVictim.DeQueue();
    if (theElem != &theElem1)
        return false;

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem2)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem2)
        return false;

    theVictim.EnQueue(&theElem1);
    theVictim.EnQueue(&theElem2);
    theVictim.EnQueue(&theElem3);

    if (theVictim.GetHead() != &theElem1)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem1)
        return false;

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem2)
        return false;

    if (theVictim.GetHead() != &theElem3)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theElem = theVictim.DeQueue();
    if (theElem != &theElem3)
        return false;

    theVictim.EnQueue(&theElem1);
    theVictim.EnQueue(&theElem2);
    theVictim.EnQueue(&theElem3);
    
    RsOSQueueIter theIterVictim(&theVictim);
    if (theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != &theElem3)
        return false;
    theIterVictim.Next();
    if (theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != &theElem2)
        return false;
    theIterVictim.Next();
    if (theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != &theElem1)
        return false;
    theIterVictim.Next();
    if (!theIterVictim.IsDone())
        return false;
    if (theIterVictim.GetCurrent() != NULL)
        return false;

    theVictim.Remove(&theElem1);

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theVictim.Remove(&theElem1);

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem3)
        return false;

    theVictim.Remove(&theElem3);

    if (theVictim.GetHead() != &theElem2)
        return false;
    if (theVictim.GetTail() != &theElem2)
        return false;

    return true;
}   
#endif



void RsOSQueueIter::Next()
{
    if (fCurrentElemP == fQueueP->GetTail())
        fCurrentElemP = NULL;
    else
        fCurrentElemP = fCurrentElemP->Prev();
}

// 
// RsOSQueueElem* RsOSQueue_Blocking::DeQueueBlocking(OSThread* inCurThread, SInt32 inTimeoutInMilSecs)
// {
//     OSMutexLocker theLocker(&fMutex);
// #ifdef __Win32_
//      if (fQueue.GetLength() == 0) 
// 	 {	fCond.Wait(&fMutex, inTimeoutInMilSecs);
// 		return NULL;
// 	 }
// #else
//     if (fQueue.GetLength() == 0) 
//         fCond.Wait(&fMutex, inTimeoutInMilSecs);
// #endif
// 
//     RsOSQueueElem* retval = fQueue.DeQueue();
//     return retval;
// }
// 
// RsOSQueueElem*    RsOSQueue_Blocking::DeQueue()
// {
//     OSMutexLocker theLocker(&fMutex);
//     RsOSQueueElem* retval = fQueue.DeQueue(); 
//     return retval;
// }
// 
// 
// void RsOSQueue_Blocking::EnQueue(RsOSQueueElem* obj)
// {
//     {
//         OSMutexLocker theLocker(&fMutex);
//         fQueue.EnQueue(obj);
//     }
//     fCond.Signal();
// }
