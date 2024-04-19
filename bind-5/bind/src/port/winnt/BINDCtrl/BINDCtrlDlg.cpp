// BINDCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BINDCtrl.h"
#include "BINDCtrlDlg.h"
#include <bind_service.h>
#include <bind_registry.h>
#include <winsvc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Time that messages appear on the status bar, in milliseconds
#define MESSAGE_FLASH_TIME 1500

// Forward Declaration
static void CALLBACK TimerCallback(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
// Used when shutting down the service
static int numdots = 0;

static CBINDCtrlDlg *gDlgWnd;

#define VERSION "1.2.0.0"

// Service status used for host without BIND
#define SERVICE_NOT_INSTALLED             0x00000008

/////////////////////////////////////////////////////////////////////////////
// CBINDCtrlDlg dialog

CBINDCtrlDlg::CBINDCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBINDCtrlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBINDCtrlDlg)
#ifdef NOTDONE
	m_nonlocalServer = FALSE;
#endif
	m_server = "";
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_version.Format("Version %s", VERSION);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIconRed = AfxGetApp()->LoadIcon(IDI_ICONRED);
	m_hIconYellow = AfxGetApp()->LoadIcon(IDI_ICONYELLOW);
	m_hIconGreen = AfxGetApp()->LoadIcon(IDI_ICONGREEN);
	m_hIconStop = AfxGetApp()->LoadIcon(IDI_ICONSTOP);
	m_pszMsgBuf = new char[64];
}

CBINDCtrlDlg::~CBINDCtrlDlg()
{
	delete[] m_pszMsgBuf;
}

void CBINDCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBINDCtrlDlg)
	DDX_Text(pDX, IDC_VERSION, m_version);
	DDX_Text(pDX, IDC_PARAMS, m_params);
#ifdef NOTDONE
	DDX_Text(pDX, IDC_HOSTNAME, m_server);
	DDX_Check(pDX, IDC_NONLOCAL, m_nonlocalServer);
#endif
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBINDCtrlDlg, CDialog)
	//{{AFX_MSG_MAP(CBINDCtrlDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RELOAD, OnReload)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_DUMP, OnDump)
	ON_BN_CLICKED(IDC_NO_TRACE, OnNoTrace)
	ON_BN_CLICKED(IDC_QRYLOG, OnQrylog)
	ON_BN_CLICKED(IDC_STATS, OnStats)
	ON_BN_CLICKED(IDC_TRACE, OnTrace)
#ifdef NOTDONE
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_NONLOCAL, OnNonlocal)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_CHANGE(IDC_HOSTNAME, OnChangeHostname)
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBINDCtrlDlg message handlers

BOOL CBINDCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Create the status bar
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CCS_BOTTOM;
	CRect sbRect;

	// Make room for the status bar.
	CRect r, rButton;
	GetDlgItem(IDC_RELOAD)->GetClientRect(&rButton);
	GetClientRect(&r);
	r.bottom += 2 * rButton.Height();
	MoveWindow(&r);

	m_version.Format("Version %s", VERSION);

	m_statusBar.Create(dwStyle, sbRect, this, IDC_STATUS_BAR);
	m_statusBar.SetSimple();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_svcStatus.dwCurrentState = SERVICE_NOT_INSTALLED;

	m_hSCManager = OpenSCManager(m_server, NULL, SC_MANAGER_ALL_ACCESS);
	if(!m_hSCManager)
	{
		MessageBox("Could not open Service Control Manager", "BIND Control Panel", MB_OK|MB_ICONSTOP);
	}
	
	m_hBINDSvc = OpenService(m_hSCManager, BIND_SERVICE_NAME, SERVICE_ALL_ACCESS);
	if(!m_hBINDSvc)
	{
		MessageBox("Could not open BIND Service", "BIND Control Panel", MB_OK|MB_ICONSTOP);
	}

	QueryStatus(TRUE);
	
	// Allow timer to call QueryStatus()
	gDlgWnd = this;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBINDCtrlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBINDCtrlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//////////////////////////////////////////////////////////////////////
