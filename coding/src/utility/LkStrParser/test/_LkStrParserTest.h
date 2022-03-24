// _LkStrParserTest.h

#ifndef LkStrParserTEST_H
#define LkStrParserTEST_H

#include <cppunit/extensions/HelperMacros.h>

class _LkStrParserTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( _LkStrParserTest );
	CPPUNIT_TEST(StrTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void StrTest();
 };

#endif
