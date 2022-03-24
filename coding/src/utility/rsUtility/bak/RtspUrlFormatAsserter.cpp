// RtspUrlFormatAsserter.cpp: implementation of the CRtspUrlFormatAsserter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RtspUrlFormatAsserter.h"

#pragma   warning   (disable : 4786)

#include <vector>
#include <string>
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



struct ST_Format_Keys_Pair
{
	EN_RTSP_CMD_FMT CmdFormat;
	BYTE NumberOfKeyRequired;
	LK_KEYWORD KeyWords[LK_KEYWORD_Max];
};

const int Format_Keys_Pair_Array_Len = 8;
const ST_Format_Keys_Pair Format_Keys_Pair_Array[] = 
{
	{EN_RTSP_CMD_FMT_InstanceStr,				0,	{	LK_KEYWORD_Instance_Cmd}	},
		
	{EN_RTSP_CMD_FMT_CreateRelayServer,			2,	{	LK_KEYWORD_RelayServer_ServerIP,		LK_KEYWORD_RelayServer_ClientType}	},
	
	{EN_RTSP_CMD_FMT_CreateDVRRelayServer,	5,	{	LK_KEYWORD_Device_IP,			LK_KEYWORD_Device_DataType, LK_KEYWORD_Device_Port,   
	LK_KEYWORD_Device_UserName,		LK_KEYWORD_Device_Password}	},
	
	{EN_RTSP_CMD_FMT_CreateChanInDevice,		1,	{	LK_KEYWORD_Channel_ID}	},
	
	{EN_RTSP_CMD_FMT_CreateChanInServer,		6,	{	LK_KEYWORD_Device_IP,			LK_KEYWORD_Device_DataType, LK_KEYWORD_Device_Port
	,   LK_KEYWORD_Device_UserName,		LK_KEYWORD_Device_Password,	LK_KEYWORD_Channel_ID}	},
	
	{EN_RTSP_CMD_FMT_PlayChannel,				1,	{ PlayChanCmd }	},
	{EN_RTSP_CMD_FMT_StopChannel,				1,	{ StopChanCmd }	},
	{EN_RTSP_CMD_FMT_SendChannelCmds,			1,	{ SendChanCmd }	}
};


CRtspUrlFormatAsserter::CRtspUrlFormatAsserter()
{
	
}

CRtspUrlFormatAsserter::~CRtspUrlFormatAsserter()
{

}


LK_Error CRtspUrlFormatAsserter::AssertRtspUrlCmdFormat(const char* strURL, EN_RTSP_CMD_FMT inFormat)
{
	//Get the ArrayIndex
	/*
	for (int theFormatIndex = 0; theFormatIndex < Format_Keys_Pair_Array_Len; ++theFormatIndex)
	{
		if(inFormat == Format_Keys_Pair_Array[theFormatIndex].CmdFormat)
		{
			AssertRtspUrlKeysIsExist(strURL, (LK_KEYWORD*) Format_Keys_Pair_Array[theFormatIndex].KeyWords, Format_Keys_Pair_Array[theFormatIndex].NumberOfKeyRequired);
		}
	}
	*/
	//Return the Result
	return LK_NoErr;
}


LK_Error CRtspUrlFormatAsserter::AssertRtspUrlKeysIsExist(const char* strURL, LK_KEYWORD* inKeys, int inKeyNumbers)
{
	/*
	for(int iKeyIndex = 0; iKeyIndex < inKeyNumbers; ++iKeyIndex)
	{
		LK_Error rsdError =  AssertRtspUrlKeyIsExist(strURL, inKeys[iKeyIndex]);
		
		if( LK_NoErr != rsdError)
			return rsdError;
	}
	*/
	return LK_NoErr;	
}


LK_Error CRtspUrlFormatAsserter::AssertRtspUrlKeyIsExist(const char* instrURL, LK_KEYWORD inKey)
{
	/*
	std::string strOriginalURL(instrURL);
	std::string::iterator strIter = std::find(strOriginalURL.begin(), strOriginalURL.end(), '/');
	if(strIter!= strOriginalURL.end())
	{
		std::string::iterator temp;
		
		do 
		{
			temp = strIter;
			strIter = std::find(strIter + 1, strOriginalURL.end(), '/');
		} while(strIter != strOriginalURL.end());
		
		strOriginalURL.erase(strOriginalURL.begin(), temp + 1);
	}
	
	
	//Step1 separate the URL into info token
	char sep[] = " ;";
	char strURL[MAX_CHAR_SIZE];
	
	strcpy(strURL, strOriginalURL.c_str());
	char* strtoken = strtok(strURL, sep);
	
	//Data
	std::vector<std::string> vtData;
	
	std::vector<std::string> vtSection;
	std::vector<std::string> vtValue;
	
	while(strtoken != NULL)
	{
		vtData.push_back(std::string(strtoken));
		
		strtoken = strtok( NULL, sep );
	}
	
	for(int i = 0; i < vtData.size(); ++i)
	{
		char subsep[]=" =";
		char strtemptoken[MAX_CHAR_SIZE];
		strcpy(strtemptoken, vtData[i].c_str());
		
		char* strsubtoken = strtok(strtemptoken, subsep);
		if(strsubtoken!= NULL)
		{
			vtSection.push_back(std::string(strsubtoken));
			strsubtoken = strtok(NULL, subsep);
			if(strsubtoken != NULL)
				vtValue.push_back(std::string(strsubtoken));
		}
	}
	
	std::vector<std::string>::iterator it = std::find(vtSection.begin(), vtSection.end(), LK_KEYWORD_STR[inKey]);
	if(it == vtSection.end())
	{
		return (inKey + RTSP_KEYWORD_MESSAGE_BASE);
	}
	*/
	return LK_NoErr;
}




