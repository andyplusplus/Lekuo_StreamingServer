//File: SampleTest.h

#ifndef _SAMPLETEST_H
#define _SAMPLETEST_H
#include <cppunit/extensions/HelperMacros.h>

class SampleTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( SampleTest );
	CPPUNIT_TEST(SampleTestMethod);
	CPPUNIT_TEST(HelperMethod);

	//ToDo: 
	// Add Test Method into Test Suite

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void SampleTestMethod();
	void HelperMethod();
	
	//ToDo:
	// Add Test Method

 };

#endif
