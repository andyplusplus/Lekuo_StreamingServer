//  #include "lkRTPPacketFormat.h"


#ifndef __INCLUDE_LKTRPPACKETFORMAT_H_
#define __INCLUDE_LKTRPPACKETFORMAT_H_

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

//RS�������������ݰ��Ĵ�С���ܵ����ݰ���С = RS_RTP_HEAD_SIZE + RS_RTP_TAG_SIZE + MAX_RS_PAYLOAD_SIZE
const unsigned int RS_RTP_HEAD_SIZE = 12;			//ǰ�����RTPЭ�飬4 Byte + 4 Byte timestamp + 4 Byte SSRC, SSRC������ʾ�Ƿ�������ͷ
const unsigned int RS_RTP_HEAD_DEV_TYPE_BYTE = 8;	//��9�ֽڱ�ʾ�豸����			Not Used
const unsigned int RS_RTP_HEAD_DATA_TYPE_BYTE = 10;	//��11�ֽڱ�ʾ��������: 

const unsigned int RS_RTP_TAG_SIZE = sizeof(Tag);	//32 for the tag
const unsigned int RS_RTP_TAG_HEAD_SIZE  = (RS_RTP_HEAD_SIZE + RS_RTP_TAG_SIZE);
const unsigned int RS_MAX_PACKET_SIZE = 4096;	//����ͷ�Լ�������֮�Ͳ��ܳ�����ֵ
const unsigned int RS_MAX_PAYLOAD_SIZE = 1024;	//�������ݰ����������صĴ�С���ø���Ҫ����С��4096�ֽڣ����Է��������ļ���
const unsigned int RS_MIN_PAYLOAD_SIZE = 16;	//�������С�ڸ�ֵ����ϲ��������ٷ�
const unsigned int RS_RTP_TOTAL_SIZE =(RS_RTP_HEAD_SIZE + RS_RTP_TAG_SIZE + RS_MAX_PAYLOAD_SIZE);

#define DATA_TYPES_DEF unsigned char
#define LP_DATA_TYPES_DEF unsigned char*


#endif