// FileSave.h: interface for the CFileSave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILESAVE_H__7C6E9023_7969_4E94_912A_77F4FEAC6E0B__INCLUDED_)
#define AFX_FILESAVE_H__7C6E9023_7969_4E94_912A_77F4FEAC6E0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class CFileSave  
{
public:
	CFileSave(const char* inFileName);
	virtual ~CFileSave();

	BOOL Save(unsigned char* inBuff, unsigned long inLen);

private:
	CFile m_file;
	BOOL m_bOpenFlag;
	
	std::string m_strSaveFileName;

	BOOL Open(const char* inFileName);
	void Close();

};

#endif // !defined(AFX_FILESAVE_H__7C6E9023_7969_4E94_912A_77F4FEAC6E0B__INCLUDED_)