// Send control codes to the service
void CBINDCtrlDlg::OnStart() 
{
	LPTSTR params[32];
	int paramCount = 0;

	UpdateData();
	if(!m_params.IsEmpty())
	{
		
		LPTSTR token;
		LPCTSTR separators = " ,\t";
		LPTSTR buf = m_params.GetBuffer(0);
		token = strtok(buf, separators);
		while(token)
		{
			if(!strcmp(token, "-f") || !strcmp(token, "/f"))
			{
				MessageBox("Foreground switch (-f) is invalid for operation as a service; ignoring it.", "BIND Control Panel", MB_OK|MB_ICONSTOP);
			}
			else
			{
				params[paramCount] = new char[strlen(token) + 1];
				memset(params[paramCount], 0, strlen(token) + 1);
				strcpy(params[paramCount], token);
				paramCount++;
			}
			token = strtok(NULL, separators);
		}
		m_params.ReleaseBuffer();
		params[paramCount] = NULL;
	}

	BOOL rc = StartService(m_hBINDSvc, paramCount, (const char **)params);
	if(rc)
		QueryStatus(TRUE);
	else
		FlashErrorMessage("Service start failed");

	for(int i = 0; i < paramCount; i++)
		delete[] params[i];
}

void CBINDCtrlDlg::OnStop() 
{
	BOOL rc = ControlService(m_hBINDSvc, SERVICE_CONTROL_STOP, &m_svcStatus);

	if(rc && !GetLastError())
		QueryStatus(TRUE);
	else
		FlashErrorMessage("Service stop failed");
}

void CBINDCtrlDlg::OnReload() 
{
	BOOL rc = ControlService(m_hBINDSvc, SERVICE_CONTROL_RELOAD, &m_svcStatus);
	
	if(rc && !GetLastError())
		SetStatusBarText("Reload signal dispatched");
	else
		FlashErrorMessage("Reload signal failed");

	QueryStatus(TRUE);
}

void CBINDCtrlDlg::OnDump() 
{
	BOOL rc = ControlService(m_hBINDSvc, SERVICE_CONTROL_DUMPDB, &m_svcStatus);
	
	if(rc && !GetLastError())
		SetStatusBarText("Dump DB signal dispatched");
	else
		FlashErrorMessage("Dump DB signal failed");
	QueryStatus(TRUE);
}

void CBINDCtrlDlg::OnNoTrace() 
{
	BOOL rc = ControlService(m_hBINDSvc, SERVICE_CONTROL_NOTRACE, &m_svcStatus);
	
	if(rc && !GetLastError())
		SetStatusBarText("No trace signal dispatched");
	else
		FlashErrorMessage("No trace signal failed");
	QueryStatus(TRUE);
}

void CBINDCtrlDlg::OnQrylog() 
{
	BOOL rc = ControlService(m_hBINDSvc, SERVICE_CONTROL_QRYLOG, &m_svcStatus);
	
	if(rc && !GetLastError())
		SetStatusBarText("Query log toggled");
	else
		FlashErrorMessage("Toggle query log signal failed");
	QueryStatus(TRUE);
}

void CBINDCtrlDlg::OnStats() 
{
	BOOL rc = ControlService(m_hBINDSvc, SERVICE_CONTROL_STATS, &m_svcStatus);
	
	if(rc && !GetLastError())
		SetStatusBarText("Dump stats signal dispatched");
	else
		FlashErrorMessage("Dump stats signal failed");
	QueryStatus(TRUE);
}

void CBINDCtrlDlg::OnTrace() 
{
	BOOL rc = ControlService(m_hBINDSvc, SERVICE_CONTROL_TRACE, &m_svcStatus);
	
	if(rc && !GetLastError())
		SetStatusBarText("Increment trace signal dispatched");
	else
		FlashErrorMessage("Increment trace signal failed");
	QueryStatus(TRUE);
}

void CBINDCtrlDlg::OnClear() 
{
	GetDlgItem(IDC_PARAMS)->SetWindowText("");
}

//////////////////////////////////////////////////////////////////////
// Support functions
CString CBINDCtrlDlg::StatusString(int status)
{
	CString pszStatus;

	switch(status)
	{
		case SERVICE_RUNNING:
			pszStatus = "running";
			break;
		case SERVICE_START_PENDING:
			pszStatus = "starting";
			break;
		case SERVICE_STOP_PENDING:
			pszStatus = "stopping";
			break;
		case SERVICE_NOT_INSTALLED:
			pszStatus = "not installed";
			break;
		default:
			pszStatus = "stopped";
			break;
	}
	return(pszStatus);
}

