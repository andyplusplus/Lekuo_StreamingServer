// DVRFIle.cpp: implementation of the CDVRFIle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <cstdio>
#include "DVRFile.h"
#include "IniReader2.h"
//#include "PathParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDVRFile::CDVRFile()
{

	m_shSaveFile = NULL;
}

CDVRFile::~CDVRFile()
{

}


//////////////////////////////////////////////////////////////////////////
// Save File function
//////////////////////////////////////////////////////////////////////////


BOOL CDVRFile::saveToFile(char* lpszServerIP, int nChannel, unsigned char* ucData, unsigned long ulDataLen)
{
	//Generate file name
	SYSTEMTIME systemTime;
	::GetLocalTime(&systemTime);
	
	char szSaveFileName[MAX_CHAR_SIZE];

	//ServerIPÊÇ·ñÎª¿Õ
	if(!*lpszServerIP)
	{		
		sprintf(szSaveFileName, "Channel%02d-%4d.%02d.%02d-%02d.%02d.%02d.%03d", nChannel, systemTime.wYear, 
			systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
	}
	else
	{
	char szServerIP[MAX_CHAR_SIZE];
	strcpy(szServerIP, lpszServerIP);

	sprintf(szSaveFileName, "IP%s-Channel%02d-%4d.%02d.%02d-%02d.%02d.%02d.%03d", 
											   szServerIP, nChannel, systemTime.wYear, 
											   systemTime.wMonth, systemTime.wDay, 
											   systemTime.wHour, systemTime.wMinute, 
											   systemTime.wSecond, systemTime.wMilliseconds);
	}

	//Get Save Path from config file
	char* pSaveFilePath = CIniReader::GetIniReader(CONFIGFILENAME)->ReadPath("ClientCommon_SaveFile", "SaveFilePath", " ");

	char szSaveFilePath[MAX_CHAR_SIZE];
	sprintf(szSaveFilePath, "%s%s.hik", pSaveFilePath, szSaveFileName);

	//Create File
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle =  TRUE;

	if(!m_shSaveFile)
	m_shSaveFile = ::CreateFile(szSaveFilePath, 
		GENERIC_WRITE, FILE_SHARE_WRITE, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	::WriteFile(m_shSaveFile, ucData, ulDataLen, &ulDataLen, NULL);
		
	return TRUE;	
}


BOOL CDVRFile::closeSaveFile()
{
	if(m_shSaveFile)
	{
		CloseHandle(m_shSaveFile);
		m_shSaveFile = NULL;
	}

	return TRUE;
}

