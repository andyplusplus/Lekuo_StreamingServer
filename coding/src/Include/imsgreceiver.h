//#include "IMsgReceiver.h"

#if !defined(LK_IMessageReceiv_H_INCLUDED_)
#define LK_IMessageReceiv_H_INCLUDED_

#include "LeKuo.h"

enum LK_MSG_TYPE
{
	LK_MSG_TYPE_Min =	0,
	//Session Die Reasion
	LK_SESSION_fine =	0,					// Session went fine
	LK_SESSION_TeardownFailed   = 1,		// Teardown failed, but session stats are all valid
	LK_SESSION_RequestFailed    = 2,		// Session couldn't be setup, the server returned an error
	LK_SESSION_BadSDP           = 3,		// Server sent back some bad SDP
	LK_SESSION_Timedout			= 4,		// Server not responding
	LK_SESSION_ConnectionFailed = 5,		// Couldn't connect at all.
	LK_SESSION_DiedWhilePlaying = 6,		// Connection was forceably closed while playing the movie
	LK_SESSION_Response_Test_OK = 7,		// When client send request  "ServerIP=127.0.0.1;ClientType=UDP;" with CreateDVR, and "PingServer=1" with CreateChannel, the server will reponse this when it is OK
	LK_SESSION_Max,
};


const char LK_SESSION_DIE_REASON_STR[LK_SESSION_Max][LK_KEYWORD_MaxLen] =
{
	"0 ����ý��������Ự����!",//"Session went fine",
	"1 �Ͽ���ý�������ʧ��!",//"Teardown failed, but session stats are all valid",
	"2 �޷�����ý������������Ự!",//"Session couldn't be setup, the server returned an error",
	"3 ��ý�������Ӧ�����!",//"Server sent back some bad SDP",
	"4 ��ý���������Ӧ��!",//"Server not responding",
	"5 ��������ý�������!",//"Couldn't connect at all.",
	"6 �ڲ��Ź����У�����ý�������ж�!",//"Connection was forceably closed while playing the movie",
	"7 ��ý����������Խ�� ����!",//"Session test is OK",
};

class IMsgReceiver
{
public:
		virtual unsigned long ReceiveMsg( HANDLE hSender, Tag inSenderTag, unsigned long inMsgType/*Enum LK_MSG_TYPE*/, unsigned char* inMsgData, unsigned long inDataLen, int isChanDeleted) = 0;
/*Channel �����DVR����ͨ���ýӿڻص�
		hSender, ������
		inSenderTag, ��Ϣ�õ�Tag
		inMsgType: �ص���Ϣ���ͣ�������ת��ΪLK_MSG_TYPEö������
		inMsgData, inDataLen: ��Ϣ�����ݼ����ݳ��ȡ�
		inIsChanDeleted:  �������Ƿ���ɾ��
*/
};

typedef IMsgReceiver* LPIMsgReceiver;

#endif // !defined(LK_IMessageReceiv_H_INCLUDED_)
