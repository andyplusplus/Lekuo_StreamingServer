// RSClientElem.h: interface for the RSClientObj class.
//    Used by RelayClientDLL, and RsDVRAdaptor
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSCLIENTOBJ_H__1305623B_C070_48AC_9920_7A90E100B989__INCLUDED_)
#define AFX_RSCLIENTOBJ_H__1305623B_C070_48AC_9920_7A90E100B989__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RsOsQueue.h"
#include "LeKuo.h"

//The Device type for the organization of basic element
enum EN_ELEM_TYPES
{
	ELEM_TYPE_UNDEFINE	=	0,
	ELEM_TYPE_INSTANCE,
	ELEM_TYPE_SERVER,
	ELEM_TYPE_DEVICE,
	ELEM_TYPE_CHANNEL,
	ELEM_TYPE_CONNECTION = 5,
	ELEM_TYPE_NUMBERS	=	6
};


class RSClientElem
{
public:
	RSClientElem(const char* inElemPara, IRelayClient* inIRelayClient, Tag inTag, RSClientElem* inParentElem, HANDLE inContainedHandle/*由外部所传入的Handle对象*/);
	virtual ~RSClientElem();

	
	RsOSQueueElem* getQueueElem()	{return &fQueueElem;		};
	RsOSQueueElem* getContainedElemFromSubQueue(HANDLE inContainedHandle);	//从fQueue中，选出ContainedHandle与inHandle相同的QueueElement，并返回
	

	RsOSQueue* getQueue()			{	return &fQueue;				}; 
	UINT getQueueLength()			{   return fQueue.GetLength();	} ;
	void DeleteAllElemInQueue();

	RSClientElem* getParentElem()	{	return fParentElem;			};
	Tag getTag()					{	return fTag;				};
	char* getElementCreationPara()	{	return fElemPara;			};
	

	void setInterface(IRelayClient* inIRelayClient)		{fIRelayClient = inIRelayClient;	};
	IRelayClient* getIRelayClient()	{	return fIRelayClient;	};
	

	void setContainedHandle(HANDLE inContainedHandle)		{fContainedHandle = inContainedHandle;};		
	HANDLE getContainedHandle()		{return fContainedHandle;};
	
protected:
	EN_ELEM_TYPES fDeviceType;
	
	RsOSQueue fQueue;						//this class have sub element
	RsOSQueueElem fQueueElem;				//this class is the element of another queues
	RSClientElem* fParentElem;				//Referto Parent Handle
	
	char fElemPara[MAX_CHAR_SIZE];			//Device config parameter
	Tag fTag;								//Tag of this Object
	IRelayClient* fIRelayClient;			//Data Process implementaion object for this element
	
	// 在Relay Server Module，依据设备类型的不同，用于指向数据结构维护的对应的 ServerHandle, ChannelHandle, ConnectionHandle
	//   如在Relay Server中，ChannelElement对象维护了一个Channel容器，fContainedHandle 指向 DVRAdaptor 中的建立Channel后的指针对象
	HANDLE fContainedHandle;
};

#endif // !defined(AFX_RSCLIENTOBJ_H__1305623B_C070_48AC_9920_7A90E100B989__INCLUDED_)
