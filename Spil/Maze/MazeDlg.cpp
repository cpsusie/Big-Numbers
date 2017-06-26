#include "stdafx.h"
#include <Random.h>
#include "MazeDlg.h"
#include "EasyRectMaze.h"
#include "HardRectMaze.h"
#include "HexMaze.h"
#include "TriangularMaze.h"

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


void CMazeDlg::OnHelpAboutMaze() {
  CAboutDlg dlg;
  dlg.DoModal();
}

CMazeDlg::CMazeDlg(CWnd *pParent) : CDialog(CMazeDlg::IDD, pParent) {
  m_initialized = false;
  m_pathFinder  = NULL;
  m_maze        = NULL;
  m_doorWidth   = 10;
  m_hIcon       = theApp.LoadIcon(IDR_MAINICON);
}

void CMazeDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMazeDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_PRINT                   , OnFilePrint                  )
    ON_COMMAND(ID_FILE_QUIT                    , OnFileQuit                   )
    ON_COMMAND(ID_FILE_NEWMAZE                 , OnFileNewMaze                )
    ON_COMMAND(ID_EDIT_FINDPATH                , OnEditFindPath               )
    ON_COMMAND(ID_EDIT_CLEAR                   , OnEditClear                  )
    ON_COMMAND(ID_HELP_ABOUTMAZE               , OnHelpAboutMaze              )
    ON_COMMAND(ID_OPTIONS_SPEED_SLOW           , OnOptionsSpeedSlow           )
    ON_COMMAND(ID_OPTIONS_SPEED_FAST           , OnOptionsSpeedFast           )
    ON_COMMAND(ID_OPTIONS_KEEPTRACK            , OnOptionsKeepTrack           )
    ON_COMMAND(ID_OPTIONS_DOORCHOICE_LEFTFIRST , OnOptionsPathchoiceLeftFirst )
    ON_COMMAND(ID_OPTIONS_DOORCHOICE_RIGHTFIRST, OnOptionsPathchoiceRightFirst)
    ON_COMMAND(ID_OPTIONS_DOORCHOICE_MIX       , OnOptionsPathchoiceMix       )
    ON_COMMAND(ID_OPTIONS_DOORSIZE_1           , OnOptionsDoorSize1           )
    ON_COMMAND(ID_OPTIONS_DOORSIZE_2           , OnOptionsDoorSize2           )
    ON_COMMAND(ID_OPTIONS_DOORSIZE_3           , OnOptionsDoorSize3           )
    ON_COMMAND(ID_OPTIONS_DOORSIZE_4           , OnOptionsDoorSize4           )
    ON_COMMAND(ID_OPTIONS_DOORSIZE_5           , OnOptionsDoorSize5           )
    ON_COMMAND(ID_OPTIONS_DOORSIZE_6           , OnOptionsDoorSize6           )
    ON_COMMAND(ID_OPTIONS_DOORSIZE_10          , OnOptionsDoorSize10          )
    ON_COMMAND(ID_OPTIONS_LEVEL_EASY           , OnOptionsLevelEasy           )
    ON_COMMAND(ID_OPTIONS_LEVEL_HARD           , OnOptionsLevelHard           )
    ON_COMMAND(ID_OPTIONS_LEVEL_HEXAGONAL      , OnOptionsLevelHexagonal      )
    ON_COMMAND(ID_OPTIONS_LEVEL_TRIANGULAR    ,  OnOptionsLevelTriangular     )
    ON_WM_CLOSE()
END_MESSAGE_MAP()

void CMazeDlg::createDC() {
  CClientDC dc(this);
  int bitsPixel = dc.GetDeviceCaps(BITSPIXEL);
  int planes = dc.GetDeviceCaps(PLANES);

  m_cleanMazeDC.CreateCompatibleDC(&dc);
  m_workDC.CreateCompatibleDC(&dc);
  CRect r;
  GetClientRect(&r);
  m_cleanMazeBM.CreateBitmap(r.Width(),r.Height(),planes,bitsPixel,NULL);
  m_workBM.CreateBitmap(r.Width(),r.Height(),planes,bitsPixel,NULL);
  m_cleanMazeDC.BitBlt(0,0,r.Width(),r.Height(),NULL,0,0,WHITENESS);
}

void CMazeDlg::destroyDC() {
  m_cleanMazeBM.DeleteObject();
  m_workBM.DeleteObject();
  m_cleanMazeDC.DeleteDC();
  m_workDC.DeleteDC();
}

