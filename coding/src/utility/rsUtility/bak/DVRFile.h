// DVRFIle.h: interface for the CDVRFIle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DVRFILE_H__767FADB3_AABA_4893_8741_08D311E2FE31__INCLUDED_)
#define AFX_DVRFILE_H__767FADB3_AABA_4893_8741_08D311E2FE31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LeKuo.h"


//Filler is the filler in filename
//Seg    is the separation in filename

const char FILLER = '_';
const char SEG = '-';

static char CONFIGFILENAME[] = "VideoRelay.ini";

enum DVRFILETYPE
{
	DVRFILETYPE_HikVideo,
	DVRFILETYPE_RelayServer
};

class CDVRFile
{
public:
	CDVRFile();
	virtual ~CDVRFile();
	
	BOOL saveToFile(char* lpszServerIP, int nChannel, unsigned char* ucData, unsigned long ulDataLen);
	BOOL closeSaveFile();

private:
	HANDLE m_shSaveFile;

};

#endif // !defined(AFX_DVRFILE_H__767FADB3_AABA_4893_8741_08D311E2FE31__INCLUDED_)
