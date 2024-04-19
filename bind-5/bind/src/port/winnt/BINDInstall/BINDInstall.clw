; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWaitDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "bindinstall.h"
LastPage=0

ClassCount=4
Class1=CBINDInstallApp
Class2=CBINDInstallDlg
Class3=CDirBrowse

ResourceCount=2
Resource1=IDD_BINDINSTALL_DIALOG
Class4=CWaitDlg
Resource2=IDD_BROWSE

[CLS:CBINDInstallApp]
Type=0
BaseClass=CWinApp
HeaderFile=BINDInstall.h
ImplementationFile=BINDInstall.cpp

[CLS:CBINDInstallDlg]
Type=0
BaseClass=CDialog
HeaderFile=BINDInstallDlg.h
ImplementationFile=BINDInstallDlg.cpp
LastObject=CBINDInstallDlg
Filter=D
VirtualFilter=dWC

[CLS:CDirBrowse]
Type=0
BaseClass=CDialog
HeaderFile=DirBrowse.h
ImplementationFile=DirBrowse.cpp
Filter=D
VirtualFilter=dWC
LastObject=CDirBrowse

[DLG:IDD_BINDINSTALL_DIALOG]
Type=1
Class=CBINDInstallDlg
ControlCount=22
Control1=IDC_STATIC,static,1342308352
Control2=IDC_TARGETDIR,edit,1350631552
Control3=IDC_STATIC,button,1342177287
Control4=IDC_COPY_TAG,static,1342308354
Control5=IDC_COPY_FILE,static,1342308352
Control6=IDC_SERVICE_TAG,static,1342308354
Control7=IDC_REG_SERVICE,static,1342308352
Control8=IDC_MESSAGE_TAG,static,1342308354
Control9=IDC_INSTALL,button,1342242817
Control10=IDC_REG_MESSAGE,static,1342308352
Control11=IDC_DIR_TAG,static,1342308354
Control12=IDC_EXIT,button,1342242816
Control13=IDC_AUTO_START,button,1342242819
Control14=IDC_STATIC,button,1342177287
Control15=IDC_UNINSTALL,button,1342242816
Control16=IDC_VERSION,static,1342312961
Control17=IDC_KEEP_FILES,button,1342242819
Control18=IDC_BROWSE,button,1342242816
Control19=IDC_CURRENT_TAG,static,1342308354
Control20=IDC_CURRENT,static,1342308352
Control21=IDC_CREATE_DIR,static,1342308352
Control22=IDC_START,button,1342242819

[DLG:IDD_BROWSE]
Type=1
Class=CDirBrowse
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_DIRLIST,listbox,1352728835
Control4=IDC_CURDIR,edit,1350631552
Control5=IDC_DRIVES,combobox,1344340227

[CLS:CWaitDlg]
Type=0
HeaderFile=WaitDlg.h
ImplementationFile=WaitDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CWaitDlg

