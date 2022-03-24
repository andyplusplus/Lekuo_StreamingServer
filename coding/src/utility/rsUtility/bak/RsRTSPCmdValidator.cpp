// RsRTSPCmdValidator.cpp: implementation of the RsRTSPCmdValidator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RsRTSPCmdValidator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//RsRTSPCmdValidator::RsRTSPCmdValidator()
//{
//
//}

RsRTSPCmdValidator::RsRTSPCmdValidator(const char* inRTSPCmd, const enum EN_RTSP_CMD_FMT inFmt)
{
	//ASSERT(inRTSPCmd && "The RTSP Cmd should NOT be NULL!");
	memset(m_ErrorMsg, 0, MAX_CHAR_SIZE*sizeof(char));
	m_RTSPCmd = inRTSPCmd;
	m_enFmt = inFmt;
	clearFindedKeywordsArray();
	//
}

RsRTSPCmdValidator::~RsRTSPCmdValidator()
{

}

LK_Error RsRTSPCmdValidator::getValudateResult(char* outResultDesp)
{
	//ASSERT(outResultDesp && "RsRTSPCmdValidator::getValudateResult_The Parameter should NOT be NULL!");
	
	return LK_NoErr;
}





void RsRTSPCmdValidator::clearFindedKeywordsArray()
{
	for (int i = 0; i<RTSP_KEYWORD_ArrLen; i++)
	{
		memset(m_FindedKeyWords[i], 0, LK_KEYWORD_MaxLen*sizeof(char));
	}
	return;
}

//Validate Something like "DeviceIP=%s,DeviceType=%s,UserName=%s,Password=%s,DevicePort=%ld,ChanID=%d,"
void RsRTSPCmdValidator::findKeywordsFromRTSPCmd()
{
	int thePosAfterComma = 0;
	int thePosInEqual = 0;
	// strstr(inStr, theStartStr)
}