BOOL CMazeDlg::OnInitDialog() {
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

  createDC();
  newMaze();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_MAINDIALOG));
  m_initialized = true;

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMazeDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)   {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CMazeDlg::OnPaint() {
  if (IsIconic())   {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client drawRectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    if(m_pathFinder != NULL) {
      m_pathFinder->suspend();
    }

    __super::OnPaint();
    workToScreen();

    if(m_pathFinder != NULL) {
      m_pathFinder->resume();
    }
  }
}

void CMazeDlg::workToScreen() {
  CRect r;
  GetClientRect(&r);
  CClientDC dc(this);
  dc.BitBlt(0, 0, r.Width(), r.Height(), &m_workDC, 0, 0, SRCCOPY);
}

void CMazeDlg::DCToWork(CDC &dc) {
  CRect r;
  GetClientRect(&r);
  SelectObject(m_workDC,m_workBM);
  m_workDC.BitBlt(0, 0, r.Width(), r.Height(), &dc, 0, 0, SRCCOPY);
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMazeDlg::OnQueryDragIcon() {
    return (HCURSOR) m_hIcon;
}

void CMazeDlg::newMaze() {
  stopPathFinder();
  theApp.BeginWaitCursor();

  if(m_maze != NULL) {
    delete m_maze;
    m_maze = NULL;
  }

  CRect rect = getClientRect(this);

  if(isMenuItemChecked(this,ID_OPTIONS_LEVEL_HARD)) {
    m_maze = new HardRectMaze(rect,m_doorWidth);
  } else if(isMenuItemChecked(this,ID_OPTIONS_LEVEL_EASY)) {
    m_maze = new EasyRectMaze(rect,m_doorWidth);
  } else if(isMenuItemChecked(this, ID_OPTIONS_LEVEL_HEXAGONAL)) {
    m_maze = new HexMaze(rect,m_doorWidth);
  } else {
    m_maze = new TriangularMaze(rect,m_doorWidth);
  }

  m_cleanMazeDC.SelectObject(&m_cleanMazeBM);
  m_maze->paint(m_cleanMazeDC);

  DCToWork(m_cleanMazeDC);

  theApp.EndWaitCursor();
}

void CMazeDlg::OnClose() {
  EndDialog(IDOK);
}

BOOL CMazeDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CMazeDlg::OnFileNewMaze() {
  newMaze();
  Invalidate();
}

void CMazeDlg::stopPathFinder() {
  if(m_pathFinder != NULL) {
    m_pathFinder->stop();
    while(m_pathFinder->stillActive()) {
      Sleep(100);
    }
    delete m_pathFinder;
    m_pathFinder = NULL;
  }
}

void CMazeDlg::OnEditFindPath() {
  stopPathFinder();
  OnEditClear();
  bool keepTrack      = isMenuItemChecked(this,ID_OPTIONS_KEEPTRACK);
  bool slow           = isMenuItemChecked(this,ID_OPTIONS_SPEED_SLOW);
  int  choiceStrategy = isMenuItemChecked(this,ID_OPTIONS_DOORCHOICE_LEFTFIRST )?ID_OPTIONS_DOORCHOICE_LEFTFIRST :
                        isMenuItemChecked(this,ID_OPTIONS_DOORCHOICE_RIGHTFIRST)?ID_OPTIONS_DOORCHOICE_RIGHTFIRST:
                                                                                 ID_OPTIONS_DOORCHOICE_MIX;

  m_pathFinder = new PathFinder(m_maze,this,m_workDC,keepTrack,slow,choiceStrategy);
  m_pathFinder->start();
}

void CMazeDlg::OnSize(UINT nType, int cx, int cy) {
  stopPathFinder();
  if(m_initialized) {
    destroyDC();
    createDC();
    newMaze();
  }

  __super::OnSize(nType, cx, cy);

  Invalidate();
}

void CMazeDlg::OnFileQuit() {
  stopPathFinder();
  EndDialog(IDOK);
}

void CMazeDlg::OnOK() {
}

void CMazeDlg::OnCancel() {
}

void CMazeDlg::OnEditClear() {
  stopPathFinder();
  DCToWork(m_cleanMazeDC);
  workToScreen();
}

void CMazeDlg::OnOptionsSpeedSlow() {
  checkMenuItem(this,ID_OPTIONS_SPEED_SLOW,true);
  checkMenuItem(this,ID_OPTIONS_SPEED_FAST,false);
  if(hasPathFinder()) m_pathFinder->setSlowSpeed(true);
}

void CMazeDlg::OnOptionsSpeedFast() {
  checkMenuItem(this,ID_OPTIONS_SPEED_SLOW,false);
  checkMenuItem(this,ID_OPTIONS_SPEED_FAST,true);
  if(hasPathFinder()) m_pathFinder->setSlowSpeed(false);
}

void CMazeDlg::OnOptionsKeepTrack() {
  checkMenuItem(this,ID_OPTIONS_KEEPTRACK,!isMenuItemChecked(this,ID_OPTIONS_KEEPTRACK));
}

void CMazeDlg::setDoorChoice(int choice) {
  checkMenuItem(this,ID_OPTIONS_DOORCHOICE_LEFTFIRST ,false);
  checkMenuItem(this,ID_OPTIONS_DOORCHOICE_RIGHTFIRST,false);
  checkMenuItem(this,ID_OPTIONS_DOORCHOICE_MIX       ,false);
  checkMenuItem(this,choice,true);
}

void CMazeDlg::OnOptionsPathchoiceLeftFirst() {
  setDoorChoice(ID_OPTIONS_DOORCHOICE_LEFTFIRST);
}

void CMazeDlg::OnOptionsPathchoiceRightFirst() {
  setDoorChoice(ID_OPTIONS_DOORCHOICE_RIGHTFIRST);
}

void CMazeDlg::OnOptionsPathchoiceMix() {
  setDoorChoice(ID_OPTIONS_DOORCHOICE_MIX);
}

static int validDoorSize[] = {
   ID_OPTIONS_DOORSIZE_1
  ,ID_OPTIONS_DOORSIZE_2
  ,ID_OPTIONS_DOORSIZE_3
  ,ID_OPTIONS_DOORSIZE_4
  ,ID_OPTIONS_DOORSIZE_5
  ,ID_OPTIONS_DOORSIZE_6
  ,ID_OPTIONS_DOORSIZE_10
};

void CMazeDlg::setDoorSize(int item,int size) {
  if(isMenuItemChecked(this,item)) {
    return;
  }
  for(int j = 0; j < ARRAYSIZE(validDoorSize); j++) {
    checkMenuItem(this,validDoorSize[j],false);
  }

  checkMenuItem(this,item,true);

  m_doorWidth = 2*size;

  newMaze();
  Invalidate();
}

void CMazeDlg::OnOptionsDoorSize1()  { setDoorSize(ID_OPTIONS_DOORSIZE_1 , 1); }
void CMazeDlg::OnOptionsDoorSize2()  { setDoorSize(ID_OPTIONS_DOORSIZE_2 , 3); }
void CMazeDlg::OnOptionsDoorSize3()  { setDoorSize(ID_OPTIONS_DOORSIZE_3 , 5); }
void CMazeDlg::OnOptionsDoorSize4()  { setDoorSize(ID_OPTIONS_DOORSIZE_4 , 7); }
void CMazeDlg::OnOptionsDoorSize5()  { setDoorSize(ID_OPTIONS_DOORSIZE_5 , 9); }
void CMazeDlg::OnOptionsDoorSize6()  { setDoorSize(ID_OPTIONS_DOORSIZE_6 ,11); }
void CMazeDlg::OnOptionsDoorSize10() { setDoorSize(ID_OPTIONS_DOORSIZE_10,18); }

void CMazeDlg::setLevel(int level) {
  if(isMenuItemChecked(this,level)) {
    return;
  }
  checkMenuItem(this,ID_OPTIONS_LEVEL_EASY      , false);
  checkMenuItem(this,ID_OPTIONS_LEVEL_HARD      , false);
  checkMenuItem(this,ID_OPTIONS_LEVEL_HEXAGONAL , false);
  checkMenuItem(this,ID_OPTIONS_LEVEL_TRIANGULAR, false);
  checkMenuItem(this,level,true);
  newMaze();
  Invalidate();
}

void CMazeDlg::OnOptionsLevelEasy() {
  setLevel(ID_OPTIONS_LEVEL_EASY);
}

void CMazeDlg::OnOptionsLevelHard() {
  setLevel(ID_OPTIONS_LEVEL_HARD);
}

void CMazeDlg::OnOptionsLevelHexagonal() {
  setLevel(ID_OPTIONS_LEVEL_HEXAGONAL);
}

void CMazeDlg::OnOptionsLevelTriangular() {
  setLevel(ID_OPTIONS_LEVEL_TRIANGULAR);
}

void CMazeDlg::OnFilePrint() {
  CPrintDialog dlg(false);
  if(dlg.DoModal() == IDOK) {
    HDC hdc = dlg.GetPrinterDC();
    CDC *dc = CDC::FromHandle(hdc);
    CSize cs1 = dc->GetViewportExt();
    CSize cs2 = dc->GetWindowExt();
    int dpw = dc->GetDeviceCaps(HORZRES);
    int dph = dc->GetDeviceCaps(VERTRES);
    dc->StartDoc(_T("Maze"));

    CRect r;
    GetClientRect(&r);
    dc->StretchBlt(0,0,dpw, dph, &m_workDC, 0, 0, r.Width(), r.Height(), SRCCOPY);
    dc->EndDoc();
  }
}