void CBINDCtrlDlg::QueryStatus(BOOL update)
{
	TCHAR pszStatus[128];
	BOOL bStarted;

	// We only perform actual updates to the status structure if we are asked
	// or we have a start or stop state pending.
	if(update == TRUE ||m_svcStatus.dwCurrentState == SERVICE_START_PENDING ||
		m_svcStatus.dwCurrentState == SERVICE_STOP_PENDING)
		
		ControlService(m_hBINDSvc, SERVICE_CONTROL_INTERROGATE, &m_svcStatus);

	if(m_svcStatus.dwCurrentState == SERVICE_START_PENDING || m_svcStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// Service starting or stopping.  Set a timer to update state automatically
		SetTimer(0x42, 200, TimerCallback);
	}
	else
	{
		numdots = 0;
	}

	bStarted = (m_svcStatus.dwCurrentState == SERVICE_RUNNING);
	
	GetDlgItem(IDC_START)->EnableWindow(!bStarted &&
		m_svcStatus.dwCurrentState != SERVICE_STOP_PENDING && m_svcStatus.dwCurrentState != SERVICE_START_PENDING
		&& m_svcStatus.dwCurrentState != SERVICE_NOT_INSTALLED);
	
	GetDlgItem(IDC_PARAMS)->EnableWindow(!bStarted &&
		m_svcStatus.dwCurrentState != SERVICE_STOP_PENDING && m_svcStatus.dwCurrentState != SERVICE_START_PENDING
		&& m_svcStatus.dwCurrentState != SERVICE_NOT_INSTALLED);

	GetDlgItem(IDC_STOP)->EnableWindow(bStarted);
	GetDlgItem(IDC_RELOAD)->EnableWindow(bStarted);
	GetDlgItem(IDC_DUMP)->EnableWindow(bStarted);
	GetDlgItem(IDC_STATS)->EnableWindow(bStarted);
	GetDlgItem(IDC_TRACE)->EnableWindow(bStarted);
	GetDlgItem(IDC_NO_TRACE)->EnableWindow(bStarted);

	GetDlgItem(IDC_QRYLOG)->EnableWindow(bStarted);

	memset(pszStatus, 0, 128);
	sprintf(pszStatus, "BIND service is %s", StatusString(m_svcStatus.dwCurrentState));
	if(numdots > 0)
		for(int i = 0; i < numdots; i++)
			strcat(pszStatus, ".");

	SetStatusBarText(pszStatus);

	if(numdots)
	{
		RECT paneRect;
		CSize csz;

		for(int i = 0; i < numdots; i++)
			strcat(pszStatus, " .");

		csz = GetDC()->GetTextExtent(pszStatus, strlen(pszStatus));
		GetWindowRect(&paneRect);
		int width = paneRect.right - paneRect.left;
		if(csz.cx > width)
			numdots = 0;
	}
	BOOL rc = m_statusBar.SetText(pszStatus, 255, 0);
	QrylogStatus();
}

LPCTSTR CBINDCtrlDlg::GetErrMessage()
{
	DWORD err = GetLastError();

	memset(m_pszMsgBuf, 0, 64);
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, m_pszMsgBuf, 128, NULL);

	return m_pszMsgBuf;
}

void CBINDCtrlDlg::SetStatusBarText(LPTSTR msg)
{
	BOOL rc = m_statusBar.SetText(msg, 255, 0);
	if(!rc)
		MessageBox("Error setting status bar text!", "BIND Control Panel", MB_OK|MB_ICONSTOP);
	// If we set the timer here, timerID is 0 in QueryStatus, so no dots.
	SetTimer(0x42, MESSAGE_FLASH_TIME, TimerCallback);
}

void CBINDCtrlDlg::QrylogStatus()
{
	HKEY hKey;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwType; 
	char buf[32];

	if(RegOpenKey(HKEY_LOCAL_MACHINE, BIND_SESSION_SUBKEY, &hKey) != ERROR_SUCCESS)
		return;

	m_qrylog = RegQueryValueEx(hKey, "QueryLog", NULL, &dwType, (LPBYTE)&m_qrylog, &dwSize);
	
	sprintf(buf, "Query Log %s", m_qrylog ? "Off" : "On");
	GetDlgItem(IDC_QRYLOG)->SetWindowText(buf);
}

