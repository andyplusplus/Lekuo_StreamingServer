// LkStrParserTest.h : main header file for the LkStrParserTest application
//

#if !defined(AFX_LkStrParserTest_H__5ADA4602_4A78_4961_B0EB_034CD7FF94C1__INCLUDED_)
#define AFX_LkStrParserTest_H__5ADA4602_4A78_4961_B0EB_034CD7FF94C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// LkStrParserTestApp:
// See LkStrParserTest.cpp for the implementation of this class
//

class LkStrParserTestApp : public CWinApp
{
public:
	LkStrParserTestApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(_TestAppApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(_TestAppApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:
		void RunUnitTests();

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LkStrParserTest_H__5ADA4602_4A78_4961_B0EB_034CD7FF94C1__INCLUDED_)
