#include "stdafx.h"
#include <Random.h>
#include <Math.h>
#include "RegneTester.h"
#include "RegneTesterDlg.h"
#include "NameDlg.h"
#include "HighScoredlg.h"
#include "PasswordDlg.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CRegneTesterDlg::CRegneTesterDlg(CWnd *pParent /*=NULL*/) : CDialog(CRegneTesterDlg::IDD, pParent) {
  randomize();
  m_result = EMPTYSTRING;
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
  m_timerIsRunning = false;
  m_answerCount    = 0;
  m_watchTime      = -10000;
}

void CRegneTesterDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITRESULT, m_result);
    DDV_MaxChars(pDX, m_result, 4);
}

BEGIN_MESSAGE_MAP(CRegneTesterDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_COMMAND(ID_FILE_HIGHSCORE        , OnFileHighscore        )
    ON_COMMAND(ID_FILE_INDSTILLINGER    , OnFileIndstillinger    )
    ON_COMMAND(ID_FILE_AFSLUT           , OnFileAfslut           )
    ON_COMMAND(ID_TABEL_1TABEL          , OnTabel1tabel          )
    ON_COMMAND(ID_TABEL_2TABEL          , OnTabel2tabel          )
    ON_COMMAND(ID_TABEL_3TABEL          , OnTabel3tabel          )
    ON_COMMAND(ID_TABEL_4TABEL          , OnTabel4tabel          )
    ON_COMMAND(ID_TABEL_5TABEL          , OnTabel5tabel          )
    ON_COMMAND(ID_TABEL_6TABEL          , OnTabel6tabel          )
    ON_COMMAND(ID_TABEL_7TABEL          , OnTabel7tabel          )
    ON_COMMAND(ID_TABEL_8TABEL          , OnTabel8tabel          )
    ON_COMMAND(ID_TABEL_9TABEL          , OnTabel9tabel          )
    ON_COMMAND(ID_TABEL_10TABEL         , OnTabel10tabel         )
    ON_COMMAND(ID_HELP_ABOUTREGNETESTER , OnHelpAboutRegneTester )
    ON_COMMAND(ID_OPGAVETYPE_ADD        , OnOpgavetypeAdd        )
    ON_COMMAND(ID_OPGAVETYPE_SUBTRACT   , OnOpgavetypeSubtract   )
    ON_COMMAND(ID_OPGAVETYPE_MULTIPLY   , OnOpgavetypeMultiply   )
    ON_COMMAND(ID_OPGAVETYPE_DIVIDE     , OnOpgavetypeDivide     )
    ON_BN_CLICKED(IDC_BUTTONSTART       , OnBnClickedButtonStart )
END_MESSAGE_MAP()

//    ON_MESSAGE(ID_MSG_DRAWWATCH         , OnMsgDrawWatch         )

BOOL CRegneTesterDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  SetIcon(m_hIcon, TRUE );
  SetIcon(m_hIcon, FALSE);

  m_watchBitmap.LoadBitmap(IDB_BITMAPTIMEWATCH);
  m_watchDC.CreateCompatibleDC(NULL);
  m_watchWorkDC.CreateCompatibleDC(NULL);

  m_watchBitmap.GetObject(sizeof(BITMAP), &m_watchInfo);
  m_watchWorkBitmap.CreateBitmap(m_watchInfo.bmWidth, m_watchInfo.bmHeight, m_watchInfo.bmPlanes, m_watchInfo.bmBitsPixel, NULL);
  m_watchWorkDC.SelectObject(&m_watchWorkBitmap);
  setEinsteinBitmap();
  setTableNumber(randInt(1,10));
  OnOpgavetypeMultiply();
  
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRegneTesterDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

void CRegneTesterDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialog::OnPaint();
    if(m_timerIsRunning) {
      drawWatch(CPaintDC(GetDlgItem(IDC_STATEIMAGEWATCH)), m_watchTime);
    }
  }
}

HCURSOR CRegneTesterDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CRegneTesterDlg::OnFileHighscore() {
  CHighScoreDlg(m_execiseType).DoModal();
}

void CRegneTesterDlg::OnFileIndstillinger() {
  stopExecise(false);
  CPasswordDlg passwordDlg(m_options.m_password);
  if(passwordDlg.DoModal() == IDOK) {
    COptionsDlg optionsDlg(m_options);
    optionsDlg.DoModal();
  }
}

