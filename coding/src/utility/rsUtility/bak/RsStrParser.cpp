// LkStrParser.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#pragma   warning   (disable : 4786)
#include <vector>
#include <string>
#include <algorithm>


#include "LkStrParser.h"


CLkStrParser::CLkStrParser()
{

}


CLkStrParser::~CLkStrParser()
{

}


LK_Error CLkStrParser::ValidateRTSPStr(char* strURL, LK_KEYWORD* inOutDataType)
{
	for(int i = 0; i < LK_KEYWORD_MaxLen; ++i)
	{
		LK_Error rsdError =  ValidateRTSPURL(strURL, inOutDataType[i]);
		
		if( LK_NoErr != rsdError)
			return rsdError;
	}

	return LK_NoErr;
}



LK_Error CLkStrParser::ValidateRTSPURL(char* strURL, LK_KEYWORD inOutDataType)
{
	
	char outstrData[MAX_CHAR_SIZE];

	ParseDataFromURL(strURL, outstrData, inOutDataType);
	if(0 != strcmp(outstrData, LK_KEYWORD_STR[inOutDataType]))	{
		return (inOutDataType + RTSP_KEYWORD_MESSAGE_BASE);
	}

	
	return LK_NoErr;
}


void CLkStrParser::ParseDataFromURL(char* instrURL, char* outstrData, LK_KEYWORD inOutDataType)
{	
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

	for(unsigned int i = 0; i < vtData.size(); ++i)
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
		
	std::vector<std::string>::iterator it = std::find(vtSection.begin(), vtSection.end(), LK_KEYWORD_STR[inOutDataType]);
	if(it != vtSection.end())
	{
			strcpy(outstrData, (*it).c_str());
	}
	else
	{
		strcpy(outstrData, "");
	}

}
