/* LeKuo.h This file defines the common data type for the whole system
����Լ��:
   1. Ϊ�˱����������󲿷ֱ����Լ�����������lk��LK��ʼ��LK��ʾLeKuo�ļ�ƣ����б�������lk��ʼ������������LK��ʼ
   2. ��������Сд��ĸ��ʼ�������Դ�д��ĸ��ʼ
   3. ö��������EN_��ʼ, �ַ�������STR_��ʼ
   3. 
*/
#ifndef __INCLUDE_LEKUO_H_
#define __INCLUDE_LEKUO_H_

#include "Msg.h"

	/* -------------------------------------------------------------------------------------------------
	��������������
	---------------------------------------------------------------------------------------------------*/

	typedef __int64			Tag;
	const Tag Tag_NULL = 0;				//NULL value of Tag
	typedef signed long		SInt32;
	typedef __int64			SInt64;


	/* -------------------------------------------------------------------------------------------------
	Ԥ����Ĵ������ͣ�������ӿڻ᷵������ΪLK_Errorһ������ֵ��������Ҫͨ������ֵ˵����������
	---------------------------------------------------------------------------------------------------*/
	enum
	{
		LK_NoErr					= 0,

		//General Errors        101 ~

		//DVR Adaptor Errors	201 ~
		LK_Init_Failed				= -201,
		LK_Login_Failed,
		LK_Logout_Failed,
		LK_StopChan_Failed,

		//Player Adaptor Errors  301 ~
		LK_Player_Opened_Failed		= -301,		//������û�д򿪣�����ʼ������Ƶ���򲥷�����ʧ��

		//Relay Server Errors    401 ~

		//User-Defined Error     501 ~
		LK_Adaptor_not_Initiated	= -501,		//����Adaptor֮ǰ��Ҫ����Initialize�����򷵻�LK_Adaptor_not_Initiated
		LK_Device_Already_Login,				//Loginĳһ�豸�Ժ�����ٴ�Login���򷵻�LK_Already_Logged_In

		//Utility Errors         601 ~
		LK_Token_not_exist			= -601,		//TokenParser::getToken����Token���������ʧ�ܣ��ɷ��ظô���
	};

	typedef long  LK_Error;			//Error type 


	/* -------------------------------------------------------------------------------------------------
	����ʹ�õ�Ԥ���峣��
	---------------------------------------------------------------------------------------------------*/

	const unsigned int MAX_CHAR_SIZE = 1024;			//����ʹ�õ�����ַ�������
	const unsigned int NORM_CHAR_SIZE = 128;			//����ʹ�õ�һ���ַ����鳤��

	enum DATA_TYPES										//��������˵��
	{
		DATA_TYPES_NET_DVR_SYSHEAD = 1,					//����ͷ����Щ�豸��������Hikvision������ͷ�������豸�Ժ����Ȼ��һ�����ȵ�����ͷ�����������Ƶ����
		DATA_TYPES_NET_DVR_STREAMDATA = 2,				//��Ƶ����
	};


	/* -------------------------------------------------------------------------------------------------
	API�ӿڵ������
	����API���ַ������� "ServerIP=192.168.0.1;UserName=admin;Password=12345;"  �ĸ�ʽ
	---------------------------------------------------------------------------------------------------*/

	const int LK_KEYWORD_MaxLen = 64;

	static const char* TOKEN_Sep = ";";

	enum LK_KEYWORD
	{
		LK_KEYWORD_Min						= 0,
		LK_KEYWORD_RelayServer_ServerIP		= 0, 
		LK_KEYWORD_RelayServer_ClientType 	= 1, 
		LK_KEYWORD_RelayServer_PingServer	= 2,
		LK_KEYWORD_RelayServer_Ext 	= 3,

		LK_KEYWORD_DEVICE_FACTORY	= 9,
		LK_KEYWORD_Device_IP		= 10, 
		LK_KEYWORD_Device_DataType	= 11, 
		LK_KEYWORD_Device_Type		= 12,
		LK_KEYWORD_Device_UserName	= 13, 
		LK_KEYWORD_Device_Password	= 14, 
		LK_KEYWORD_Device_Port		= 15, 
		LK_KEYWORD_Device_Ext		= 16, 
		LK_KEYWORD_Device_isVirtual	= 17,

		//Create Channel Related
		LK_KEYWORD_Channel_ID	= 20, 
		LK_KEYWORD_Channel_Name	= 21, 
		LK_KEYWORD_Channel_Ext	= 22, 	

		LK_KEYWORD_PlayTimes	= 26,
		LK_KEYWORD_PlayerType	= 27,
		LK_KEYWORD_Max,
	};


	const char LK_KEYWORD_STR[LK_KEYWORD_Max][LK_KEYWORD_MaxLen] =
	{
		//Server Related Para
		"ServerIP",		//0		CreateRelayServer
		"ClientType",	//1		
		"PingServer",	//2		Equals 0 or 1, if ServerPing=1, then client just query the status of server
		"ServerExt",	//3
		"","","","","",	//4~8

		//Device related Para
		"DeviceFactory",	// 9
		"DeviceIP",			//10
		"DeviceDataType",	//11
		"DeviceType",		//12	refer to series of the device		if DeviceType=vDevice, DeviceDataType describe what kind of file is
		"UserName",			//13
		"Password",			//14
		"DevicePort",		//15
		"DeviceExt",		//16
		"isVirtualDevice",	//17
		"","",				//18~19

		//Chan related Para
		"ChanID",		//20			
		"ChanName",		//21				
		"ChanExt",		//22
		"","","",		//23~25
		"PlayTimes",	//26
		"PlayerType",   //27
	};


	/* -------------------------------------------------------------------------------------------------
	MediaServer�ͻ��˵���������
	---------------------------------------------------------------------------------------------------*/
	enum LK_CLIENT_TYPE
	{
		LK_CLIENT_TYPE_Min			= 0,
		LK_CLIENT_TYPE_UDP          = 0,
		LK_CLIENT_TYPE_TCP          = 1,
		LK_CLIENT_TYPE_HTTP         = 2,
		LK_CLIENT_TYPE_HTTPDropPost = 3,
		LK_CLIENT_TYPE_ReliableUDP  = 4,
		LK_CLIENT_TYPE_Max
	};

	const char LK_CLIENT_TYPE_STR[LK_CLIENT_TYPE_Max][LK_KEYWORD_MaxLen]  =
	{
		"UDP",
		"TCP",
		"HTTP",
		"HTTPDropPost",
		"ReliableUDP"
	};

	/* -------------------------------------------------------------------------------------------------
	�Ѽ��ɵ��豸���ң���Ҫ��һ���޸ģ��Ա��ڵ������豸�Ľ���
	---------------------------------------------------------------------------------------------------*/

	enum DEVICE_FACTORY
	{
		DEVICE_FACTORY_Min = 0,
		DEVICE_FACTORY_HikVision = 0,
		DEVICE_FACTORY_Dahua,
		DEVICE_FACTORY_vDevice,		//Load the Virtual Device
		DEVICE_FACTORY_RelayServer,	//Load RelayClientDLL
		DEVICE_FACTORY_Max
	};

	const char DEVICE_FACTORY_STR[DEVICE_FACTORY_Max][LK_KEYWORD_MaxLen] = 
	{
		"Hikvision",
		"Dahua",
		"vDevice",		//Only when the Device is Virtual Device
		"RelayServer"	//Only when the video is relay through Server
	};


	/* -------------------------------------------------------------------------------------------------
	�ļ������У��ļ�ͷ���ָ�ʽ
	---------------------------------------------------------------------------------------------------*/

	enum LK_FILE_FORMAT
	{
		LK_FILE_FORMAT_HeadSize=0,
		LK_FILE_FORMAT_Version,
		LK_FILE_FORMAT_VerTime,
		LK_FILE_FORMAT_Description,
		LK_FILE_FORMAT_MoreInfo,
		LK_FILE_FORMAT_Max = 5,
	};

	const char LK_FILE_FORMAT_STR[LK_FILE_FORMAT_Max][LK_KEYWORD_MaxLen] = 
	{
		"HeadSize",	//Size of the HeadSection, must less than 1k
		"Version",  //1,2,3,4,5
		"VerTime",  //Time of the version of format implemented
		"Description",
		"MoreInfo",
		//DeviceFactory, //refer to LK_KEYWORD_DEVICE_FACTORY
	};


	/* -------------------------------------------------------------------------------------------------
	������common.DeviceManager.dllʱ������dvrAdaptor.h��Setup����ͨ�����²���ָ��Devicemanager�ĵ�����
	�ǿͻ��˳����Ƿ���������DeviceManager���ݲ�ͬ������ز�ͬ�Ķ�̬��
	---------------------------------------------------------------------------------------------------*/

	enum LK_DEVICE_MANAGER_CallerType
	{
		LK_DEVICE_MANAGER_CallerType_Client = 0,
		LK_DEVICE_MANAGER_CallerType_Server,
		LK_DEVICE_MANAGER_CallerType_TypeName,
		LK_DEVICE_MANAGER_CallerType_Max = 3,
	};

	const char LK_DEVICE_MANAGER_CallerType_STR[LK_DEVICE_MANAGER_CallerType_Max][LK_KEYWORD_MaxLen] = 
	{
		"Server",		// The Caller is the streaming server
		"Client",		// The Caller is the streaming client
		"CallerType",
	};


#endif __INCLUDE_LEKUO_H_
