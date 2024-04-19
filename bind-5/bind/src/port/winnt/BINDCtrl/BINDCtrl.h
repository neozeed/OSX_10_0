// BINDCtrl.h : main header file for the BINDCTRL application
//

#if !defined(AFX_BINDCTRL_H__BFE182C5_2B0A_11D3_A979_00105A12BD65__INCLUDED_)
#define AFX_BINDCTRL_H__BFE182C5_2B0A_11D3_A979_00105A12BD65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBINDCtrlApp:
// See BINDCtrl.cpp for the implementation of this class
//

class CBINDCtrlApp : public CWinApp
{
public:
	CBINDCtrlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBINDCtrlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBINDCtrlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BINDCTRL_H__BFE182C5_2B0A_11D3_A979_00105A12BD65__INCLUDED_)
