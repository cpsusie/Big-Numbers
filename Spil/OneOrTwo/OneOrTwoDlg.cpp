#include "stdafx.h"
#include <Random.h>
#include "OneOrTwo.h"
#include "OneOrTwoDlg.h"
#include "PasswordDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
private:
  COneOrTwoDlg *m_mainDlg;
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg(COneOrTwoDlg *mainDlg) : CDialog(IDD), m_mainDlg(mainDlg) {
  }

  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CAboutDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
  CRect iconRect = getRelativeClientRect(this, IDC_GAMEICON);
  if(iconRect.PtInRect(point) && (nFlags & MK_CONTROL)) {
    if(m_mainDlg->isShowingMoveList()) {
      m_mainDlg->toggleShowMoveList();
    } else {
//      CPasswordDlg dlg;
//      if(dlg.DoModal() == IDOK) {
        m_mainDlg->toggleShowMoveList();
//      }
    }
  }
  __super::OnLButtonDblClk(nFlags, point);
}

COneOrTwoDlg::COneOrTwoDlg(CWnd *pParent) : CDialog(COneOrTwoDlg::IDD, pParent) {
  m_hIcon         = theApp.LoadIcon(IDR_MAINFRAME);
  m_game          = nullptr;
  m_showMoveList  = false;
  m_timerInterval = 0;
}

void COneOrTwoDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COneOrTwoDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_TIMER()
    ON_COMMAND(ID_FILE_NYTSPIL_DUBEGYNDER , OnFileNewGameHumanStart   )
    ON_COMMAND(ID_FILE_NYTSPIL_JEGBEGYNDER, OnFileNewGameComputerStart)
    ON_COMMAND(ID_FILE_EXIT               , OnFileExit                )
    ON_COMMAND(ID_SPIL_PENTAGON           , OnSpilPentagon            )
    ON_COMMAND(ID_SPIL_HEXAGON            , OnSpilHexagon             )
    ON_COMMAND(ID_SPIL_OCTAGON            , OnSpilOctagon             )
    ON_COMMAND(ID_LEVEL_BEGINNER          , OnLevelBeginner           )
    ON_COMMAND(ID_LEVEL_INTERMEDIATE      , OnLevelIntermediate       )
    ON_COMMAND(ID_LEVEL_EXPERT            , OnLevelExpert             )
    ON_COMMAND(ID_HELP_ABOUTONEORTWO      , OnHelpAboutOneOrTwo       )
    ON_BN_CLICKED(IDC_BUTTONREMOVE        , OnButtonRemove            )
    ON_BN_CLICKED(IDC_BUTTONSHOWMOVES     , OnButtonShowMoves         )
END_MESSAGE_MAP()

BOOL COneOrTwoDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != nullptr) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  randomize();
  newGame((Player)(randInt() % 2));
  return TRUE;
}

void COneOrTwoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg(this).DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void COneOrTwoDlg::startTimer(int msec) {
  if(!m_timerInterval && SetTimer(1, msec, nullptr)) {
    m_timerInterval = msec;
  }
}

void COneOrTwoDlg::stopTimer() {
  if(m_timerInterval) {
    KillTimer(1);
    m_timerInterval = 0;
  }
}

void COneOrTwoDlg::OnPaint() {
  if(IsIconic()) {
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
    CPaintDC dc(this);
    m_game->paint(dc, getClientRect(this), m_markedPositions);
//    enableRemoveButton();
  }
}

void COneOrTwoDlg::toggleShowMoveList() {
  m_showMoveList = !m_showMoveList;
  m_game->setShowNumbers(m_showMoveList);
  GetDlgItem(IDC_BUTTONSHOWMOVES)->ShowWindow(m_showMoveList?SW_SHOW:SW_HIDE);
  Invalidate();
}

void COneOrTwoDlg::enableRemoveButton() {
  GetDlgItem(IDC_BUTTONREMOVE)->EnableWindow((m_markedPositions != 0) && m_game->isHumanToMove());
}

HCURSOR COneOrTwoDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void COneOrTwoDlg::OnHelpAboutOneOrTwo() {
  CAboutDlg dlg(this);
  dlg.DoModal();
}

void COneOrTwoDlg::OnFileExit() {
  PostMessage(WM_QUIT);
}

void COneOrTwoDlg::OnClose() {
  OnFileExit();
}

void COneOrTwoDlg::OnCancel() {
}

void COneOrTwoDlg::OnOK() {
}

void COneOrTwoDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  Invalidate();
}

void COneOrTwoDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  if(!m_game->isHumanToMove()) {
    return;
  }
  const int pos = m_game->findPosition(point);
  if(pos >= 0) {
    changePositionMark(pos);
  }
  __super::OnLButtonDown(nFlags, point);
}

int COneOrTwoDlg::getMarkedCount() const {
  UINT count = 0;
  for(unsigned short v = m_markedPositions; v; v &= (v-1)) {
    count++;
  }
  return count;
}

void COneOrTwoDlg::changePositionMark(int pos) {
  if(SET_CONTAINS(m_markedPositions, pos)) {
    SET_REMOVE(m_markedPositions, pos);
  } else {
    if(getMarkedCount() == 2) {
      return;
    }
    SET_ADD(m_markedPositions, pos);
    if(!m_game->isLegalMarks(m_markedPositions)) {
      SET_REMOVE(m_markedPositions, pos);
      return;
    }
  }

  CClientDC dc(this);
  m_game->paintPosition(dc, pos, SET_CONTAINS(m_markedPositions, pos));
  enableRemoveButton();
}

