#include "stdafx.h"
#include "_LkStrParserSuite.h"
#include "_LkStrParserTest.h"
#include "ILkTokenParser.h"
#include "LkStrParser.h"
#include "StdRsType.h"


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( _LkStrParserTest, LkStrParserSuiteName());

void _LkStrParserTest::StrTest()
{
	//#include "StdRsType.h"
	//std::auto_ptr<ILkTokenParser> theTokenParser(CreateTokenParser(inURL));

	//char* testStr="DeviceIP=127.0.0.1;UserName=WuZengDe;CompanyName=LeKuo";
	char* testStr="PLAY rtsp://127.0.0.1/;  UserName=admin;ChanID=01;ClientType=UDP;DeviceFactory=Hikvision;DeviceIP=192.168.0.58;DevicePort=8000;Password=password;ServerIP=127.0.0.1;isVirtualDevice=-1; RTSP/1.0 CSeq: 3 Session: 2860448235675 Range: npt=0.0- x-prebuffer: maxtime=3.0 User-agent: LeKuo Client";
	OutputDebugString(testStr);
	ILkTokenParser* theParser=CreateTokenParser(testStr);

	char theResult[1024];
	theParser->getToken("UserName", theResult, 1024, NULL);
	//CPPUNIT_ASSERT(strcmp(theResult, "WuZengDe")==0);

	theParser->getAllTokens(theResult, 1024);
	//OutputDebugString(theResult);
	theParser->DelServerToken();

	theParser->getAllTokens(theResult, 1024);
	OutputDebugString("\n");
	OutputDebugString(theResult);

	DestroyTokenParser(theParser);
}
