#include "stdafx.h"
#include "__RelayClientDLLTestTest.h"


#include "InstanceClient.h"
#include "ServerClient.h"
#include "ChannelClient.h"

//#ifdef _I_Test_GUI_Suite
#include "__RelayClientDLLTestSuite.h"
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( __RelayClientDLLTestTest, _RelayClientDLLTestSuiteName());
//#endif

const Tag InstanceTAG = 1;
const Tag RelayServerTAG = 2; 
const Tag ChannelTAG = 3;

const char* CreateInstanceCmd = "InstanceCmd=  ----> This is Create Instance URL";
const char* CreateRelayServerURL = "ServerIP=127.0.0.1,ClientType=RTSPUDPClientType";
const char* CreateChannelURL = "DeviceType=HikVision,DeviceIP=192.168.0.1,UserName=admin,Password=12345,DevicePort=8000,ChanID=0";
const char* PlayChannelURL			= "PlayChanCmd=  ---->   Play Channel URL";
const char* StopChannelURL			= "StopChanCmd=  ---->   Stop Channel URL";
const char* SendChannelCommandURL	= "SendChanCmd=  ---->   Send Channel Command URL";


InstanceClient sInstanceClient;
RelayClient* sRelayClient;
HANDLE sInstanceClientHandle;

void __RelayClientDLLTestTest::setUp()
{

}


void __RelayClientDLLTestTest::tearDown()
{

}


void __RelayClientDLLTestTest::SimpleTest()
{

	sRelayClient = RelayClient::Instance(CreateInstanceCmd, &sInstanceClient, InstanceTAG);
	sInstanceClientHandle = RelayClient::toHandle(sRelayClient);
	Sleep(200);

	//==================================================================================================================


	ServerClient theServerClient;
	HANDLE theServerHandle = sRelayClient->CreateRelayServer(sInstanceClientHandle, CreateRelayServerURL, &theServerClient, RelayServerTAG);
	Sleep(200);
	
	ChannelClient theChannelClient;
	HANDLE theChannelHandle = sRelayClient->CreateChannelInRelayServer(theServerHandle, CreateChannelURL, &theChannelClient, ChannelTAG);
	Sleep(200);

	
	LK_Error theResult;

	theResult = sRelayClient->PlayChannel(theChannelHandle, PlayChannelURL);
	CPPUNIT_ASSERT(LK_NoErr ==theResult );
	Sleep(1000);

	theResult = sRelayClient->StopChannel(theChannelHandle, StopChannelURL);
	CPPUNIT_ASSERT(LK_NoErr ==theResult );
	Sleep(200);

	theResult = sRelayClient->SendChannelCommands(theChannelHandle, SendChannelCommandURL);
	CPPUNIT_ASSERT(LK_NoErr ==theResult );
	Sleep(200);

	theResult = sRelayClient->DestroyChannel(theChannelHandle);
	CPPUNIT_ASSERT(LK_NoErr ==theResult );
	Sleep(200);

	theResult = sRelayClient->DestroyALLChannelInRelayServer(theServerHandle);
	CPPUNIT_ASSERT(LK_NoErr ==theResult );
	Sleep(10);

	theResult = sRelayClient->DestroyRelayServer(theServerHandle);
	CPPUNIT_ASSERT(LK_NoErr ==theResult );
	Sleep(10);

	theResult = sRelayClient->DestroyALLRelayServer();
	CPPUNIT_ASSERT(LK_NoErr ==theResult );
	Sleep(10);

	//==================================================================================================================

	//LK_Error theResult;
	theResult = sRelayClient->DestroyInstance();
	CPPUNIT_ASSERT(LK_NoErr ==theResult );
	Sleep(10);

}