void COneOrTwoDlg::OnFileNewGameHumanStart() {
  newGame(HUMANPLAYER);
}

void COneOrTwoDlg::OnFileNewGameComputerStart() {
  newGame(COMPUTERPLAYER);
}

void COneOrTwoDlg::newGame() {
  newGame(GETOPPONENT(m_startPlayer));
}

void COneOrTwoDlg::newGame(Player starter) {
  if(m_game == nullptr || (m_game->getGameType() != getSelectedGameType())) {
    if(m_game != nullptr) {
      delete m_game;
      m_game = nullptr;
    }
    switch(getSelectedGameType()) {
    case PENTAGON : m_game = new GamePentagon(); break;
    case HEXAGON  : m_game = new GameHexagon() ; break;
    case OCTAGON  : m_game = new GameOctagon();  break;
    }
  }

  m_game->setShowNumbers(m_showMoveList);
  m_game->init(starter);
  m_markedPositions = 0;
  m_startPlayer     = starter;
  Invalidate();
  if(!m_game->isHumanToMove()) {
    startTimer(500);
  }
}

BOOL COneOrTwoDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void COneOrTwoDlg::OnButtonRemove() {
  if(m_markedPositions == 0 || !m_game->isHumanToMove()) {
    return;
  }

  m_game->executeMove(m_markedPositions);
  m_markedPositions = 0;
  Invalidate();
  if(m_game->isGameOver()) {
    if(MessageBox(_T("Jeg vandt.\n\rVil du spille igen"), _T("Spillet er slut"), MB_ICONQUESTION | MB_YESNO) == IDYES) {
      newGame();
    } else {
      OnFileExit();
    }
  } else {
    startTimer(800);
  }
}

void COneOrTwoDlg::OnButtonShowMoves() {
  MoveResultArray a;
  const Move m = m_game->findBestMove(a, getSelectedLavel());
  String tmp;
  for(size_t i = 0; i < a.size(); i++) {
    tmp += a[i].toString();
    tmp += _T("\n\r");
  }
  showInformation(tmp);
}

void COneOrTwoDlg::OnTimer(UINT_PTR nIDEvent) {
  stopTimer();
  MoveResultArray a;
  Move m = m_game->findBestMove(a, getSelectedLavel());
  m_game->animateMove(CClientDC(this), m);
  m_game->executeMove(*m);
  Invalidate();
  __super::OnTimer(nIDEvent);
  if(m_game->isGameOver()) {
    if(MessageBox(_T("Du vandt.\n\rVil du spille igen"), _T("Spillet er slut"), MB_ICONQUESTION | MB_YESNO) == IDYES) {
      newGame();
    } else {
      OnFileExit();
    }
  }
}

void COneOrTwoDlg::OnSpilPentagon() {
  if(getSelectedGameType() == PENTAGON) {
    return;
  }
  setGameType(ID_SPIL_PENTAGON);
  newGame(m_startPlayer);
}

void COneOrTwoDlg::OnSpilHexagon() {
  if(getSelectedGameType() == HEXAGON) {
    return;
  }
  setGameType(ID_SPIL_HEXAGON);
  newGame(m_startPlayer);
}

void COneOrTwoDlg::OnSpilOctagon() {
  if(getSelectedGameType() == OCTAGON) {
    return;
  }
  setGameType(ID_SPIL_OCTAGON);
  newGame(m_startPlayer);
}

GameType COneOrTwoDlg::getSelectedGameType() {
  if(isMenuItemChecked(this, ID_SPIL_PENTAGON)) {
    return PENTAGON;
  } else if(isMenuItemChecked(this, ID_SPIL_HEXAGON)) {
    return HEXAGON;
  } else if(isMenuItemChecked(this, ID_SPIL_OCTAGON)) {
    return OCTAGON;
  } else {
    showWarning(_T("Ingen spiltype valgt!!"));
    return PENTAGON;
  }
}

void COneOrTwoDlg::setGameType(int id) {
  checkMenuItem(this, ID_SPIL_PENTAGON, false);
  checkMenuItem(this, ID_SPIL_HEXAGON , false);
  checkMenuItem(this, ID_SPIL_OCTAGON , false);
  checkMenuItem(this, id              , true);
}

void COneOrTwoDlg::OnLevelBeginner() {
  selectLevel(ID_LEVEL_BEGINNER);
}

void COneOrTwoDlg::OnLevelIntermediate() {
  selectLevel(ID_LEVEL_INTERMEDIATE);
}

void COneOrTwoDlg::OnLevelExpert() {
  selectLevel(ID_LEVEL_EXPERT);
}

void COneOrTwoDlg::selectLevel(int id) {
  checkMenuItem(this, ID_LEVEL_BEGINNER    , false);
  checkMenuItem(this, ID_LEVEL_INTERMEDIATE, false);
  checkMenuItem(this, ID_LEVEL_EXPERT      , false);
  checkMenuItem(this, id                   , true );
}

PlayLevel COneOrTwoDlg::getSelectedLavel() {
  if(isMenuItemChecked(       this, ID_LEVEL_BEGINNER    )) {
    return LEVEL_BEGINNER;
  } else if(isMenuItemChecked(this, ID_LEVEL_INTERMEDIATE)) {
    return LEVEL_INTERMEDIATE;
  } else if(isMenuItemChecked(this, ID_LEVEL_EXPERT      )) {
    return LEVEL_EXPERT;
  } else {
    showWarning(_T("Niveau ikke valgt"));
    return LEVEL_BEGINNER;;
  }
}
