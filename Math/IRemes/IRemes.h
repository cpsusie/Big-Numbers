// IRemes.h : main header file for the IREMES application
//

#if !defined(AFX_IREMES_H__F83FE60A_8531_44CA_9AEF_530744DC354E__INCLUDED_)
#define AFX_IREMES_H__F83FE60A_8531_44CA_9AEF_530744DC354E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIRemesApp:
// See IRemes.cpp for the implementation of this class
//

class CIRemesApp : public CWinApp
{
public:
	CIRemesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IREMES_H__F83FE60A_8531_44CA_9AEF_530744DC354E__INCLUDED_)
