// FileSave.cpp: implementation of the CFileSave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileSave.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileSave::CFileSave(const char* inFileName)
:m_file(NULL), m_bOpenFlag(FALSE), m_strSaveFileName(inFileName)
{
	
}

CFileSave::~CFileSave()
{
	
}

BOOL CFileSave::Open(const char* inFileName)
{
	if(m_bOpenFlag)
		return TRUE;

	m_bOpenFlag = m_file.Open(inFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone);

	return m_bOpenFlag;
}

BOOL CFileSave::Save(unsigned char* inBuff, unsigned long inLen)
{
	if(!m_bOpenFlag)
	{
		if(!Open(m_strSaveFileName.c_str()))
			return FALSE;
	}
	
	m_file.Write(inBuff, inLen);

	m_file.Close();

	return TRUE;
}

void CFileSave::Close()
{
	if(m_bOpenFlag)
	{
		m_file.Close();
		m_bOpenFlag = FALSE;
	}
}