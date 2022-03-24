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
	RSClientElem(const char* inElemPara, IRelayClient* inIRelayClient, Tag inTag, RSClientElem* inParentElem, HANDLE inContainedHandle/*���ⲿ�������Handle����*/);
	virtual ~RSClientElem();

	
	RsOSQueueElem* getQueueElem()	{return &fQueueElem;		};
	RsOSQueueElem* getContainedElemFromSubQueue(HANDLE inContainedHandle);	//��fQueue�У�ѡ��ContainedHandle��inHandle��ͬ��QueueElement��������
	

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
	
	// ��Relay Server Module�������豸���͵Ĳ�ͬ������ָ�����ݽṹά���Ķ�Ӧ�� ServerHandle, ChannelHandle, ConnectionHandle
	//   ����Relay Server�У�ChannelElement����ά����һ��Channel������fContainedHandle ָ�� DVRAdaptor �еĽ���Channel���ָ�����
	HANDLE fContainedHandle;
};

#endif // !defined(AFX_RSCLIENTOBJ_H__1305623B_C070_48AC_9920_7A90E100B989__INCLUDED_)
