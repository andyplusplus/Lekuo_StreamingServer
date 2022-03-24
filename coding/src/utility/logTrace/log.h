//
//  Log.h
//


#ifndef CLOG_H
#define CLOG_H

#include "LogPriorityLevel.h"
#include "LeKuo.h"

#ifdef _LOGTRACE_
	class __declspec(dllexport) CLog
#else
	class __declspec(dllimport) CLog
#endif
{
public:
	CLog();
	virtual ~CLog();

	static void SetLevel(PriorityLevel level = PL_INFOO);
	static void Log(const char * strLog, PriorityLevel level = PL_INFOO);
	static void LogFun_S	(const char* strFunName);
	static void LogFun_SI	(const char* strFunName, HANDLE hHandle);
	static void LogFun_SII	(const char* strFunName, int nDVRID, int nChannel);

	static void LogFun_SIISI(const char* strFunName, int nDVRID, int nChannel, const unsigned char* inBuffer, INT32 inBufferLength);
	static void LogFun_SS	(const char* strFunName, const char* strPara);

	static void LogFun_SIS	(const char* strFunName, const HANDLE hHandle, const char* strPara);
	static void LogFun_SST	(const char* strFunName, const char* strPara, const Tag inTag);
	static void LogFun_SSIT	(const char* strFunName, const char* strPara, const void* pCallback, const Tag inTag);
	static void LogFun_SSTI	(const char* strFunName, const char* strPara, const Tag inTag, const void* pCallback);

	static void LogFun_SIST	(const char* strFunName, const HANDLE hHandle, const char* strPara, const Tag inTag);
	static void LogFun_SISS	(const char* strFunName, const HANDLE hHandle, const char* ServerIP, const char* strPara);
	static void LogFun_SISSSI(const char* strFunName, int nDVRID, const char* pstrIP, const char* pstrUserName, const char* inPassWord, int port);
	static void LogFun_SSISS(const char* strFunName, const char* pstrIP, int port, const char* pstrUserName, const char* inPassWord);

	static void LogFun_SISIT(const char* strFunName, const HANDLE hHandle, const char* strPara, const void* pCallback, const Tag inTag);

private:
	static char* CLog::getTimeStr(char* pTimeInfo);
};

#endif  // CLOG_H
