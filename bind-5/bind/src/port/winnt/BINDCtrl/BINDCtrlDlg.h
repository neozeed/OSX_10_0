// BINDCtrlDlg.h : header file
//

#if !defined(AFX_BINDCTRLDLG_H__BFE182C7_2B0A_11D3_A979_00105A12BD65__INCLUDED_)
#define AFX_BINDCTRLDLG_H__BFE182C7_2B0A_11D3_A979_00105A12BD65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsvc.h>

/////////////////////////////////////////////////////////////////////////////
// CBINDCtrlDlg dialog

class CBINDCtrlDlg : public CDialog
{
// Construction
public:
	CBINDCtrlDlg(CWnd* pParent = NULL);	// standard constructor
	~CBINDCtrlDlg();					// standard destructor
	void QueryStatus(BOOL Update = TRUE);
	CString	m_version;
	CString	m_params;
	CString m_server;

// Dialog Data
	//{{AFX_DATA(CBINDCtrlDlg)
	enum { IDD = IDD_BINDCTRL_DIALOG };
	BOOL	m_nonlocalServer;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBINDCtrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hIconStop;
	HICON m_hIconRed;
	HICON m_hIconYellow;
	HICON m_hIconGreen;
	HANDLE m_hSCManager;
	HANDLE m_hBINDSvc;
	SERVICE_STATUS m_svcStatus;
	LPTSTR m_pszMsgBuf;

	CStatusBarCtrl m_statusBar;

	DWORD m_qrylog;

	// Generated message map functions
	//{{AFX_MSG(CBINDCtrlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReload();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnClear();
	afx_msg void OnDump();
	afx_msg void OnNoTrace();
	afx_msg void OnQrylog();
	afx_msg void OnStats();
	afx_msg void OnTrace();
#ifdef NOTDONE
	afx_msg void OnDestroy();
	afx_msg void OnNonlocal();
	afx_msg void OnBrowse();
	afx_msg void OnChangeHostname();
#endif
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	LPCTSTR GetErrMessage();
	CString StatusString(int status);
	void FlashErrorMessage(LPTSTR msg);
	void SetStatusBarText(LPTSTR msg);
	void QrylogStatus();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BINDCTRLDLG_H__BFE182C7_2B0A_11D3_A979_00105A12BD65__INCLUDED_)
