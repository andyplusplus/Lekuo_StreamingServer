// IVFile.h: interface for the IVFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVFILE_H__96C87889_7ACF_4B82_9835_5C31DB1A9711__INCLUDED_)
#define AFX_IVFILE_H__96C87889_7ACF_4B82_9835_5C31DB1A9711__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class IVFile
{
public:

	struct ST_RSNextData
	{
		unsigned char* pData;
		unsigned long DataLen;
		unsigned long TimeOffSet;
		int           isPlayAgain;
	};

public:

	virtual ~IVFile() {};

	virtual BOOL openFile(const char* inFileName)=0;
	virtual BOOL closeFile()=0;

	virtual unsigned long	getMetadataLen() = 0;
	virtual unsigned long	getMetadata(unsigned char* outDataBuffer, unsigned long inBufferLength) = 0;

	virtual unsigned long	getNextData(unsigned long &ioDataPos, unsigned char* ioData, unsigned long inDataLen, unsigned long& outDataType, BOOL inReplay) 
	{
		ioDataPos; ioData; inDataLen; outDataType; inReplay;					//to remove warning
		return 0;
	};

	virtual IVFile::ST_RSNextData	getNextData(unsigned long &ioFromDataPos, BOOL inLoopRead)  
		{
			ST_RSNextData theData;
			theData.DataLen = 0;		theData; inLoopRead; ioFromDataPos;		//to remove warning
			return theData;
		};

	virtual unsigned long	getFileSize()=0;



};


typedef IVFile* LPIVFile;

#endif // !defined(AFX_IVFILE_H__96C87889_7ACF_4B82_9835_5C31DB1A9711__INCLUDED_)
