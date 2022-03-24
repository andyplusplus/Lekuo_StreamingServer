// RSClientElem.cpp: implementation of the RSClientElem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RSClientElem.h"
#include "RsOSQueue.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RSClientElem::RSClientElem(const char* inElemPara, IRelayClient* inIRelayClient, Tag inTag, RSClientElem* inParentElem, HANDLE inContainedHandle/*���ⲿ�������Handle����*/)
: fIRelayClient(inIRelayClient), fTag(inTag), fParentElem(inParentElem), fContainedHandle(inContainedHandle)
{
	fQueueElem.SetEnclosingObject(this);

	memset(fElemPara, 0, MAX_CHAR_SIZE * sizeof(char));
	if (inElemPara)
	{
		strcpy(fElemPara, inElemPara);
	}

	if (fParentElem)
	{
		fParentElem->getQueue()->EnQueue(&fQueueElem);
	}
}


RSClientElem::~RSClientElem()
{

	if (fParentElem)
	{
		fParentElem->getQueue()->Remove(&fQueueElem);
	}
	
	DeleteAllElemInQueue();

	return;
}


//��fQueue�У�ѡ��ContainedHandle��inHandle��ͬ��QueueElement��������  tobe
RsOSQueueElem*  RSClientElem::getContainedElemFromSubQueue(HANDLE inContainedHandle)
{
	RsOSQueueElem* theResultElem = NULL;

	for (RsOSQueueIter iter(&fQueue); !iter.IsDone(); iter.Next())
	{
		
		RsOSQueueElem* theElem = iter.GetCurrent();
		if (theElem)
		{
			HANDLE theContainedHandle = (HANDLE) ( ((RSClientElem*)theElem)->getContainedHandle() );
		
			if (theContainedHandle == inContainedHandle)
			{
				theResultElem = theElem;
				break;
			}
		}
		
	}
	
	return theResultElem;
}


void RSClientElem::DeleteAllElemInQueue()
{

	RsOSQueueElem* theElem = fQueue.DeQueue();

	while(theElem)
	{
		RSClientElem* theRsElem =  (RSClientElem*)(theElem->GetEnclosingObject());
		delete theRsElem;
		
		theElem = fQueue.DeQueue();
	}

	return;
}