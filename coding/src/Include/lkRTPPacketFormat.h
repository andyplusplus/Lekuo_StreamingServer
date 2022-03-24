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

//RS服务器发送数据包的大小，总的数据包大小 = RS_RTP_HEAD_SIZE + RS_RTP_TAG_SIZE + MAX_RS_PAYLOAD_SIZE
const unsigned int RS_RTP_HEAD_SIZE = 12;			//前具体见RTP协议，4 Byte + 4 Byte timestamp + 4 Byte SSRC, SSRC用来表示是否是数据头
const unsigned int RS_RTP_HEAD_DEV_TYPE_BYTE = 8;	//第9字节表示设备类型			Not Used
const unsigned int RS_RTP_HEAD_DATA_TYPE_BYTE = 10;	//第11字节表示数据类型: 

const unsigned int RS_RTP_TAG_SIZE = sizeof(Tag);	//32 for the tag
const unsigned int RS_RTP_TAG_HEAD_SIZE  = (RS_RTP_HEAD_SIZE + RS_RTP_TAG_SIZE);
const unsigned int RS_MAX_PACKET_SIZE = 4096;	//数据头以及数据体之和不能超过该值
const unsigned int RS_MAX_PAYLOAD_SIZE = 1024;	//单个数据包传输的最大负载的大小，该负载要必须小于4096字节，可以放在配置文件中
const unsigned int RS_MIN_PAYLOAD_SIZE = 16;	//如果长度小于该值，则合并数据与再发
const unsigned int RS_RTP_TOTAL_SIZE =(RS_RTP_HEAD_SIZE + RS_RTP_TAG_SIZE + RS_MAX_PAYLOAD_SIZE);

#define DATA_TYPES_DEF unsigned char
#define LP_DATA_TYPES_DEF unsigned char*


#endif