void CBINDCtrlDlg::FlashErrorMessage(LPTSTR msg)
{
	TCHAR buf[128];

	sprintf(buf, "%s: %s", msg, GetErrMessage());
	SetStatusBarText(buf);
}

static void CALLBACK TimerCallback(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	KillTimer(hWnd, nIDEvent);
	
	numdots++;

	gDlgWnd->QueryStatus(FALSE);
}

#ifdef NOTDONE
void CBINDCtrlDlg::OnBrowse() 
{
	// TODO: Add your control notification handler code here
	LPITEMIDLIST	lpiFolder,				
					lpiServeur;				
	TCHAR			szFolder[_MAX_PATH+1];	
	TCHAR			szLocalComputerName[MAX_COMPUTERNAME_LENGTH+1];
	BROWSEINFO		biFolder;				
	LPSHELLFOLDER	lpsfDestopFolder;
	SHDESCRIPTIONID ntServeur;
	DWORD			dwSize = MAX_COMPUTERNAME_LENGTH + 1;
	CString			csMessage;

	if (SHGetDesktopFolder( &lpsfDestopFolder) != NOERROR)
	{
		MessageBox( "Memory error !", "BIND Control Panel", MB_OK|MB_ICONSTOP);
		return;
	}
	if (SHGetSpecialFolderLocation( m_hWnd, CSIDL_NETWORK, &lpiFolder) != NOERROR)
	{
		MessageBox( "Memory error !", "BIND Control Panel", MB_OK|MB_ICONSTOP);
		return;
	}
	biFolder.hwndOwner = m_hWnd;
	biFolder.pidlRoot = lpiFolder;
	biFolder.pszDisplayName = szFolder;
	biFolder.lpszTitle = "Select a Windows NT host which run BIND DNS...";
	biFolder.ulFlags = BIF_BROWSEFORCOMPUTER;
	biFolder.lpfn = NULL;
	biFolder.lParam = 0L;
	if ((lpiServeur = SHBrowseForFolder( &biFolder)) == NULL)
	{
		return;
	}
	if (SHGetDataFromIDList( lpsfDestopFolder, lpiServeur, SHGDFIL_DESCRIPTIONID, &ntServeur, sizeof( ntServeur)) != NOERROR)
	{
		MessageBox( "Memory error !", "BIND Control Panel", MB_OK|MB_ICONSTOP);
		return;
	}
	m_server = szFolder;
	if (GetComputerName( szLocalComputerName, &dwSize) == 0)
	{
		MessageBox( "Memory error !", "BIND Control Panel", MB_OK|MB_ICONSTOP);
		return;
	}
	if (strcmpi( m_server, szLocalComputerName) == 0)
		// Local computer => Server name = null
		m_server.Empty();

	UpdateData(FALSE);
	OnChangeHostname();

    // close the service handle
    CloseServiceHandle( m_hBINDSvc);
    // close the service control manager handle
    CloseServiceHandle( m_hSCManager);
	m_svcStatus.dwCurrentState = SERVICE_NOT_INSTALLED;

	m_hSCManager = OpenSCManager(m_server, NULL, SC_MANAGER_ALL_ACCESS);
	if(!m_hSCManager)
	{
		MessageBox("Could not open Service Control Manager", "BIND Control Panel", MB_OK|MB_ICONSTOP);
	}
	
	m_hBINDSvc = OpenService(m_hSCManager, BIND_SERVICE_NAME, SERVICE_ALL_ACCESS);
	if(!m_hBINDSvc)
	{
		MessageBox("Could not open BIND Service", "BIND Control Panel", MB_OK|MB_ICONSTOP);
	}

	QueryStatus();
}

void CBINDCtrlDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
    // close the service handle
    CloseServiceHandle( m_hBINDSvc);
    // close the service control manager handle
    CloseServiceHandle( m_hSCManager);
}

void CBINDCtrlDlg::OnNonlocal() 
{
	UpdateData();
	if(m_nonlocalServer)
	{
		GetDlgItem(IDC_HOSTNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BROWSE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_HOSTNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
	}
}

void CBINDCtrlDlg::OnChangeHostname() 
{
	CString title;

	UpdateData();
	title.Format("BIND Control Panel (%s)", m_server);
	SetWindowText(title);
}
#endif
