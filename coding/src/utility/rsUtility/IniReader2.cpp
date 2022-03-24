// IniReader.cpp  

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Shlwapi.h"
#include "IniReader2.h"

//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

CIniReader2::CIniReader2(const char* szFileName)
{
	if(strstr(szFileName, ":")) //such as C:\windows
	{
		strcpy(m_szFileName, szFileName);
	}else
	{
		char thePath[INIREADER2_MAX_PATH]; 
		GetExePath(thePath);

		int thePathLen = strlen(thePath);	
		strcpy(m_szFileName, thePath);
		strcpy(m_szFileName + thePathLen, "\\");
		strcpy(m_szFileName + thePathLen + 1, szFileName);
	}

}
 
char* CIniReader2::GetExePath(char* ioPath)
{
	memset(ioPath, 0x00, INIREADER2_MAX_PATH);
	::GetModuleFileName(NULL, ioPath, INIREADER2_MAX_PATH);
	::PathRemoveFileSpec(ioPath);

	return ioPath;
}



int CIniReader2::ReadInteger(const char* szSection, const char* szKey, int iDefaultValue)
{
	int iResult = GetPrivateProfileInt(szSection,  szKey, iDefaultValue, m_szFileName); 
	return iResult;
}


long CIniReader2::ReadLong(const char* szSection, const char* szKey, const char* szDefaultValue)
{
	char* theStr;
	long theResult;
	theStr = ReadString(szSection,  szKey, szDefaultValue); 
	theResult = atol(theStr);
	delete []theStr;
	return theResult;
}


__int64 CIniReader2::ReadLongLong(const char* szSection, const char* szKey, const char* szDefaultValue)
{
	char* theStr;
	__int64 theResult;
	theStr = ReadString(szSection,  szKey, szDefaultValue); 
	theResult = _atoi64(theStr);
	delete []theStr;
	return theResult;
}

float CIniReader2::ReadFloat(const char* szSection, const char* szKey, float fltDefaultValue)
{
	char szResult[INIREADER2_MAX_PATH];
	char szDefault[INIREADER2_MAX_PATH];
	float fltResult;
	sprintf(szDefault, "%f",fltDefaultValue);
	GetPrivateProfileString(szSection,  szKey, szDefault, szResult, INIREADER2_MAX_PATH, m_szFileName); 
	fltResult =  (float) atof(szResult);
	return fltResult;
}

bool CIniReader2::ReadBoolean(const char* szSection, const char* szKey, bool bolDefaultValue)
{
	char szResult[INIREADER2_MAX_PATH];
	char szDefault[INIREADER2_MAX_PATH];
	bool bolResult;
	sprintf(szDefault, "%s", bolDefaultValue? "True" : "False");
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, INIREADER2_MAX_PATH, m_szFileName); 
	bolResult =  (strcmp(szResult, "True") == 0 || strcmp(szResult, "true") == 0) ? true : false;
	return bolResult;
}

// char* CIniReader2::ReadString(char* szSection, char* szKey, const char* szDefaultValue)
// {
// 	char* szResult = new char[INIREADER2_MAX_PATH];
// 	memset(szResult, 0x00, INIREADER2_MAX_PATH);
// 	GetPrivateProfileString(szSection,  szKey, szDefaultValue, szResult, INIREADER2_MAX_PATH, m_szFileName); 
// 	return szResult;
// }


char* CIniReader2::ReadString(const char* szSection, const char* szKey, const char* szDefaultValue, char* ioszValue)
{
	if(ioszValue == NULL)
	{
		char* szResult = new char[INIREADER2_MAX_PATH];
		memset(szResult, 0x00, INIREADER2_MAX_PATH);
		GetPrivateProfileString(szSection,  szKey, szDefaultValue, szResult, INIREADER2_MAX_PATH, m_szFileName); 
 		return szResult;
	}
	else
	{
		GetPrivateProfileString(szSection,  szKey, szDefaultValue, ioszValue, INIREADER2_MAX_PATH, m_szFileName); 
		return (char*)szDefaultValue;
	}

}


char* CIniReader2::ReadPath(const char* szSection, const char* szKey, const char* szDefaultValue, char* ioszValue)
{
	if (!ioszValue)
	{
		ioszValue = new char[INIREADER2_MAX_PATH];
	}
	ReadString(szSection, szKey, szDefaultValue, ioszValue);
	if ('.' == ioszValue[0] )
	{
		char theResultPath[INIREADER2_MAX_PATH];

		char thePath[INIREADER2_MAX_PATH];
		GetExePath(thePath);
		int thePathLen = strlen(thePath);

		strcpy(theResultPath, thePath);
		strcpy(theResultPath + thePathLen, ioszValue+1);


		strcpy(ioszValue, theResultPath);

		if( !PathFileExists(m_szFileName) )
		{
			//_assert(!"The File was not founded");
		}

		strcpy(ioszValue, theResultPath);
	}
	return ioszValue;
}


char* CIniReader2::ReadPathName(const char* szSection, const char* szKey, const char* szDefaultValue, char* ioszValue)
{
	return ReadPath(szSection, szKey, szDefaultValue, ioszValue);
}


void CIniReader2::WriteInteger(const char* szSection, const char* szKey, int inValue)
{
	char theBuffer[20];
	_itoa(inValue, theBuffer, 10);
	WriteString(szSection, szKey, theBuffer);
}


void CIniReader2::WriteString(const char* szSection, const char* szKey, const char* inStr)
{
	::WritePrivateProfileString (szSection, szKey, inStr, m_szFileName); 
	return;
}