void CRegneTesterDlg::OnFileAfslut() {
#ifndef _DEBUG
  if(m_options.m_password.length() == 0) {
    if(MessageBox(_T("Ingen indstillinger gemt"), _T("Advarsel"), MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL) {
      return;
    }
  }
#endif

  EndDialog(IDOK);
}

void CRegneTesterDlg::OnBnClickedButtonStart() {
  startExecise();
}

void CRegneTesterDlg::OnCancel() {
}

void CRegneTesterDlg::OnClose() {
  OnFileAfslut();
}

void CRegneTesterDlg::OnHelpAboutRegneTester() {
  CAboutDlg().DoModal();
}

void CRegneTesterDlg::generateExecise() {
  const int selectedNumber = (int)m_numbersLeft.select();
  m_numbersLeft.remove(selectedNumber);

  int leftOperand, rightOperand;

  switch(m_execiseType) {
  case ID_OPGAVETYPE_ADD      :
  case ID_OPGAVETYPE_SUBTRACT :
  case ID_OPGAVETYPE_MULTIPLY :
    if(randInt() % 2) {
      leftOperand  = selectedNumber;
      rightOperand = m_tableNumber;
    } else {
      rightOperand = selectedNumber;
      leftOperand  = m_tableNumber;
    }
    switch(m_execiseType) {
    case ID_OPGAVETYPE_ADD      : m_expectedAnswer = leftOperand + rightOperand; break;
    case ID_OPGAVETYPE_SUBTRACT : m_expectedAnswer = leftOperand - rightOperand; break;
    case ID_OPGAVETYPE_MULTIPLY : m_expectedAnswer = leftOperand * rightOperand; break;
    }
    break;

  case ID_OPGAVETYPE_DIVIDE   :
    leftOperand      = selectedNumber * m_tableNumber;
    rightOperand     = m_tableNumber;
    m_expectedAnswer = selectedNumber;
    break;
  }

  setWindowText(this, IDC_LEFTOPERAND , format(_T("%d"), leftOperand ));
  setWindowText(this, IDC_RIGHTOPERAND, format(_T("%d"), rightOperand));

  setWindowText(this, IDC_EDITRESULT, EMPTYSTRING);
  GetDlgItem(IDC_EDITRESULT)->SetFocus();
}

void CRegneTesterDlg::setAnswerCount(int value) {
  m_answerCount = value;
  setWindowText(this, IDC_ANSWERCOUNT, format(_T("%d"), m_answerCount));
}

void CRegneTesterDlg::resetExecise() {
  stopExecise(false);
  setWindowText(this, IDC_ANSWERCOUNT, EMPTYSTRING);
  setWindowText(this, IDC_TIME       , EMPTYSTRING);
}

void CRegneTesterDlg::startExecise() {
  m_numbersLeft.addAll();
  setAnswerCount(0);
  GetDlgItem(IDC_EDITRESULT)->EnableWindow(TRUE);
  generateExecise();
  startTimer();
  ajourStartButtonVisible();
}

void CRegneTesterDlg::stopExecise(bool checkHighScore) {
  stopTimer();
  setWindowText(this, IDC_LEFTOPERAND  , EMPTYSTRING);
  setWindowText(this, IDC_RIGHTOPERAND , EMPTYSTRING);
  setWindowText(this, IDC_EDITRESULT   , EMPTYSTRING);
  GetDlgItem(IDC_EDITRESULT)->EnableWindow(FALSE);

  if(checkHighScore) {
    if(HighScore().isBetterTime(m_execiseType, m_tableNumber, m_usedTime)) {
      CNameDlg dlg;
      if(dlg.DoModal() == IDOK) {
        HighScore::update(m_execiseType, m_tableNumber, dlg.getName(), m_usedTime);
      }
    }
  }
  ajourStartButtonVisible();
}

void CRegneTesterDlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,100,NULL)) {
    m_timerIsRunning = true;
    m_startTime      = Timestamp();
    m_watchTime      = 0;
    m_usedTime       = 0;
    setWatchBitmap();
  }
}

void CRegneTesterDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
    setEinsteinBitmap();
  }
}

void CRegneTesterDlg::OnTimer(UINT_PTR nIDEvent) {
  updateTime();
  CDialog::OnTimer(nIDEvent);
}

void CRegneTesterDlg::updateTime() {
  m_usedTime = diff(m_startTime, Timestamp(), TMILLISECOND);
  updateWatch();
  setWindowText(this, IDC_TIME, secondsToString(m_usedTime));
}

void CRegneTesterDlg::updateWatch() {
  const int t = (int)m_usedTime / 1000;
  if(t != m_watchTime) {
    drawWatch(CClientDC(GetDlgItem(IDC_STATEIMAGEWATCH)), t);
  }
}
/*
LRESULT CRegneTesterDlg::OnMsgDrawWatch(WPARAM wp, LPARAM lp) {
  drawWatch((int)wp);
  return 0;
}
*/

void CRegneTesterDlg::setWatchBitmap() {
  GetDlgItem(IDC_STATEIMAGEWATCH   )->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATEIMAGEEINSTEIN)->ShowWindow(SW_HIDE);
  Invalidate();
//  PostMessage(ID_MSG_DRAWWATCH, 0);
}

void CRegneTesterDlg::setEinsteinBitmap() {
  GetDlgItem(IDC_STATEIMAGEEINSTEIN)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATEIMAGEWATCH   )->ShowWindow(SW_HIDE);
}

