// LkStrParserTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LkStrParserTest.h"


#include <cppunit/ui/mfc/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LkStrParserTestApp

BEGIN_MESSAGE_MAP(LkStrParserTestApp, CWinApp)
	//{{AFX_MSG_MAP(_TestAppApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LkStrParserTestApp construction

LkStrParserTestApp::LkStrParserTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only LkStrParserTestApp object

LkStrParserTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// LkStrParserTestApp initialization

BOOL LkStrParserTestApp::InitInstance()
{

	RunUnitTests();

	return FALSE;
}

void LkStrParserTestApp::RunUnitTests()
{
	CppUnit::MfcUi::TestRunner runner;
	runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
	
	runner.run(); 
}

