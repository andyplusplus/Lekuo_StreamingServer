// RsRTSPUtility.h: interface for the RsRTSPUtility class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSRTSPUTILITY_H__3F038499_430C_41BF_AE3A_3D9AC9BDC4E2__INCLUDED_)
#define AFX_RSRTSPUTILITY_H__3F038499_430C_41BF_AE3A_3D9AC9BDC4E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LeKuo.h"

class RsRTSPUtility  
{
public:
	RsRTSPUtility();
	virtual ~RsRTSPUtility();

public:

	//
	//General string Processor in url
	//	
	static char* GeneratePlayURL(const char* inCreateRelayServerPara, const char* inCreateChannelInRelayServerPara, const char* inPlayChannelPara);
	
	static BOOL isURLSelfDefined(const char* inURL);
	
	//Parse vale of Keyword from URL according the Datatype, TBD: Ãû×ÖÓÅ»¯
	static BOOL parserDataFromStr(const char* inURL, char* outData, int inBuffSize, const LK_KEYWORD inParseDataType);
	
};

#endif // !defined(AFX_RSRTSPUTILITY_H__3F038499_430C_41BF_AE3A_3D9AC9BDC4E2__INCLUDED_)
