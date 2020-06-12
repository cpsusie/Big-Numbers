#include "stdafx.h"
#include "fraktalsDlg.h"

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

CFraktalsDlg::CFraktalsDlg(CWnd *pParent /*=NULL*/) : CDialog(CFraktalsDlg::IDD, pParent) {
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CFraktalsDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFraktalsDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    ON_COMMAND(ID_FILE_SIERPINSKI, OnFileSierpinski)
    ON_COMMAND(ID_FILE_TREKANT, OnFileTrekant)
    ON_COMMAND(ID_FILE_ROSE, OnFileRose)
    ON_COMMAND(ID_FILE_TR, OnFileTr)
    ON_COMMAND(ID_FILE_COUNTDOWN, OnFileCountDown)
    ON_COMMAND(ID_FILE_COUNTUP, OnFileCountUp)
    ON_COMMAND(ID_FILE_KRUSSEDULLE, OnFileKrussedulle)
    ON_COMMAND(ID_FILE_BEHOLDFORRIGE, OnFileBeholdforrige)
    ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
    ON_WM_KEYDOWN()
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_FNUG, OnFileFnug)
    ON_COMMAND(ID_FILE_STJERNE, OnFileStjerne)
END_MESSAGE_MAP()

BOOL CFraktalsDlg::OnInitDialog() {
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

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_currentlevel = 1;
  m_currentfraktal = ID_FILE_SIERPINSKI;

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFraktalsDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CFraktalsDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
    drawfraktal();
  }
}

HCURSOR CFraktalsDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CFraktalsDlg::OnFileExit() {
  exit(0);
}

void CFraktalsDlg::OnFileSierpinski() {
  setcurrentfraktype(ID_FILE_SIERPINSKI);
  Invalidate(true);
}

void CFraktalsDlg::OnFileTrekant() {
  setcurrentfraktype(ID_FILE_TREKANT);
  Invalidate(true);
}

void CFraktalsDlg::OnFileRose() {
  setcurrentfraktype(ID_FILE_ROSE);
  Invalidate(true);
}

void CFraktalsDlg::OnFileTr() {
  setcurrentfraktype(ID_FILE_TR);
  Invalidate(true);
}

void CFraktalsDlg::OnFileKrussedulle() {
  setcurrentfraktype(ID_FILE_KRUSSEDULLE);
  Invalidate(true);
}

void CFraktalsDlg::OnFileFnug() {
  setcurrentfraktype(ID_FILE_FNUG);
  Invalidate(true);
}

void CFraktalsDlg::OnFileStjerne() {
  setcurrentfraktype(ID_FILE_STJERNE);
  Invalidate(true);
}

void CFraktalsDlg::OnCancel() {
  if(MessageBox( _T("Er du sikker på at du vil stoppe"),EMPTYSTRING, MB_YESNO + MB_ICONQUESTION) == IDYES) {
    exit(0);
  }
}

void CFraktalsDlg::OnOK() {
}

void CFraktalsDlg::OnFileCountDown() {
  if(m_currentlevel <= 0) return;
  m_currentlevel--;
  if(ischecked(ID_FILE_BEHOLDFORRIGE))
    Invalidate(false);
  else
    Invalidate(true);
}

void CFraktalsDlg::OnFileCountUp() {
  m_currentlevel++;
  if(ischecked(ID_FILE_BEHOLDFORRIGE))
    Invalidate(false);
  else
    Invalidate(true);
}

void CFraktalsDlg::OnFileBeholdforrige() {
  togglestate(ID_FILE_BEHOLDFORRIGE);
  Invalidate(true);
}

void CFraktalsDlg::OnHelpAbout() {
  CAboutDlg dlg;
  dlg.DoModal();
}

void CFraktalsDlg::sirp1(double l, int dir) {
  m_turtle.move(l); m_turtle.turn(dir);
  m_turtle.move(l); m_turtle.turn(dir);
  m_turtle.move(l); m_turtle.turn(2*dir);
}

