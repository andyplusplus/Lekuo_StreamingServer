// __RelayClientDLLTest.h

#ifndef _RelayClientDLLTest_H
#define _RelayClientDLLTest_H

#include <cppunit/extensions/HelperMacros.h>

class __RelayClientDLLTestTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( __RelayClientDLLTestTest );

	CPPUNIT_TEST(SimpleTest);

	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();

	void SimpleTest();
 };

#endif
