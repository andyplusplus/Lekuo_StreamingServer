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
	"0 与流媒体服务器会话正常!",//"Session went fine",
	"1 断开流媒体服务器失败!",//"Teardown failed, but session stats are all valid",
	"2 无法与流媒体服务器建立会话!",//"Session couldn't be setup, the server returned an error",
	"3 流媒体服务器应答错误!",//"Server sent back some bad SDP",
	"4 流媒体服务器无应答!",//"Server not responding",
	"5 连不上流媒体服务器!",//"Couldn't connect at all.",
	"6 在播放过程中，与流媒体连接中断!",//"Connection was forceably closed while playing the movie",
	"7 流媒体服务器测试结果 正常!",//"Session test is OK",
};

class IMsgReceiver
{
public:
		virtual unsigned long ReceiveMsg( HANDLE hSender, Tag inSenderTag, unsigned long inMsgType/*Enum LK_MSG_TYPE*/, unsigned char* inMsgData, unsigned long inDataLen, int isChanDeleted) = 0;
/*Channel 对象或DVR对象通过该接口回调
		hSender, 调用者
		inSenderTag, 消息用的Tag
		inMsgType: 回调消息类型，该内容转化为LK_MSG_TYPE枚举类型
		inMsgData, inDataLen: 消息的数据及数据长度。
		inIsChanDeleted:  调用者是否已删除
*/
};

typedef IMsgReceiver* LPIMsgReceiver;

#endif // !defined(LK_IMessageReceiv_H_INCLUDED_)
