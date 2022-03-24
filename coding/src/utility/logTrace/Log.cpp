//#include "stdafx.h"

#include "stdafx.h"
#include "Log.h"

#include "ScopeTrace.h"

#define _LogEnabled_ 1

CLog::CLog()
{
	SetLevel();
}


CLog::~CLog()
{

};


void CLog::SetLevel(PriorityLevel level)
{
#ifndef _RSSTUDIO6_
	SETTRLEVEL(level);
#endif
};



void CLog::Log(const char * strLog, PriorityLevel level)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	//
	char TimeInfo[128];
	memset(TimeInfo, 0, 128);
	//
	sprintf(LogInfo, "%-18s  %s", getTimeStr((char*)TimeInfo), strLog);
	//
 	#ifdef _DEBUG
 		OutputDebugString(LogInfo);
		OutputDebugString("\n");
 	#endif 
		

	if (_LogEnabled_)
	TROUT(LogInfo, level, false);

};
 


void CLog::LogFun_SSIT(const char* strFunName, const char* strPara, const void* pCallback, const Tag inTag)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);

	sprintf(LogInfo, "%s( \"%s\", 0X%lx, %ld) ", strFunName, strPara, pCallback, inTag);
	CLog::Log(LogInfo);
}


void CLog::LogFun_SSTI(const char* strFunName, const char* strPara, const Tag inTag, const void* pCallback)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	
	sprintf(LogInfo, "%s( \"%s\", %ld, 0X%lx) ", strFunName, strPara, inTag, pCallback);
	CLog::Log(LogInfo);
}


void CLog::LogFun_SIST(const char* strFunName, const HANDLE hHandle, const char* strPara, const Tag inTag)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);

	sprintf(LogInfo, "%s( 0X%lx, \"%s\", %ld) ", strFunName, hHandle, strPara, inTag);
	CLog::Log(LogInfo);
}

void CLog::LogFun_SST(const char* strFunName, const char* strPara, const Tag inTag)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	
	sprintf(LogInfo, "%s( \"%s\", %ld ) ", strFunName, strPara, inTag);
	CLog::Log(LogInfo);
};


void CLog::LogFun_SISS(const char* strFunName, const HANDLE hHandle, const char* ServerIP, const char* strPara)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	
	sprintf(LogInfo, "%s( 0X%lx, \"%s\", \"%s\")", strFunName, hHandle, ServerIP, strPara);
	CLog::Log(LogInfo);
};


void CLog::LogFun_SISSSI(const char* strFunName, int nDVRID, const char* pstrIP, const char* pstrUserName, const char* inPassWord, int port)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	
	sprintf(LogInfo, "%s( %d, \"%s\", \"%s\",  \"%s\", %d)", strFunName, nDVRID, pstrIP, pstrUserName, inPassWord, port);
	CLog::Log(LogInfo);
};

void CLog::LogFun_SSISS(const char* strFunName, const char* pstrIP, int port, const char* pstrUserName, const char* inPassWord)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);

	sprintf(LogInfo, "%s( %s, \"%d\", \"%s\",  \"%s\")", strFunName, pstrIP, port, pstrUserName, inPassWord);
	CLog::Log(LogInfo);
}


void CLog::LogFun_SISIT(const char* strFunName, const HANDLE hHandle, const char* strPara, const void* pCallback, const Tag inTag)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	
	sprintf(LogInfo, "%s( 0X%lx, \"%s\", 0X%lx, \"%ld\")", strFunName, hHandle, strPara, pCallback, inTag);
	CLog::Log(LogInfo);
};


void CLog::LogFun_S(const char* strFunName)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	//
	sprintf(LogInfo, "%s() ", strFunName);
	//
	CLog::Log(LogInfo);	
	//
}


void CLog::LogFun_SI(const char* strFunName, const HANDLE hHandle)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	//
	sprintf(LogInfo, "%s( 0X%lx) ", strFunName, hHandle);
	//
	CLog::Log(LogInfo);
	//
}


void CLog::LogFun_SII(const char* strFunName, int nDVRID, int nChannel)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	//
	sprintf(LogInfo, "%s( %d, %d )", strFunName, nDVRID, nChannel);
	//
	CLog::Log(LogInfo);
	//
}

/*
void CLog::LogFun_SIIII(const char* strFunName, int nDVRID, int nChannel, Tag inTag, IRelayClient* inRelayClient)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	//
	sprintf(LogInfo, "%s( %d, %d, 0X%lx, 0X%lx)", strFunName, nDVRID, nChannel, inTag, inRelayClient);
	//
	CLog::Log(LogInfo);
}
*/

void CLog::LogFun_SIISI(const char* strFunName, int nDVRID, int nChannel, const unsigned char* outDataBuffer, INT32 inBufferLength)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	//
	sprintf(LogInfo, "%s( %d, %d, \"%s\", %ld)", strFunName, nDVRID, nChannel, outDataBuffer, inBufferLength);
	//
	CLog::Log(LogInfo);
}



void CLog::LogFun_SS(const const char* strFunName, const char* strPara)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	//
	sprintf(LogInfo, "%s( \"%s\") ", strFunName, strPara);
	//
	CLog::Log(LogInfo);
	//
}


void CLog::LogFun_SIS(const char* strFunName, const HANDLE hHandle, const char* strPara)
{
	char LogInfo[1024];
	memset(LogInfo, 0, 1024);
	
	sprintf(LogInfo, "%s( 0X%lx, \"%s\")", strFunName, hHandle, strPara);
	
	CLog::Log(LogInfo);
}


// the output length would be 22
char* CLog::getTimeStr(char* pTimeInfo)
{
	//static char LogInfo[128];
	//memset(LogInfo, 0, 128);
	SYSTEMTIME   tmNow;
	GetLocalTime(&tmNow);
	sprintf(pTimeInfo, "%2ld,%2ld:%2ld:%2ld, %3ld", tmNow.wDay, tmNow.wHour, tmNow.wMinute, tmNow.wSecond, tmNow.wMilliseconds);
	return pTimeInfo;
}
