#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "Chess.h"
#include "ConnectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PORTNUMBER 3571

CConnectDlg::CConnectDlg(CWnd* pParent /*=NULL*/) : CDialog(CConnectDlg::IDD, pParent) {
  const Options &options = getOptions();
  m_serverComputerName = options.getServerComputerName().cstr();
  m_connectedToServer  = options.isConnectedToServer() ? 1 : 0;
  m_listener = INVALID_SOCKET;
  m_timerIsRunning = false;
}

void CConnectDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX , IDC_EDIT_COMPUTERNAME, m_serverComputerName);
  DDX_Radio(pDX, IDC_RADIO_CREATEGAME , m_connectedToServer );
}

BEGIN_MESSAGE_MAP(CConnectDlg, CDialog)
	ON_COMMAND(ID_GOTO_COMPUTERNAME , OnGotoComputerName    )
	ON_BN_CLICKED(IDC_RADIO_CREATEGAME, OnRadioCreateGame)
	ON_BN_CLICKED(IDC_RADIO_CONNECTTOGAME, OnRadioConnectToGame)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CConnectDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  setControlText(IDD, this);

  enableComputerName();
  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_CONNECT_ACCELERATOR));
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectDlg::OnOK() {
  UpdateData();
  m_serverComputerName.TrimLeft();
  m_serverComputerName.TrimRight();
  if(m_connectedToServer && m_serverComputerName.GetLength() == 0) {
    gotoEditBox(this, IDC_EDIT_COMPUTERNAME);
    MessageBox(loadString(IDS_MSG_EMPTYCOMPUTERNAME).cstr(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONWARNING);
    return;
  }
  Options &options = getOptions();
  options.setServerComputerName((LPCTSTR)m_serverComputerName);
  options.setConnectedToServer(m_connectedToServer?true:false);
  makeHandshake();
}

void CConnectDlg::OnCancel() {
  CLOSESOCKET(m_listener);
  CDialog::OnCancel();
}

void CConnectDlg::OnGotoComputerName() {
  gotoEditBox(this, IDC_EDIT_COMPUTERNAME);
}

BOOL CConnectDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CConnectDlg::OnRadioCreateGame() {
  enableComputerName();
}

void CConnectDlg::OnRadioConnectToGame() {
  enableComputerName();
  OnGotoComputerName();
}

void CConnectDlg::enableComputerName() {
  BOOL enable = IsDlgButtonChecked(IDC_RADIO_CONNECTTOGAME);
  GetDlgItem(IDC_EDIT_COMPUTERNAME  )->EnableWindow(enable);
  GetDlgItem(IDC_STATIC_COMPUTERNAME)->EnableWindow(enable);
}

void CConnectDlg::makeHandshake() {
  const Options &options = getOptions();
  try {
    if(options.isConnectedToServer()) {
      const String serverName = options.getServerComputerName();
      USES_CONVERSION;
      const char *aserverName = T2A(serverName.cstr());
      SOCKET writeS = tcpOpen(PORTNUMBER, aserverName);
      SOCKET readS  = tcpOpen(PORTNUMBER, aserverName);
      m_channel = SocketChannel(readS, writeS);
      m_channel.read(m_myColor);
      CDialog::OnOK();
    } else {
      m_listener = tcpCreate(PORTNUMBER);
      GetDlgItem(IDC_STATIC_WAITING      )->SetWindowText(loadString(IDS_MSG_WAIT_FOR_CONNECTION).cstr());
      GetDlgItem(IDC_GROUP_CONNECTMETHOD )->EnableWindow(FALSE);
      GetDlgItem(IDC_RADIO_CONNECTTOGAME )->EnableWindow(FALSE);
      GetDlgItem(IDC_RADIO_CREATEGAME    )->EnableWindow(FALSE);
      GetDlgItem(IDOK                    )->EnableWindow(FALSE);
      startTimer();
    }
  } catch(Exception e) {
    MessageBox(e.what(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONWARNING);
  }
}

void CConnectDlg::OnTimer(UINT_PTR nIDEvent) {
  if(tcpPoll(m_listener)) {
    stopTimer();
    SOCKET readS  = tcpAccept(m_listener);
    SOCKET writeS = tcpAccept(m_listener);
    m_channel = SocketChannel(readS, writeS);
    CLOSESOCKET(m_listener);
    const Player remotePlayer = getOptions().getComputerPlayer();
    m_myColor = GETENEMY(remotePlayer);
    m_channel.write(remotePlayer);
    CDialog::OnOK();
  }
  CDialog::OnTimer(nIDEvent);
}

void CConnectDlg::startTimer() {
  if(SetTimer(1, 100, NULL)) {
    m_timerIsRunning = true;
  }
}

void CConnectDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}
