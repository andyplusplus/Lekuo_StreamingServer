//File: SampleTest.cpp

#include "SampleTest.h"
#include "TestSuiteSample.h"
#include "Windows.h"
#include "RsRTSPUtility.h"


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SampleTest, TestSuiteSampleName());

void SampleTest::setUp()
{}


void SampleTest::tearDown()
{}

void SampleTest::SampleTestMethod()
{
	char theChannelIP[NORM_CHAR_SIZE];		//通道Ip
	char theChannelID[NORM_CHAR_SIZE];		//通道号	
	const char* strPara = "RTSP://192.168.0.106/ClientType=RTSPUDPClientType,DeviceType=HikVision,DeviceIP=192.168.0.58,UserName=admin,Password=12345,DevicePort=8000,ChanID=0,() ";
	BOOL theParseResult = FALSE;
	theParseResult = RsRTSPUtility::parserDataFromStr(strPara, theChannelIP, NORM_CHAR_SIZE, LK_KEYWORD_Device_IP);
	theParseResult = RsRTSPUtility::parserDataFromStr(strPara, theChannelID, NORM_CHAR_SIZE, LK_KEYWORD_Channel_ID);
	RsRTSPUtility::isURLSelfDefined(strPara);
}

void SampleTest::HelperMethod()
{

	bool theCondition = true;
	char theMessage[] = "Message";

	//Assertions that a condition is true
	CPPUNIT_ASSERT( theCondition );

	//Message reported in diagnostic if condition evaluates to false
	CPPUNIT_ASSERT_MESSAGE(theMessage, theCondition);

	//Fails with the specified message
	//CPPUNIT_FAIL(theMessage);

	int expected = 1;
	int actual = 1;
	int delta = 1;

	//Asserts that two values are equals
	CPPUNIT_ASSERT_EQUAL(expected, actual);
		
	//Asserts that two values are equals, provides additional messafe on failure
	CPPUNIT_ASSERT_EQUAL_MESSAGE(theMessage, expected, actual);
		
	//Macro for primitive value comparisons	
	CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, actual, delta);


	//Asserts that the given expression throws an exception of the specified type.	
	//CPPUNIT_ASSERT_THROW(expression, ExceptionType)
		
	//Asserts that the given expression does not throw any exceptions.	
	//CPPUNIT_ASSERT_NO_THROW(expression)
		
		
	//Asserts that an assertion fail
	//CPPUNIT_ASSERT_ASSERTION_FAIL(assertion);		

	//Asserts that an assertion pass	
	//CPPUNIT_ASSERT_ASSERTION_PASS(assertion);
}