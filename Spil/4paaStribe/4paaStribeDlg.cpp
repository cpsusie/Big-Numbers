#include "stdafx.h"
#include "4PaaStribe.h"
#include "4PaaStribeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BACKCOLOR RGB(0  ,255,0  )
#define RED       RGB(255,0  ,0  )
#define BLUE      RGB(0  ,0  ,255)
#define BLACK     RGB(0  ,0  ,0  )
#define PLAYER_COLOR(who) ((who) == ME ? RED : BLUE)

#define LEFTMARG 0
#define TOPMARG  50
#define RIGHTMARG 0
#define BOTTOMMARG 0

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

void C4PaaStribeDlg::OnHelpAbout() {
  CAboutDlg dlg;
  dlg.DoModal();
}

C4PaaStribeDlg::C4PaaStribeDlg(CWnd *pParent)
    : CDialog(C4PaaStribeDlg::IDD, pParent) {
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void C4PaaStribeDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C4PaaStribeDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(ID_FILE_AFSLUT, OnFileAfslut)
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_COMMAND(ID_FILE_NYT_SPIL, OnFileNytSpil)
    ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
    ON_COMMAND(ID_OPTIONS_ANIMATION, OnOptionsAnimation)
    ON_COMMAND(ID_OPTIONS_VISEVALUERINGSFUNKTION, OnOptionsVisevalueringsfunktion)
    ON_COMMAND(ID_OPTIONS_BEGYNDER, OnOptionsBegynder)
    ON_COMMAND(ID_OPTIONS_EXPERT, OnOptionsExpert)
    ON_COMMAND(ID_OPTIONS_VISFELTVRDIER, OnOptionsVisfeltvrdier)
END_MESSAGE_MAP()

BOOL C4PaaStribeDlg::OnInitDialog() {
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

    SetIcon(m_hIcon, TRUE );
    SetIcon(m_hIcon, FALSE);

    randomize();
    m_starter = YOU;
    m_game.init(m_starter);

/*
    showInformation(_T("sizeof(Game):%d sizeof(Position):%d, sizeof(Stribe4):%d",sizeof(Game),sizeof(Position),sizeof(Stribe4)));
*/
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void C4PaaStribeDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

CRect C4PaaStribeDlg::getHoleRect(int r, int c) {
  CRect clientrect;
  GetClientRect(&clientrect);
  int w   = clientrect.Width()  - RIGHTMARG - LEFTMARG;
  int h   = clientrect.Height() - TOPMARG   - BOTTOMMARG;
  int hsx = w / BOARDWIDTH  - 3;
  int hsy = h / BOARDHEIGHT - 3;
  int hs  = min(hsx,hsy);
  int cx  = LEFTMARG + c * w / BOARDWIDTH  + 2;
  int cy  = TOPMARG  + (BOARDHEIGHT-r-1) * h / BOARDHEIGHT + 2;
  return CRect(cx,cy,cx + hs, cy + hs);
}

void C4PaaStribeDlg::paintHole(int r, int c, COLORREF color) {
  CClientDC dc(this);
  dc.SelectObject(CreateSolidBrush(color));
  dc.Ellipse(&getHoleRect(r,c));
}

void C4PaaStribeDlg::animateMove(Move m, Player who) {
  int c = m;
  for(int r = BOARDHEIGHT - 1; r > m_game.colh(c); r--) {
    paintHole(r,m,PLAYER_COLOR(who));
    Sleep(40);
    paintHole(r,c,BLACK);
  }
}

void C4PaaStribeDlg::paintFieldValues() {
  PosMatrix values_me,values_you;

  m_game.findPosValue( values_me,  ME );
  m_game.findPosValue( values_you, YOU);

  CClientDC dc(this);
  for(int r = 0; r < BOARDHEIGHT;r++) {
    for(int c = 0; c < BOARDWIDTH; c++) {
      CRect rect = getHoleRect(r,c);
      int textx = (rect.left + rect.right)/2 - 8;
      int texty = (rect.top  + rect.bottom)/2;

      dc.SetTextColor(PLAYER_COLOR(ME));
      textOut(dc, textx,texty-14, _T("  "));
      textOut(dc, textx,texty-14, format(_T("%2d"), values_me[r][c]));
      dc.SetTextColor(PLAYER_COLOR(YOU));
      textOut(dc, textx, texty, _T("  "));
      textOut(dc, textx, texty, format(_T("%2d"), values_you[r][c]));
    }
  }
}

void C4PaaStribeDlg::paintGame() {
  CClientDC dc(this);
  CRect rect;
  GetClientRect(&rect);

  dc.SelectObject(CreateSolidBrush(BACKCOLOR));
  dc.Rectangle(LEFTMARG,TOPMARG,rect.Width() - RIGHTMARG, rect.Height() - BOTTOMMARG);
  for(int r = 0; r < BOARDHEIGHT; r++) {
    for(int c = 0; c < BOARDWIDTH; c++) {
      int br = m_game.pos(r,c);
      if(br) {
        paintHole(r,c,PLAYER_COLOR(br));
      } else {
        paintHole(r,c,BLACK);
      }
    }
  }
  if(isMenuItemChecked(this, ID_OPTIONS_VISFELTVRDIER)) {
    paintFieldValues();
  }
}

int C4PaaStribeDlg::point2column(CPoint point) {
  for(int c = 0; c < BOARDWIDTH; c++) {
    CRect rect = getHoleRect(0,c);
    if(point.x >= rect.left && point.x <= rect.right)
      return c;
  }
  return -1;
}

void C4PaaStribeDlg::paintMessage() {
  CClientDC dc(this);
  dc.SetBkColor(RGB(128,128,200));
  textOut(dc,10,10, format(_T("%*.*s"),113,113,_T(" ")));
  textOut(dc, 10,10,messageString);
}

void C4PaaStribeDlg::OnPaint()  {
  if(IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    paintGame();
    paintMessage();
    __super::OnPaint();
  }
}

HCURSOR C4PaaStribeDlg::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

void C4PaaStribeDlg::OnFileAfslut()  {
  exit(0);
}

static TCHAR *winningstring[] = {
  _T("Nu går det vist galt for dig")
 ,_T("Æv bæv, du taber")
 ,_T("Du har ikke langt igen")
 ,_T("Du er på vej mod afgrunden")
 ,_T("Du kan lige så godt opgive")
 ,_T("Nu er det kun et spørgsmål om tid")
 ,_T("Din eneste chance er at hive stikket ud")
 ,_T("Kan du mærke løkken strammes")
 ,_T("He he")
};

static TCHAR *winnerstring[] = {
  _T("Hvad sagde jeg ?")
 ,_T("Hvem er så bedst, mennesket eller computeren ?")
 ,_T("Mon ikke du skulle gå hjem og øve dig lidt")
 ,_T("Prøv igen")
 ,_T("Du skal bare spille lige som mig")
 ,_T("Du har chancen i næste spil")
 ,_T("Tab og vind...")
 ,_T("Een skal jo tabe")
};

static TCHAR *looserstring[] = {
  _T("Der var du vist heldig")
 ,_T("Ok - du vandt, men du var heldig")
 ,_T("Hør, her er vist noget galt")
 ,_T("Ok - jeg lod dig vinde")
 ,_T("Du har vist smugtrænet")
 ,_T("Een skal jo vinde")
 ,_T("Sig mig, har du øvet dig ?")
};

#define randomstring(sarray) sarray[randInt(ARRAYSIZE(sarray))]


void C4PaaStribeDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  paintGame();
}

void C4PaaStribeDlg::showMessage(TCHAR *format, ... ) {
  va_list argptr;
  va_start(argptr,format);
  messageString = vformat(format,argptr);
  va_end(argptr);
  paintMessage();
}

void C4PaaStribeDlg::newGame() {
  m_starter = OPPONENT(m_starter);
  m_game.init(m_starter);
  showMessage(EMPTYSTRING);
  Invalidate(FALSE);
  if(m_game.playerInTurn() == ME) {
    executeMachineMove();
  }
}

void C4PaaStribeDlg::markStribe(Move lastmove) {
  Stribe4 holes = m_game.findStribe(lastmove);
  for(int s = 0; s < 3; s++ ) {
    for(int i=0;i<4;i++) {
      paintHole(holes.p[i],BLACK);
    }
    Sleep(200);
    for(int i=0;i<4;i++) {
      paintHole(holes.p[i],PLAYER_COLOR(m_game.winner()));
    }
    Sleep(200);
  }
}

void C4PaaStribeDlg::checkgameover(Move lastmove) {
  if(m_game.gameOver() || m_game.getBrickCount() == BOARDHEIGHT * BOARDWIDTH) {
    TCHAR *s;
    switch(m_game.winner()) {
    case YOU: s = randomstring(looserstring);
              markStribe(lastmove);
              break;
    case ME:  s = randomstring(winnerstring);
              markStribe(lastmove);
              break;
    default:  s = _T("Uafgjort");
              break;
    }
    const String msg = format(_T("%s\n\nVil du spille igen ?"),s);
    switch(MessageBox(msg.cstr(), _T("Nyt spil"), MB_YESNO)) {
    case IDYES:
      newGame();
      break;
    case IDNO:
      exit(0);
    }
  }
}

void C4PaaStribeDlg::executeMove(Move m, Player who) {
  if(isMenuItemChecked(this, ID_OPTIONS_ANIMATION)) {
    animateMove(m,who);
  }
  paintHole(m_game.colh(m),m,PLAYER_COLOR(who));
  m_game.doMove(m);
  if(isMenuItemChecked(this, ID_OPTIONS_VISFELTVRDIER)) {
    paintFieldValues();
  }
}

Move C4PaaStribeDlg::executeMachineMove() {
  int br = m_game.getBrickCount();
  String scoreMsg, winnerMsg = EMPTYSTRING;
  int lookahead;
  if(isMenuItemChecked(this, ID_OPTIONS_BEGYNDER)) {
    lookahead = 6;
  } else {
    lookahead = br >= 14 ? 12 : br >= 8 ? 8 : 6;
  }
  Move m = m_game.findMove(lookahead);

  executeMove(m, m_game.playerInTurn());
  if(isMenuItemChecked(this, ID_OPTIONS_VISEVALUERINGSFUNKTION)) {
    scoreMsg = format(_T("Score:%d evalCount:%s eval/sec:%s"),m_game.getBestScore(),format1000(m_game.getEvalCount()).cstr(), format1000(m_game.getEvalCountPerSecond()).cstr());
  } else {
    scoreMsg = EMPTYSTRING;
  }

  if(m_game.getBestScore() > MAX_SCORE - 100 ) {
    winnerMsg = randomstring(winningstring);
  }

  showMessage(_T("%s %s"), scoreMsg.cstr(), winnerMsg.cstr());
  return m;
}

void C4PaaStribeDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  if(m_game.playerInTurn() == YOU && !m_game.gameOver() && m_game.getBrickCount() < BOARDHEIGHT * BOARDWIDTH) {
    int c = point2column(point);

    if( c >= 0 && m_game.colh(c) < BOARDHEIGHT) { // validering
      executeMove(c, m_game.playerInTurn());
      if(!m_game.gameOver() && m_game.getBrickCount() < BOARDHEIGHT * BOARDWIDTH) {
        c = executeMachineMove();
      }
      checkgameover(c);
    }
  }
  __super::OnLButtonDown(nFlags, point);
}

