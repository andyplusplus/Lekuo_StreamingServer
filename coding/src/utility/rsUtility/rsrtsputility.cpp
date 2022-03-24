// RsRTSPUtility.cpp: implementation of the RsRTSPUtility class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RsRTSPUtility.h"

#include <string.h>
#include <stdio.h>
//#include "RSMemroy.h"
#include "CRTDBG.H"

const char*   sEOL		= "\r\n";
const char*   sSpaces	="                /";  //16 Spaces for easy to read
const char*   RTSPHeader = "RTSP://";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RsRTSPUtility::RsRTSPUtility()
{

}

RsRTSPUtility::~RsRTSPUtility()
{

}



char* RsRTSPUtility::GeneratePlayURL(const char* inCreateRelayServerPara, const char* inCreateChannelInRelayServerPara, const char* inPlayChannelPara)
{
	char* theURL = new char[MAX_CHAR_SIZE];
	memset(theURL, 0, MAX_CHAR_SIZE*sizeof(char));
	//
	const int theParas = 3;
	const char* pPara[theParas];
	pPara[0] = inCreateRelayServerPara;
	pPara[1] = inCreateChannelInRelayServerPara;
	pPara[2] = inPlayChannelPara;

	strcpy(theURL, RTSPHeader);													//		RTSP://
	//
	for(int i = 0; i < theParas; i++)
	{
		if(pPara[i])
		{
			//
			//������������Copy����������Copy��ǰ��ȷ�ϴ�Copy�����ݲ�����RTSP�����������Buffer���
			const char* theRTSPHeadInURL = strstr(pPara[i], RTSPHeader);
			if(theRTSPHeadInURL)
			{
				theURL[0] = NULL;
			}
			//
			//
			if ( 0 == i)
			{//Ĭ�������, ServerIP=192.168.0.58�����ServerIP��ǰ��ȥ��
				if(theRTSPHeadInURL)
				{
					strcpy(theURL + strlen(theURL), pPara[0]);
				}else
				{//������RTSP��ͷ�ģ���Լ����ServerIP��ʼ
					strcpy(theURL+strlen(theURL), strchr(pPara[0], '=') +1 );	// "="����Ҫcopy���ַ�����				
				}

			}else
			{
				strcpy(theURL + strlen(theURL), pPara[i]);
			}
			if (',' != theURL[strlen(theURL)-1])
			{
				theURL[strlen(theURL)] = ',';
			};
		};
		
	}
	//
	//���RTSP������û��'/'����ѵ�һ��','�滻��"/"
	char* theRepLoc = strchr(theURL + strlen(RTSPHeader), '/');
	if (!theRepLoc)
	{
		char* theRepLoc1 = strchr(theURL, ',');
		theRepLoc1[0] = '/';
	}

	return theURL;
}

//
//
//Wheather the URL is the self defined URL
//
BOOL RsRTSPUtility::isURLSelfDefined(const char* inURL)
{
	BOOL theSelfDefinedURL = FALSE;

	char* theUpperDupURL = _strupr(_strdup(inURL));
	if (strstr(theUpperDupURL, "CLIENTTYPE")  && strstr(theUpperDupURL, "RTSP://"))
	{
		theSelfDefinedURL = TRUE;	
	}
	
	//Tobe here  delete theUpperDupURL;   //Ϊ�δ˴�ɾ���ڴ���ִ���

	return theSelfDefinedURL;
}


//�ú�����LK_KEYWORD�Ĳ�ͬ�����ز�ͬ��ֵ
//
//    ע: outData��buffer�����㹻��
//		tobe:  outDataLen��������
BOOL RsRTSPUtility::parserDataFromStr(const char* inStr, char* outData, int /*inBuffSize*/, const LK_KEYWORD inOutDataType)
{

	//
	//������ʼ�����ַ������Լ������ַ���
	//
	char theStartStr[MAX_CHAR_SIZE];
	char theEndStr[MAX_CHAR_SIZE];
	memset(theStartStr, 0, MAX_CHAR_SIZE);
	memset(theEndStr, 0, MAX_CHAR_SIZE);

	//RSCharMemroy::zeroStrMemory(theStartStr);
	//RSCharMemroy::zeroStrMemory(theEndStr);

	if (LK_KEYWORD_RelayServer_ServerIP ==inOutDataType)
	{
		char* theStartPara =  "RTSP://";
		strcpy(theStartStr,theStartPara);

		theEndStr[0] = '/';
	}
	else
	{
		const char* theStartPara = LK_KEYWORD_STR[inOutDataType];
		strcpy(theStartStr, theStartPara);
		theStartStr[strlen(theStartStr)] = '=';

		theEndStr[0] = ',';
	}

	//
	//�������ҽ������������ʼλ�úͽ���λ��
	//
	if ( !strstr(inStr, theStartStr) )
	{
		_ASSERT(!"The keyword was not founded in the URL!");
		return FALSE;	
	};
	
	const char* theStartLoc  = strstr(inStr, theStartStr) + strlen(theStartStr);
	const char* theEndLoc = strstr(theStartLoc, theEndStr);				//ָ��','��λ��

	int theStrLen = 0;
	if (NULL==theEndLoc)
	{
		theStrLen = strlen(theStartLoc);
	}else
	{
		theStrLen = theEndLoc - theStartLoc;
	}

	//
	//Copy the Parsed data to the buffer
	//
	_ASSERT( theStrLen>0 );
	//strncpy_s(outData, outDataLen, theStartLoc, theStrLen);
	strncpy(outData, theStartLoc, theStrLen);
	outData[theStrLen] = NULL;

	return TRUE;
}


