//File: SampleTest.cpp

#include "SampleTest.h"
#include "TestSuiteSample.h"
#include "Windows.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SampleTest, TestSuiteSampleName());

void SampleTest::setUp()
{}


void SampleTest::tearDown()
{}

void SampleTest::SampleTestMethod()
{
//	CLog::Log("This is a Test, line1");
//	CLog::Log("This is a Test, the second line");
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