void C4PaaStribeDlg::OnFileNytSpil() {
  switch(MessageBox(_T("Er du sikker på at du vil afslutte dette spil"),_T("Ny spil"), MB_YESNO | MB_ICONQUESTION)) {
  case IDYES:
    newGame();
    break;
  case IDNO :
    return;
  }
}

void C4PaaStribeDlg::OnOptionsAnimation() {
  toggleMenuItem(this, ID_OPTIONS_ANIMATION);
}

void C4PaaStribeDlg::OnOptionsVisevalueringsfunktion() {
  toggleMenuItem(this, ID_OPTIONS_VISEVALUERINGSFUNKTION);
}

void C4PaaStribeDlg::OnOptionsBegynder() {
  checkMenuItem(this, ID_OPTIONS_BEGYNDER,true );
  checkMenuItem(this, ID_OPTIONS_EXPERT  ,false);
}

void C4PaaStribeDlg::OnOptionsExpert() {
  checkMenuItem(this, ID_OPTIONS_BEGYNDER,false);
  checkMenuItem(this, ID_OPTIONS_EXPERT  ,true );
}

void C4PaaStribeDlg::OnOptionsVisfeltvrdier() {
  toggleMenuItem(this, ID_OPTIONS_VISFELTVRDIER);
  Invalidate(false);
}
