#ifndef __CLIENT__H_
#define __CLIENT__H_

#ifdef CLIENT_EXPORTS
#define CLIENT_API __declspec(dllexport)
#else
#define CLIENT_API __declspec(dllimport)
#endif

#define   COMMAND_LENGTH   32

enum datatype
{
	DataType_Video,
	DataType_Audio,
	DataType_Control,
	
//	//added, the above useless now
//	SOCKET_MESSAGE,
};

typedef void (WINAPI *pFunction)(LPBYTE,DWORD,int Flag,int WatchID,DWORD dwSequence);
  
///////////////////////////////////////////////////////////////
CLIENT_API void  CLIENT_Close();
CLIENT_API BOOL  CLIENT_Init(HWND hWnd,pFunction pFun);
CLIENT_API BOOL  CLIENT_Open(char * FrontIP, unsigned short HostPort=37777);
CLIENT_API void  CLIENT_Read(WPARAM pSock);
CLIENT_API void  CLIENT_WaitClose();//调用该函数实现关闭socket连接

CLIENT_API BOOL  CLIENT_SetActive(BYTE WatchID);

//no import
CLIENT_API BOOL SendData(LPVOID lpData,DWORD Length,DWORD _flags=0,DWORD dwTimeOut=10000);

extern pFunction _pFun;

#endif //#ifndef __CLIEMT__H_
