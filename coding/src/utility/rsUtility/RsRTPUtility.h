// #include "RsRTPUtility.h"


#if !defined(AFX_RsRTPUtility_H__A5DE0BBE_0A31_4887_BBC6_8DC3F8CFCDC0__INCLUDED_)
#define AFX_RsRTPUtility_H__A5DE0BBE_0A31_4887_BBC6_8DC3F8CFCDC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LeKuo.h"

/*
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|V=2|P|X|  CC   |M|     PT      |       sequence number         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           timestamp                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           synchronization source (SSRC) identifier            |         0     |     1      |    DATA_TYPES_NET_DVR_SYSHEAD    |   4
+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
|            contributing source (CSRC) identifiers             |
|                             ....                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#include "lkRTPPacketFormat.h"

class RsRTPUtility  
{
public:
	RsRTPUtility();
	virtual ~RsRTPUtility();

public:
	static void				setDataType(BYTE* inBuff, unsigned long inLen, DATA_TYPES_DEF inEnDataType);
	static DATA_TYPES_DEF	getDataType(BYTE* inBuff, unsigned long inLen);

	static void				setSeqNumber(BYTE* inBuff, unsigned long inLen, unsigned short inSeqNumber);
	static unsigned short	getSeqNumber(BYTE* inBuff, unsigned long inLen);

	static void				printRTPInfo(BYTE* inBuff, unsigned long inLen);
};

#endif // !defined(AFX_RsRTPUtility_H__A5DE0BBE_0A31_4887_BBC6_8DC3F8CFCDC0__INCLUDED_)
