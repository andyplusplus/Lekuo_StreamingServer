// IniReader.cpp  

#include <Windows.h>
//#include "SafeStdLib.h"

#include "stdafx.h"
#include <stdio.h>
#include "Shlwapi.h"
//#include "stdlib.h"
//#include "OSHeaders.h"
#include "IniReader.h"

CIniReader* CIniReader::s_IniReader = NULL;
BOOL  CIniReader::s_FileNameSeted = FALSE;
BOOL  CIniReader::s_PathNameGeted = FALSE;
char  CIniReader::s_szPathName[INIREADER_MAX_PATH];
char  CIniReader::s_szFileName[INIREADER_MAX_PATH];

CIniReader* CIniReader::GetIniReader()
{
	if (s_IniReader == NULL)
	{
		//ASSERT(s_FileNameSeted);
		s_IniReader = new CIniReader();
	}
	return s_IniReader;
}

CIniReader* CIniReader::GetIniReader(char* szFileName)
{
	SetIniFileName(szFileName);
	if (s_IniReader == NULL)
	{
		s_IniReader = new CIniReader();
	}
	return s_IniReader;
}


char* CIniReader::GetExePath()
{
	if (!s_PathNameGeted)
	{
		memset(s_szPathName, 0x00, INIREADER_MAX_PATH);
		::GetModuleFileName(NULL, s_szPathName, INIREADER_MAX_PATH);
		PathRemoveFileSpec(s_szPathName);
	}
	return s_szPathName;
}

void CIniReader::SetIniFileName(char* szFileName)
{
	////ASSERT(!s_FileNameSeted);
	//qtss_sprintf(s_szFileName, "%s\\%s", GetExePath(), szFileName);
	sprintf(s_szFileName, "%s\\%s", GetExePath(), szFileName);

    if( !PathFileExists(s_szFileName) )
	{
        //ASSERT(FALSE);
    }
	//qtss_printf(s_szFileName);
	//
	s_FileNameSeted = TRUE;
	//
}



//Start __Andy__
CIniReader::CIniReader()
{
}
//End __Andy__

//CIniReader::CIniReader(char* szFileName)
//{
//	memset(szModulePath, 0x00, INIREADER_MAX_PATH);
//	memcpy(s_szFileName, szFileName, strlen(szFileName));
//}

int CIniReader::ReadInteger(char* szSection, char* szKey, int iDefaultValue)
{
	int iResult = GetPrivateProfileInt(szSection,  szKey, iDefaultValue, s_szFileName); 
	return iResult;
}


long CIniReader::ReadLong(char* szSection, char* szKey, const char* szDefaultValue )
{
	char* theStr;
	long theResult;
	theStr = ReadString(szSection,  szKey, szDefaultValue); 
	theResult = atol(theStr);
	delete []theStr;
	return theResult;
}


//SInt64 CIniReader::ReadLongLong(char* szSection, char* szKey, const char* szDefaultValue)
//{
//	char* theStr;
//	SInt64 theResult;
//	theStr = ReadString(szSection,  szKey, szDefaultValue); 
//	theResult = _atoi64(theStr);
//	delete []theStr;
//	return theResult;
//}

float CIniReader::ReadFloat(char* szSection, char* szKey, float fltDefaultValue)
{
	char szResult[INIREADER_MAX_PATH];
	char szDefault[INIREADER_MAX_PATH];
	float fltResult;
	sprintf(szDefault, "%f",fltDefaultValue);
	GetPrivateProfileString(szSection,  szKey, szDefault, szResult, INIREADER_MAX_PATH, s_szFileName); 
	fltResult =  (float) atof(szResult);
	return fltResult;
}

bool CIniReader::ReadBoolean(char* szSection, char* szKey, bool bolDefaultValue)
{
	char szResult[INIREADER_MAX_PATH];
	char szDefault[INIREADER_MAX_PATH];
	bool bolResult;
	sprintf(szDefault, "%s", bolDefaultValue? "True" : "False");
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, INIREADER_MAX_PATH, s_szFileName); 
	bolResult =  (strcmp(szResult, "True") == 0 || strcmp(szResult, "true") == 0) ? true : false;
	return bolResult;
}

// char* CIniReader::ReadString(char* szSection, char* szKey, const char* szDefaultValue)
// {
// 	char* szResult = new char[INIREADER_MAX_PATH];
// 	memset(szResult, 0x00, INIREADER_MAX_PATH);
// 	GetPrivateProfileString(szSection,  szKey, szDefaultValue, szResult, INIREADER_MAX_PATH, s_szFileName); 
// 	return szResult;
// }


char* CIniReader::ReadString(char* szSection, char* szKey, const char* szDefaultValue, char* outszValue)
{
	if(outszValue == NULL)
	{
		char* szResult = new char[INIREADER_MAX_PATH];
		memset(szResult, 0x00, INIREADER_MAX_PATH);
		GetPrivateProfileString(szSection,  szKey, szDefaultValue, szResult, INIREADER_MAX_PATH, s_szFileName); 
 		return szResult;
	}
	else
	{
		GetPrivateProfileString(szSection,  szKey, szDefaultValue, outszValue, INIREADER_MAX_PATH, s_szFileName); 
		return NULL;
	}

}


char* CIniReader::ReadPath(char* szSection, char* szKey, const char* szDefaultValue, char* ioszValue)
{
	char* theRawPath = ReadString(szSection, szKey, szDefaultValue, ioszValue);
	if ('.' == theRawPath[0] )
	{
		char theResultPath[INIREADER_MAX_PATH];
		char* thePath = GetExePath();
		int thePathLen = strlen(thePath);

		strcpy(theResultPath, thePath);
		strcpy(theResultPath + thePathLen, theRawPath+1);


		strcpy(ioszValue, theResultPath);

		if( !PathFileExists(s_szFileName) )
		{
			//assert(!"The File was not founded");
		}
	}
	return theRawPath;
}


char* CIniReader::ReadPathName(char* szSection, char* szKey, const char* szDefaultValue, char* ioszValue)
{
	return ReadPath(szSection, szKey, szDefaultValue, ioszValue);
}


void CIniReader::WriteInteger(char* szSection, char* szKey, int inValue)
{
	char theBuffer[20];
	_itoa(inValue, theBuffer, 10);
	WriteString(szSection, szKey, theBuffer);
}


void CIniReader::WriteString(char* szSection, char* szKey, char* inStr)
{
	::WritePrivateProfileString (szSection, szKey, inStr, s_szFileName); 
	return;
}