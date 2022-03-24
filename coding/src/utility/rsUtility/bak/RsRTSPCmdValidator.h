// RsRTSPCmdValidator.h: interface for the RsRTSPCmdValidator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSRTSPCMDVALIDATOR_H__30967041_2BD6_40BF_ACE7_E39A8F650158__INCLUDED_)
#define AFX_RSRTSPCMDVALIDATOR_H__30967041_2BD6_40BF_ACE7_E39A8F650158__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LeKuo.h"

class RsRTSPCmdValidator  
{
public:
	RsRTSPCmdValidator(const char* inRTSPCmd, const enum EN_RTSP_CMD_FMT inFmt);
	LK_Error getValudateResult(char* outResultDesp);
	virtual ~RsRTSPCmdValidator();
	//
private:
	void clearFindedKeywordsArray();
	void findKeywordsFromRTSPCmd();
	//
	char m_ErrorMsg[MAX_CHAR_SIZE];
	char m_FindedKeyWords[RTSP_KEYWORD_ArrLen][LK_KEYWORD_MaxLen];
	const char* m_RTSPCmd;
	EN_RTSP_CMD_FMT m_enFmt;
};

#endif // !defined(AFX_RSRTSPCMDVALIDATOR_H__30967041_2BD6_40BF_ACE7_E39A8F650158__INCLUDED_)
