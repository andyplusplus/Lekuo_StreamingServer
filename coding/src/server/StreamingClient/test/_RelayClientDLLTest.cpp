// _RelayClientDLLTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "_RelayClientDLLTest.h"


#include <cppunit/ui/mfc/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// _RelayClientDLLTestApp

BEGIN_MESSAGE_MAP(_RelayClientDLLTestApp, CWinApp)
	//{{AFX_MSG_MAP(_TestAppApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// _RelayClientDLLTestApp construction

_RelayClientDLLTestApp::_RelayClientDLLTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only _RelayClientDLLTestApp object

_RelayClientDLLTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// _RelayClientDLLTestApp initialization

BOOL _RelayClientDLLTestApp::InitInstance()
{

	RunUnitTests();

	return FALSE;
}

void _RelayClientDLLTestApp::RunUnitTests()
{
	CppUnit::MfcUi::TestRunner runner;
	runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
	
	runner.run(); 
}

