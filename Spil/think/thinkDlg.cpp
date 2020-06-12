#include "stdafx.h"
#include "think.h"
#include <MFCUtil/ColorSpace.h>
#include "thinkDlg.h"

#if defined(_DEBUG)
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
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CThinkDlg::CThinkDlg(CWnd *pParent) : CDialog(CThinkDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CThinkDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CThinkDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(ID_FILE_QUIT, OnFileQuit)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


BOOL CThinkDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE );
  SetIcon(m_hIcon, FALSE);
  randomize();
  g.init();

  return TRUE;
}

void CThinkDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CThinkDlg::drawLine(CClientDC &dc, int x1, int y1, int x2, int y2) {
  CPen mypen;
  mypen.CreatePen(PS_SOLID,1,BLACK);
  CPen *origpen = dc.SelectObject(&mypen);
  dc.MoveTo(x1,y1);
  dc.LineTo(x2,y2);
  dc.SelectObject(origpen);
}

void CThinkDlg::markField(int r, int c, bool current) {
  static HBRUSH redbrush   = CreateSolidBrush(RED);
  static HBRUSH whitebrush = CreateSolidBrush(WHITE);
  CRect rect = getRect(r,c);
  CClientDC dc(this);

  CBrush *pTempBrush = (CBrush*)dc.SelectObject(current ? redbrush : whitebrush);
  CBrush OrigBrush;
  OrigBrush.FromHandle((HBRUSH)pTempBrush);

  dc.Rectangle(rect.left,rect.top,rect.right+1,rect.bottom+1);
  dc.SelectObject(&OrigBrush);
}

CRect CThinkDlg::getRect(int r, int c) const {
  const CRect crect = getClientRect(this);
  const int   w     = crect.Width();
  const int   h     = crect.Height();
#define TOPMARGIN    60
#define LEFTMARGIN   10
#define BOTTOMMARGIN 20
#define RIGHTMARGIN  20
  CRect result;
  result.top    =  r*(h-(TOPMARGIN+BOTTOMMARGIN))/BSIZE+TOPMARGIN;
  result.left   =  c*(w-(LEFTMARGIN+RIGHTMARGIN))/BSIZE+LEFTMARGIN;
  result.bottom = result.top + (h-(TOPMARGIN+BOTTOMMARGIN))/BSIZE;
  result.right  = result.left+ (w-(LEFTMARGIN+RIGHTMARGIN))/BSIZE;
  return result;
}

bool CThinkDlg::findField(const CPoint &point, int &r, int &c) const {
  for(r = 0; r < BSIZE; r++) {
    for(c = 0; c < BSIZE; c++) {
      if(getRect(r,c).PtInRect(point)) {
        return true;
      }
    }
  }
  return false;
}

void CThinkDlg::drawBoard() {
  const CRect crect = getClientRect(this);
  const CRect rect1 = getRect(0,0);
  const CRect rect2 = getRect(BSIZE,BSIZE);
  const int   w     = crect.Width();
  const int   h     = crect.Height();
  CClientDC dc(this);
  dc.SetBkColor(GREY);
  for(int r = 0; r <= BSIZE; r++) {
    const CRect rect = getRect(r,0);
    drawLine(dc,rect1.left,rect.top,rect2.left,rect.top);
  }
  for(int c = 0; c <= BSIZE; c++) {
    const CRect rect = getRect(0,c);
    drawLine(dc,rect.left,rect1.top,rect.left,rect2.top);
  }
  for(int r = 0; r < BSIZE; r++) {
    for(int c = 0; c < BSIZE; c++) {
      const CRect rect = getRect(r,c);
      if(!g.m_used[r][c]) {
        textOut(dc, rect.left+rect.Width()/2-7,rect.top+rect.Height()/2-7,format(_T("%2d"),g.m_board[r][c]));
      } else if(r==g.m_currentRow && c == g.m_currentCol) {
        markField(r,c,true);
      } else {
        markField(r,c,false);
      }
    }
  }
  textOut(dc,10,10,format(_T("Min sum:%2d  "), g.m_mySum  ));
  textOut(dc,10,30,format(_T("Din sum:%2d  "), g.m_yourSum));
}

void CThinkDlg::OnPaint()  {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width()  - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
    drawBoard();
  }
}

HCURSOR CThinkDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CThinkDlg::OnFileQuit()  {
  EndDialog(IDOK);
}

void CThinkDlg::showWinner() {
  String str;
  if(g.m_mySum > g.m_yourSum) {
    str = _T("Jeg vandt");
  } else if(g.m_yourSum > g.m_mySum) {
    str = _T("Du vinder");
  } else {
    str = _T("Uafgjort");
  }
  showInformation(str);
  g.init();
  Invalidate(true);
}

void CThinkDlg::OnLButtonDown(UINT nFlags, CPoint point)  {
  __super::OnLButtonDown(nFlags, point);
  int r, c;
  if(g.m_playerInTurn == YOU
    && findField(point,r,c)
    && r == g.m_currentRow
    && !g.m_used[r][c]) {
    g.executeMove(c);
    Invalidate(false);
    if(!g.isGameOver()) {
//        Sleep(1000);
      r = g.findMoveR();
      g.executeMove(r);
      Invalidate(false);
      if(g.isGameOver()) {
        showWinner();
      }
    } else {
      showWinner();
    }
  }
}
