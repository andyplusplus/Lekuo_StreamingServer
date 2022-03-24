// RsRTPUtility.cpp: implementation of the RsRTPUtility class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lkRTPPacketFormat.h"
#include "RsRTPUtility.h"
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1

RsRTPUtility::RsRTPUtility()
{

}

RsRTPUtility::~RsRTPUtility()
{

}


//
//Date Type related f	unctions
//
void				RsRTPUtility::setDataType(BYTE* inBuff, unsigned long /* inLen */, DATA_TYPES_DEF inEnDataType)
{
	memcpy(inBuff + RS_RTP_HEAD_DATA_TYPE_BYTE, (LP_DATA_TYPES_DEF)&inEnDataType, sizeof(DATA_TYPES_DEF) );	
}


DATA_TYPES_DEF		RsRTPUtility::getDataType(BYTE* inBuff, unsigned long /* inLen */)
{
	DATA_TYPES_DEF theType;
	theType = * ( (LP_DATA_TYPES_DEF) (inBuff + RS_RTP_HEAD_DATA_TYPE_BYTE) );
	return theType;
}

//
//Seq related functions
//
void			RsRTPUtility::setSeqNumber(BYTE* inBuff, unsigned long /* inLen */, unsigned short inSeqNumber)
{
	memcpy(inBuff + 2, &inSeqNumber, sizeof(unsigned short));
}


unsigned short	RsRTPUtility::getSeqNumber(BYTE* inBuff, unsigned long /* inLen */)
{
	unsigned short theSeqNumber = *	(		(unsigned short*)(inBuff + 2)		);
	return theSeqNumber;
}

//
//
void			RsRTPUtility::printRTPInfo(BYTE* inBuff, unsigned long /*  */inLen)
{
	DATA_TYPES_DEF theDataType;
	unsigned short theSeqNumber;
	//
	theDataType		=	RsRTPUtility::getDataType(inBuff, inLen);
	theSeqNumber	=	RsRTPUtility::getSeqNumber(inBuff, inLen);
	unsigned long theDataLen	= 	inLen - RS_RTP_TAG_HEAD_SIZE;
	//
	char theStrBuffer[1024];
	memset(theStrBuffer, 0, 1024 * sizeof(char));
	sprintf(theStrBuffer, "       DataType = %ld, SeqNumber = 0x%lX, DataLen = %ld\n", theDataType, theSeqNumber, theDataLen);
	//
	OutputDebugString(theStrBuffer);
}
 