void CRegneTesterDlg::drawWatch(CDC &dc, int t) { // t in seconds
  CGdiObject *oldBitmap = m_watchDC.SelectObject(&m_watchBitmap);
  m_watchWorkDC.BitBlt(0,0,m_watchInfo.bmWidth, m_watchInfo.bmHeight, &m_watchDC, 0, 0, SRCCOPY);
  m_watchDC.SelectObject(oldBitmap);

  const double angleSecond = ((double)t/60  ) * 2 * M_PI;
  const double angleMinute = ((double)t/3600) * 2 * M_PI;

#define SECOND_CENTERX  62
#define SECOND_CENTERY 102
#define SECOND_LENGTH   50
#define MINUTE_CENTERX  62
#define MINUTE_CENTERY  81
#define MINUTE_LENGTH   12

  m_watchWorkDC.MoveTo(SECOND_CENTERX, SECOND_CENTERY);
  m_watchWorkDC.LineTo(SECOND_CENTERX + (int)round((sin(angleSecond)*SECOND_LENGTH)), SECOND_CENTERY - (int)round((cos(angleSecond)*SECOND_LENGTH)));
  m_watchWorkDC.MoveTo(MINUTE_CENTERX, MINUTE_CENTERY);
  m_watchWorkDC.LineTo(MINUTE_CENTERX + (int)round((sin(angleMinute)*MINUTE_LENGTH)), MINUTE_CENTERY - (int)round((cos(angleMinute)*MINUTE_LENGTH)));
  dc.BitBlt(0,0,m_watchInfo.bmWidth, m_watchInfo.bmHeight, &m_watchWorkDC, 0,0, SRCCOPY);
  m_watchTime = t;
}

void CRegneTesterDlg::OnOK() {
  String s = getWindowText(this, IDC_EDITRESULT);
  int answer;
  if(_stscanf(s.cstr(),_T("%d"), &answer) != 1) {
    setWindowText(this, IDC_EDITRESULT, EMPTYSTRING);
    return;
  }
  if(answer != m_expectedAnswer) {
    setWindowText(this, IDC_EDITRESULT, EMPTYSTRING);
  } else {
    setAnswerCount(m_answerCount + 1);
    if(!m_numbersLeft.isEmpty()) {
      generateExecise();
    } else {
      stopExecise(true);
    }
  }
}

BOOL CRegneTesterDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CRegneTesterDlg::OnTabel1tabel()  {  setTableNumber( 1); }
void CRegneTesterDlg::OnTabel2tabel()  {  setTableNumber( 2); }
void CRegneTesterDlg::OnTabel3tabel()  {  setTableNumber( 3); }
void CRegneTesterDlg::OnTabel4tabel()  {  setTableNumber( 4); }
void CRegneTesterDlg::OnTabel5tabel()  {  setTableNumber( 5); }
void CRegneTesterDlg::OnTabel6tabel()  {  setTableNumber( 6); }
void CRegneTesterDlg::OnTabel7tabel()  {  setTableNumber( 7); }
void CRegneTesterDlg::OnTabel8tabel()  {  setTableNumber( 8); }
void CRegneTesterDlg::OnTabel9tabel()  {  setTableNumber( 9); }
void CRegneTesterDlg::OnTabel10tabel() {  setTableNumber(10); }

void CRegneTesterDlg::setTableNumber(int tableNumber) {
  static const int tableMenuItem[] = {
    0
   ,ID_TABEL_1TABEL,    ID_TABEL_2TABEL 
   ,ID_TABEL_3TABEL,    ID_TABEL_4TABEL 
   ,ID_TABEL_5TABEL,    ID_TABEL_6TABEL 
   ,ID_TABEL_7TABEL,    ID_TABEL_8TABEL 
   ,ID_TABEL_9TABEL,    ID_TABEL_10TABEL
  };
  for (int i = 1; i < ARRAYSIZE(tableMenuItem); i++) {
    checkMenuItem(this, tableMenuItem[i], i==tableNumber);
  }
  m_tableNumber = tableNumber;
  resetExecise();
}

void CRegneTesterDlg::OnOpgavetypeAdd()      { setExeciseType(ID_OPGAVETYPE_ADD     ); }
void CRegneTesterDlg::OnOpgavetypeSubtract() { setExeciseType(ID_OPGAVETYPE_SUBTRACT); }
void CRegneTesterDlg::OnOpgavetypeMultiply() { setExeciseType(ID_OPGAVETYPE_MULTIPLY); }
void CRegneTesterDlg::OnOpgavetypeDivide()   { setExeciseType(ID_OPGAVETYPE_DIVIDE  ); }

void CRegneTesterDlg::setExeciseType(int type) {
  checkMenuItem(this, ID_OPGAVETYPE_ADD      , false); 
  checkMenuItem(this, ID_OPGAVETYPE_SUBTRACT , false);
  checkMenuItem(this, ID_OPGAVETYPE_MULTIPLY , false);
  checkMenuItem(this, ID_OPGAVETYPE_DIVIDE   , false);

  checkMenuItem(this, type, true); 
  m_execiseType = type;
  setWindowText(this, IDC_STATIC_OPERATOR, format(_T("%c"), getOperatorChar(type)));

  resetExecise();
}

void CRegneTesterDlg::ajourStartButtonVisible() {
  const bool visible = !m_timerIsRunning 
                    && isValidExeciseType(m_execiseType)
                    && isValidTableNumber(m_tableNumber);
  GetDlgItem(IDC_BUTTONSTART)->ShowWindow(visible?SW_SHOW:SW_HIDE);
}
