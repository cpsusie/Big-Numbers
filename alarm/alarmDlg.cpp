#include "stdafx.h"
#include <MyUtil.h>
#include <Date.h>
#include <Registry.h>
#include "AlarmDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg() : CDialog(IDD) {
  }

protected:
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CAlarmDlg::CAlarmDlg(CWnd *pParent) : CDialog(IDD, pParent) {
  m_time = EMPTYSTRING;
  m_hIcon = theApp.LoadIcon(IDR_MAINICON);
  for(int t = 0; t < MAXTIMERCOUNT; t++) {
    m_timerIsRunning[t] = false;
  }
}

void CAlarmDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITALARMTIME, m_time);
}

BEGIN_MESSAGE_MAP(CAlarmDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_TIMER()
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_CLOSE()
  ON_COMMAND(ID_FILE_EXIT               , OnFileExit               )
  ON_COMMAND(ID_OPTIONS_SOUND           , OnOptionsSound           )
  ON_COMMAND(ID_MENU_EXIT               , OnMenuExit               )
  ON_COMMAND(ID_MENU_OPEN               , OnMenuOpen               )
  ON_COMMAND(ID_HELP_ABOUTALARM         , OnHelpAboutalarm         )
  ON_COMMAND(ID_GOTO_ALARMTIME          , OnGotoAlarmTime          )
  ON_COMMAND(ID_OPTIONS_HIDEWHENSETALARM, OnOptionsHidewhensetalarm)
  ON_BN_CLICKED(IDC_SETALARM            , OnSetalarm               )
  ON_BN_CLICKED(IDC_BUTTONPLAY          , OnButtonPlay             )
END_MESSAGE_MAP()

class Options {
private:
  String m_soundFileName;
  bool   m_hideWhenSetAlarm;

  void clear();
  RegistryKey getKey();
public:
  void load();
  void save();

  const String &getSoundFileName() const {
    return m_soundFileName;
  }

  void setSoundFileName(const TCHAR *name) {
    m_soundFileName = name;
  }

  bool getHideWhenSetAlarm() const {
    return m_hideWhenSetAlarm;
  }

  void setHideWhenSetAlarm(bool hide) {
    m_hideWhenSetAlarm = hide;
  }
};

void Options::clear() {
  m_soundFileName    = EMPTYSTRING;
  m_hideWhenSetAlarm = true;
}

RegistryKey Options::getKey() {
  return RegistryKey(HKEY_CURRENT_USER,_T("Software")).createOrOpenKey(_T("JGMData\\Alarm\\Options"));
}

void Options::load() {
  try {
    clear();
    RegistryKey key    = getKey();
    m_soundFileName    = key.getString(_T("sound")      , EMPTYSTRING);
    m_hideWhenSetAlarm = key.getBool(  _T("hidewhenset"), false);
  } catch(Exception) {
    // ignore
  }
}

void Options::save() {
  try {
    RegistryKey key    = getKey();
    key.setValue(_T("sound")      , m_soundFileName   );
    key.setValue(_T("hidewhenset"), m_hideWhenSetAlarm);
  } catch(Exception) {
    // ignore
  }
}

void CAlarmDlg::readSetup() {
  Options options;
  options.load();

  m_soundFileName = options.getSoundFileName().cstr();
  checkMenuItem(ID_OPTIONS_HIDEWHENSETALARM,options.getHideWhenSetAlarm());
}

void CAlarmDlg::writeSetup() {
  Options options;
  options.setSoundFileName(m_soundFileName);
  options.setHideWhenSetAlarm(isMenuItemChecked(ID_OPTIONS_HIDEWHENSETALARM));
  options.save();
}

BOOL CAlarmDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);           // Set big icon
  SetIcon(m_hIcon, FALSE);      // Set small icon
  readSetup();
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_MAINDIALOG));
  m_hasStatusIcon = false;
  GetDlgItem(IDC_STATICSOUNDFILE)->SetWindowText(m_soundFileName);
  gotoField(IDC_EDITALARMTIME);
  startTimer(SHOWTIME_TIMER,1);
  return false;  // return TRUE  unless you set the focus to a control
}

void CAlarmDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else {
    __super::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAlarmDlg::OnPaint() {
  if (IsIconic()) {
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
  else {
    __super::OnPaint();
  }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAlarmDlg::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

BOOL CAlarmDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

bool CAlarmDlg::isMenuItemChecked(int id) const {
  return (GetMenu()->GetMenuState(id,MF_BYCOMMAND) & MF_CHECKED) ? true : false;
}

void CAlarmDlg::checkMenuItem(int id, bool checked) {
  GetMenu()->CheckMenuItem(id, checked ? MF_CHECKED : MF_UNCHECKED );
}

bool CAlarmDlg::toggleMenuItem(int id) {
  bool checked = !isMenuItemChecked(id);
  checkMenuItem(id,checked);
  return checked;
}

void CAlarmDlg::OnCancel() {
}

void CAlarmDlg::OnOK() {
}

void CAlarmDlg::OnFileExit() {
  deleteStatusIcon();
  exit(0);
}

void CAlarmDlg::OnMenuExit() {
  deleteStatusIcon();
  exit(0);
}

void CAlarmDlg::OnClose() {
  exit(0);
}

void CAlarmDlg::gotoField(int id) {
  CEdit *e = (CEdit*)GetDlgItem(id);
  e->SetSel(0,10);
  e->SetFocus();
}

BOOL MyTaskBarIcon(DWORD op, HWND hwnd, UINT uID, HICON hicon, LPTSTR lpszTip) {
  NOTIFYICONDATA tnid;
  tnid.cbSize           = sizeof(NOTIFYICONDATA);
  tnid.hWnd             = hwnd;
  tnid.uID              = uID;
  tnid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  tnid.uCallbackMessage = WM_LBUTTONDOWN;
  tnid.hIcon            = hicon;
  if(lpszTip) {
    lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
  } else {
    tnid.szTip[0] = '\0';
  }

  return Shell_NotifyIcon(op, &tnid);
}

BOOL MyTaskBarIconDelete(HWND hwnd, UINT uID) {
  NOTIFYICONDATA tnid;
  memset(&tnid,0,sizeof(tnid));
//    tnid.cbSize = sizeof(NOTIFYICONDATA);
  tnid.hWnd = hwnd;
  tnid.uID = uID;
  return Shell_NotifyIcon(NIM_DELETE, &tnid);
}

void CAlarmDlg::addStatusIcon() {
  m_hasStatusIcon = true;
  MyTaskBarIcon(NIM_ADD,m_hWnd,1,m_hIcon,_T("Alarm"));
}

void CAlarmDlg::deleteStatusIcon() {
  m_hasStatusIcon = false;
  MyTaskBarIconDelete(m_hWnd,1);
}

void CAlarmDlg::startTimer(unsigned int timerIndex, int waitSeconds) {
  if(timerIndex >= MAXTIMERCOUNT) {
    showWarning(_T("%s:Timerindex %d too big. Max=%d"), __TFUNCTION__, timerIndex, MAXTIMERCOUNT-1);
    return;
  }
  if(m_timerIsRunning[timerIndex]) {
    stopTimer(timerIndex);
  }
  if(SetTimer(timerIndex,1000 * waitSeconds,NULL)) {
    m_timerIsRunning[timerIndex] = true;
  } else {
    showWarning(_T("Cannot install timer"));
  }
}

void CAlarmDlg::stopTimer(unsigned int timerIndex) {
  if(timerIndex >= MAXTIMERCOUNT) {
    showWarning(_T("%s:Timerindex %d too big. Max=%d"),__TFUNCTION__,timerIndex, MAXTIMERCOUNT-1);
    return;
  }
  if(m_timerIsRunning[timerIndex]) {
    KillTimer(timerIndex);
    m_timerIsRunning[timerIndex] = false;
  }
}

void CAlarmDlg::OnGotoAlarmTime() {
  gotoField(IDC_EDITALARMTIME);
}

void CAlarmDlg::OnSetalarm() {
  UpdateData();
  const TCHAR *timestr = (LPCTSTR)m_time;
  int alarmhour,alarmminute,alarmsecond;
  if(_stscanf(timestr,_T("%d:%d:%d"),&alarmhour,&alarmminute,&alarmsecond) != 3) {
    alarmsecond = 0;
    if(_stscanf(timestr,_T("%d:%d"),&alarmhour,&alarmminute) != 2) {
      showWarning(_T("Illegal input. (hh:mm:[ss])"));
      return;
    }
  }
  if(alarmhour > 23 || alarmminute > 59 || alarmsecond > 59) {
    showWarning(_T("Illegal time"));
    return;
  }
  time_t now;
  time(&now);
  struct tm *tmnow = localtime(&now);
  int waitSeconds = ((alarmhour - tmnow->tm_hour) * 60 + alarmminute - tmnow->tm_min) * 60 + (alarmsecond - tmnow->tm_sec);
  if(alarmhour <= 3 && tmnow->tm_hour >= 22) {
    waitSeconds += 24*60*60; // past midnight
  }
  if(waitSeconds < 0) {
    waitSeconds = 1;
  }
  stopPlayer();
  startTimer(ALARM_TIMER,waitSeconds);
  if(isMenuItemChecked(ID_OPTIONS_HIDEWHENSETALARM)) {
    ShowWindow(SW_HIDE);
  }
  if(!m_hasStatusIcon) {
    addStatusIcon();
  }
}

void CAlarmDlg::showTime() {
  GetDlgItem(IDC_STATICCURRENTTIME)->SetWindowText(Timestamp::cctime().cstr());
}

void CAlarmDlg::OnTimer(UINT_PTR nIDEvent) {
  switch(nIDEvent) {
  case ALARM_TIMER:
    stopTimer(ALARM_TIMER);
    ShowWindow(SW_SHOW);
    BringWindowToTop();
    if(m_soundFileName == EMPTYSTRING) {
      showInformation(_T("Time is up!!!"));
    } else {
      startPlayer();
      showInformation(_T("Time is up!!!"));
      stopPlayer();
    }
    break;
  case SHOWTIME_TIMER:
    showTime();
    break;
  case PLAYSOUND_TIMER:
    if(m_player.getStatus() != playing) {
      stopPlayer();
    }
    break;
  }
  __super::OnTimer(nIDEvent);
}

static const TCHAR *FileDialogExtensions = _T("Sound-files (*.wav, *.mp3, *.wma)\0*.wav; *.mp3; *.wma\0All files (*.*)\0*.*\0\0");

void CAlarmDlg::OnOptionsSound() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = FileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Select sound");

  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  try {
    FILE *f = FOPEN(dlg.m_ofn.lpstrFile,_T("rb"));
    m_soundFileName = dlg.m_ofn.lpstrFile;
    fclose(f);
    writeSetup();
    GetDlgItem(IDC_STATICSOUNDFILE)->SetWindowText(m_soundFileName);
  } catch (Exception e) {
    showException(e);
  }

}

bool CAlarmDlg::startPlayer() {
  try {
    if(m_player.getStatus() != stopped) {
      m_player.stop();
    }
    m_player.play(m_soundFileName);
    startTimer(PLAYSOUND_TIMER,1);
    setWindowText(this, IDC_BUTTONPLAY, _T("S&top"));
    return true;
  } catch(Exception e) {
    showException(e);
    return false;
  }
}

void CAlarmDlg::stopPlayer() {
  try {
    stopTimer(PLAYSOUND_TIMER);
    setWindowText(this, IDC_BUTTONPLAY, _T("&Play"));

    if(m_player.getStatus() != stopped) {
      m_player.stop();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CAlarmDlg::OnButtonPlay() {
  if(m_player.getStatus() == stopped) {
    if(m_soundFileName == EMPTYSTRING) {
      showWarning(_T("No sound to play"));
    } else {
      startPlayer();
    }
  } else {
    stopPlayer();
  }
}

void CAlarmDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  const MSG *msg = GetCurrentMessage();
  if(msg->lParam & 0x1) {
    ShowWindow(SW_SHOW);
    BringWindowToTop();
  }
}

void CAlarmDlg::OnMenuOpen() {
  ShowWindow(SW_SHOW);
  BringWindowToTop();
}

void CAlarmDlg::OnHelpAboutalarm() {
  CAboutDlg().DoModal();
}

void CAlarmDlg::OnOptionsHidewhensetalarm() {
  bool checked = toggleMenuItem(ID_OPTIONS_HIDEWHENSETALARM);
  writeSetup();
}