void CFraktalsDlg::sirp2(int level, double l, int dir) {
  if(level < 1)
    sirp1(l,dir);
  else {
    level--;
    m_turtle.turn(dir);  sirp2(level,l,-dir) ; m_turtle.turn(-dir); m_turtle.move(l);
    sirp2(level,l, dir); m_turtle.turn(dir)  ; m_turtle.move(l);    m_turtle.turn(-dir);
    sirp2(level,l, dir); m_turtle.turn(2*dir); m_turtle.move(l);
    m_turtle.turn(dir); sirp2(level,l, -dir) ; m_turtle.turn(dir);

  }
}

void CFraktalsDlg::drawsierpinski(int level) {
  m_turtle.init(this,0,640,0,480);
  m_turtle.jumpTo(10,-10);
  m_turtle.turnTo(90);

  double f = 2;
  for(int i = 0; i < level; i++) f *= 2;
  double l = 500 / (f - 1);
  sirp2(level,l,270);
}

void CFraktalsDlg::trekant(int level, double l, int dir) {
  if(level < 1)
    m_turtle.move(l);
  else {
    level--;
    m_turtle.turn(dir) ; trekant(level,l,-dir);
    m_turtle.turn(-dir); trekant(level,l,dir);
    m_turtle.turn(-dir); trekant(level,l,-dir);
    m_turtle.turn(dir) ;
  }
}

void CFraktalsDlg::drawtrekant(int level) {
  double l;
  int i;
  m_turtle.init(this,0,640,0,480);
  m_turtle.jumpTo(0,-10);
  m_turtle.turnTo(0);
  for(i = 0,l = 600; i < level; i++) l /= 2;
  trekant(level,l,60);
}

void CFraktalsDlg::rose(int level, double l, int dir) {
  if(level < 1)
    m_turtle.move(l);
  else {
    level--;
    rose(level,l,dir);

    m_turtle.turn(60);
    rose(level,l,dir);

    m_turtle.turn(-120);
    rose(level,l,dir);

    m_turtle.turn(60);
    rose(level,l,dir);
  }
}

void CFraktalsDlg::drawrose(int level) {
  m_turtle.init(this,0,640,0,480);
  m_turtle.jumpTo(0,-10);
  m_turtle.turnTo(0);
  int i;
  double l;
  for(i = 0,l = 600; i < level; i++) l /= 3;
  rose(level,l,60);
}

void CFraktalsDlg::krussedulle(int level, double l, int dir) {
  if(level < 1)
    m_turtle.move(l);
  else {
    level--;

    m_turtle.turn(60);
    krussedulle(level,l,dir);

    m_turtle.turn(-120);
    krussedulle(level,l,dir);

    m_turtle.turn(60);
    krussedulle(level,l,dir);
  }
}

void CFraktalsDlg::drawkrussedulle(int level) {
  m_turtle.init(this,0,640,0,480);
  m_turtle.jumpTo(50,100);
  m_turtle.turnTo(0);
  int i;
  double l;
  for(i = 0,l = 600; i < level; i++) l /= 2;
  krussedulle(level,l,60);
}

void CFraktalsDlg::trae(int level, double l) {
  if(level < 1)
    m_turtle.move(l);
  else {
    m_turtle.move(l);
    Point2D pos = m_turtle.pos();
    double direc = m_turtle.dir();
    m_turtle.turn(-45);
    trae(level-1,l/1.5);
    m_turtle.jumpTo(pos);
    m_turtle.turnTo(direc);
    m_turtle.turn(45);
    trae(level-1,l/1.5);
  }
}

void CFraktalsDlg::drawtrae(int level) {
  m_turtle.init(this,0,640,0,480);
  m_turtle.jumpTo(330,0);
  m_turtle.turnTo(90);
  trae(level,180);
}

