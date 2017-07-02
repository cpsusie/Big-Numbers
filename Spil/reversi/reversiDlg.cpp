#include "stdafx.h"
#include <MFCUtil/ColorSpace.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
    CAboutDlg();

    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CReversiDlg::CReversiDlg(CWnd *pParent /*=NULL*/) : CDialog(CReversiDlg::IDD, pParent) {
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CReversiDlg::OnHjlpAboutreversi() {
  CAboutDlg dlg;
  dlg.DoModal();
}


void CReversiDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CReversiDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_TIMER()
    ON_COMMAND(ID_SPIL_QUIT, OnSpilQuit)
    ON_WM_MOUSEMOVE()
    ON_COMMAND(ID_HJLP_ABOUTREVERSI, OnHjlpAboutreversi)
    ON_COMMAND(ID_OPTIONS_VISSCORE, OnOptionsVisScore)
    ON_COMMAND(ID_SPIL_NYTSPIL, OnSpilNytspil)
END_MESSAGE_MAP()

BOOL CReversiDlg::OnInitDialog() {
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
  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_timerIsRunning = false;
  m_whoStarts      = YOU;
  m_score          = 0;
  startGame();

  m_currentIsSystemCursor = true;
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_BOARDFRAME, RELATIVE_SIZE );
  m_layoutManager.addControl(IDC_SCORE     , RELATIVE_Y_POS);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CReversiDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

BOOL CReversiDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg))
    return true;
  return __super::PreTranslateMessage(pMsg);
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.

void CReversiDlg::drawLine(CDC &dc, int x1, int y1, int x2, int y2) {
  CPen mypen,*origpen;
  mypen.CreatePen(PS_SOLID,1,BLACK);
  origpen = dc.SelectObject(&mypen);
  dc.MoveTo(x1,y1);
  dc.LineTo(x2,y2);
  dc.SelectObject(origpen);
}

CRect CReversiDlg::getFieldRect(int r, int c) {
  CWnd *board = GetDlgItem(IDC_BOARDFRAME);
  CRect crect;
  board->GetClientRect(&crect);
  int w = crect.Width();
  int h = crect.Height();
  CRect result;
  result.top    = r * (h-20)/BSIZE + 10;
  result.left   = c * (w-20)/BSIZE + 10;
  result.bottom = result.top  + (h-20)/BSIZE;
  result.right  = result.left + (w-20)/BSIZE;
  return result;
}

void CReversiDlg::markField(int r, int c, int who) {
  if(who == 0) {
    return;
  }
  static HBRUSH redbrush  = CreateSolidBrush(RED);
  static HBRUSH bluebrush = CreateSolidBrush(BLUE);
  CRect rect = getFieldRect(r,c);
  CClientDC dc(GetDlgItem(IDC_BOARDFRAME));

  CBrush* pTempBrush = (CBrush*)dc.SelectObject(who == ME ? redbrush : bluebrush);
  CBrush OrigBrush;
  OrigBrush.FromHandle((HBRUSH)pTempBrush);

  dc.Ellipse(rect);
  dc.SelectObject(&OrigBrush);
}

int CReversiDlg::findField(CPoint &point) {
  for(int r = 0; r < BSIZE; r++) {
    for(int c = 0; c < BSIZE; c++) {
      if(getFieldRect(r,c).PtInRect(point))
        return GameState::findField(r,c);
    }
  }
  return -1;
}

void CReversiDlg::OnPaint() {
  if(IsIconic()) {
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

  } else {

    __super::OnPaint();
    CWnd *board = GetDlgItem(IDC_BOARDFRAME);
    CRect crect;
    board->GetClientRect(&crect);
    int w = crect.Width();
    int h = crect.Height();
    CClientDC dc(board);
    CRect rect1 = getFieldRect(0,0);
    CRect rect2 = getFieldRect(BSIZE,BSIZE);
    for(int r = 0; r <= BSIZE; r++) {
      CRect rect = getFieldRect(r,0);
      drawLine(dc,rect1.left,rect.top,rect2.left,rect.top);
    }
    for(int c = 0; c <= BSIZE; c++) {
      CRect rect = getFieldRect(0,c);
      drawLine(dc,rect.left,rect1.top,rect.left,rect2.top);
    }

    for(int r = 0; r < BSIZE; r++) {
      for(int c = 0; c < BSIZE; c++) {
        markField(r,c,m_game.getField(r,c));
      }
    }
  }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CReversiDlg::OnQueryDragIcon() {
    return (HCURSOR)m_hIcon;
}

void CReversiDlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,1000,NULL)) {
    m_timerIsRunning = true;
  }
}

void CReversiDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

bool CReversiDlg::computerMustPass() {
  MoveEvaluater moveEvaluater;
  return moveEvaluater.findmove(ME,1,m_game) == 0;
}

bool CReversiDlg::userMustPass() {
  MoveEvaluater moveEvaluater;
  return moveEvaluater.findmove(YOU,1,m_game) == 0;
}

void CReversiDlg::startGame() {
  m_computerTurn = m_whoStarts == ME;
  m_game.init();
  Invalidate(TRUE);
  if(m_computerTurn) {
    startTimer();
  }
}

bool CReversiDlg::gameOver() {
  return computerMustPass() && userMustPass();
}

void CReversiDlg::showWinner() {
  int me,you;
  const TCHAR *s;
  m_game.getBoardCount(me,you);
  if(me > you) {
    s = _T("I win");
  } else if(me < you) {
    s = _T("You win");
  } else {
    s = _T("Draw");
  }

  MessageBox(format(_T("Me:%d You %d. %s"),me,you,s).cstr(),_T("Game over"),MB_ICONINFORMATION);
  m_whoStarts = -m_whoStarts;
  startGame();
}

void CReversiDlg::computerMove() {
  MoveEvaluater moveEvaluater;
  stopTimer();
  int f = moveEvaluater.findmove(ME,6,m_game);
  if(f > 0) {
    m_game.executeMove(ME,f);
    m_score = moveEvaluater.getScore();
    setWindowText(this, IDC_SCORE, format(_T("Score:%d"),m_score));

    Invalidate(FALSE);
    if(gameOver()) {
      showWinner();
    } else
      if(userMustPass()) {
        MessageBox(_T("You must pass"), EMPTYSTRING, MB_ICONINFORMATION);
        startTimer();
      } else {
        m_computerTurn = false;
      }
  }
}

void CReversiDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  if(m_computerTurn)  {
    return;
  }

  int f = findField(point);
  if(f > 0) {
    if(m_game.executeMove(YOU,f)) {
      Invalidate(FALSE);
      if(gameOver()) {
        showWinner();
      } else {
        if(computerMustPass()) {
          MessageBox(_T("I must pass"),EMPTYSTRING, MB_ICONINFORMATION);
          return;
        } else {
          m_computerTurn = true;
          startTimer();
        }
      }
    }
  }
  __super::OnLButtonDown(nFlags, point);
}

void CReversiDlg::OnTimer(UINT_PTR nIDEvent) {
  computerMove();   
  __super::OnTimer(nIDEvent);
}

void CReversiDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType, cx, cy);
  Invalidate(TRUE);
}

void CReversiDlg::OnSpilNytspil() {
  m_whoStarts = -m_whoStarts;
  startGame();
}

void CReversiDlg::OnSpilQuit() {
  exit(0);  
}

void CReversiDlg::OnOptionsVisScore() {
  GetDlgItem(IDC_SCORE)->ShowWindow(toggleMenuItem(this,ID_OPTIONS_VISSCORE) ? SW_SHOW : SW_HIDE);
}

void CReversiDlg::OnMouseMove(UINT nFlags, CPoint point) {
  ClientToScreen(&point);
  GetDlgItem(IDC_BOARDFRAME)->ScreenToClient(&point);

  int f = findField(point);
  Move m;
  if(f > 0 && m_game.checkMove(YOU, f, m)) {
    if(m_currentIsSystemCursor) {
      setWindowCursor(this,MAKEINTRESOURCE(OCR_HAND));
    }
    m_currentIsSystemCursor = false;
  } else {
    if(!m_currentIsSystemCursor) {
      setWindowCursor(this,MAKEINTRESOURCE(OCR_NO));
    }
    m_currentIsSystemCursor = true;
  }

  __super::OnMouseMove(nFlags, point);
}
