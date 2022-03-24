// _TestGUI.cpp : Defines the class behaviors for the application.
//

#include "_TestGUI_StdAfx.h"
#include "_TestGUI.h"


#include <cppunit/ui/mfc/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// _TestGUIApp

BEGIN_MESSAGE_MAP(_TestGUIApp, CWinApp)
	//{{AFX_MSG_MAP(_TestAppApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// _TestGUIApp construction

_TestGUIApp::_TestGUIApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only _TestGUIApp object

_TestGUIApp theApp;

/////////////////////////////////////////////////////////////////////////////
// _TestGUIApp initialization

BOOL _TestGUIApp::InitInstance()
{

	RunUnitTests();

	return FALSE;
}

void _TestGUIApp::RunUnitTests()
{
	CppUnit::MfcUi::TestRunner runner;
	runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
	
	runner.run(); 
}