void CFraktalsDlg::fnug(int level, double l, int dir) {
  if(level < 1)
    m_turtle.move(l);
  else {
    level--;

    m_turtle.turn(dir);
    fnug(level,l,dir);

    m_turtle.turn(-2*dir);
    fnug(level,l,dir);

    fnug(level,l,dir);

    m_turtle.turn(2*dir);
    fnug(level,l,dir);

    m_turtle.turn(-dir);
  }
}

void CFraktalsDlg::drawfnug(int level) {
  m_turtle.init(this,0,640,0,480);
  m_turtle.jumpTo(50,250);
  m_turtle.turnTo(0);
  int i;
  double l;
  for(i = 0,l = 500; i < level; i++) l /= 2;
  fnug(level,l,60);
}

#define STARPARAM 20
void CFraktalsDlg::stjerne(int level, double l) {
  for(int i = 0; i < 22; i++) {
    m_turtle.move(l);
    if(level > 0) {
      m_turtle.turn(-STARPARAM);
      stjerne(level-1,l/4);
      m_turtle.turn(STARPARAM);
    }
    m_turtle.turn(180-STARPARAM);
  }
}

void CFraktalsDlg::drawstjerne(int level) {
  m_turtle.init(this,0,600,0,600);
  m_turtle.jumpTo(100,300);
  m_turtle.turnTo(0);
  stjerne(level,400);
}

void CFraktalsDlg::drawfraktal() {
  switch(m_currentfraktal) {
  case ID_FILE_SIERPINSKI:
    drawsierpinski(m_currentlevel);
    break;
  case ID_FILE_TREKANT   :
    drawtrekant(m_currentlevel);
    break;
  case ID_FILE_ROSE      :
    drawrose(m_currentlevel);
    break;
  case ID_FILE_KRUSSEDULLE:
    drawkrussedulle(m_currentlevel);
    break;
  case ID_FILE_TR:
    drawtrae(m_currentlevel);
    break;
  case ID_FILE_FNUG:
    drawfnug(m_currentlevel);
    break;
  case ID_FILE_STJERNE:
    drawstjerne(m_currentlevel);
    break;
  }

}

bool CFraktalsDlg::ischecked(int id) {
  CMenu *cm = GetMenu();
  int checked = cm->GetMenuState(id,MF_BYCOMMAND) & MF_CHECKED;
  return checked ? true : false;
}

void CFraktalsDlg::setcheckstate(int id, bool on) {
  CMenu *cm = GetMenu();
  cm->CheckMenuItem(id, on ? MF_CHECKED : MF_UNCHECKED);
}

void CFraktalsDlg::togglestate(int id) {
  setcheckstate(id,!ischecked(id));
}

void CFraktalsDlg::setcurrentfraktype(int id) {
  setcheckstate(ID_FILE_SIERPINSKI ,false);
  setcheckstate(ID_FILE_TREKANT    ,false);
  setcheckstate(ID_FILE_ROSE       ,false);
  setcheckstate(ID_FILE_KRUSSEDULLE,false);
  setcheckstate(ID_FILE_TR         ,false);
  setcheckstate(ID_FILE_FNUG       ,false);
  setcheckstate(ID_FILE_STJERNE    ,false);
  setcheckstate(id,true);
  if(id != m_currentfraktal)
    m_currentlevel = 1;

  m_currentfraktal = id;
}

void CFraktalsDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  switch(nChar) {
  case 34: /* Pagedown */
    OnFileCountDown();
    break;
  case 33: /* Pageup */
    OnFileCountUp();
    break;
  case 113: // F2
    OnFileBeholdforrige();
    break;
  case 114: // F3
    break;
  case 115: // F4
    break;
  default:
    break;
  }
  __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFraktalsDlg::OnSize(UINT nType, int cx, int cy)
{
  __super::OnSize(nType, cx, cy);
  m_turtle.OnSize(this);
  Invalidate(true);
}


