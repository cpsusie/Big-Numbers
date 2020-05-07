#include "stdafx.h"
#include "stdafx.h"
#include <Random.h>
#include <D3DGraphics/D3Math.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/D3Camera.h>
#include "QuartoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CQuartoDlg::CQuartoDlg(CWnd *pParent) : CDialog(IDD, pParent) {
  m_hIcon           = theApp.LoadIcon(IDR_MAINFRAME);
  m_startPlayer     = HUMAN_PLAYER;
  m_state           = DLG_IDLE;
  m_timerRunning    = false;
}

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg(CQuartoDlg *parent) : CDialog(IDD, parent) {
  }
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  const CRect r = getRelativeClientRect(this, IDC_STATICABOUTICON);
  if((nFlags&MK_CONTROL) && r.PtInRect(point)) {
    GetParent()->PostMessage(ID_MSG_TOGGLE_EDITMODE);
  }
  __super::OnLButtonDown(nFlags, point);
}

void CQuartoDlg::OnHelpAbout() {
  CAboutDlg(this).DoModal();
}

BEGIN_MESSAGE_MAP(CQuartoDlg, CDialog)
    ON_WM_QUERYDRAGICON()
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_CLOSE()
    ON_WM_LBUTTONDOWN()
    ON_COMMAND(ID_FILE_NEW              , OnFileNew             )
    ON_COMMAND(ID_FILE_OPEN             , OnFileOpen            )
    ON_COMMAND(ID_FILE_SAVE             , OnFileSave            )
    ON_COMMAND(ID_FILE_SAVEAS           , OnFileSaveAs          )
    ON_COMMAND(ID_FILE_EXIT             , OnFileExit            )
    ON_COMMAND(ID_VIEW_LEFT             , OnViewLeft            )
    ON_COMMAND(ID_VIEW_RIGHT            , OnViewRight           )
    ON_COMMAND(ID_VIEW_RESETVIEW        , OnViewResetView       )
    ON_COMMAND(ID_OPTIONS_LEVEL_BEGINNER, OnOptionsLevelBeginner)
    ON_COMMAND(ID_OPTIONS_LEVEL_EXPERT  , OnOptionsLevelExpert  )
    ON_COMMAND(ID_OPTIONS_COLOREDGAME   , OnOptionsColoredGame  )
    ON_COMMAND(ID_DUMP_SETUP            , OnDumpSetup           )
    ON_COMMAND(ID_HELP_ABOUT            , OnHelpAbout           )
    ON_MESSAGE(ID_MSG_RENDER            , OnMsgRender           )
    ON_MESSAGE(ID_MSG_TOGGLE_EDITMODE   , OnMsgToggleEditMode   )

END_MESSAGE_MAP()

HCURSOR CQuartoDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CQuartoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX)    {
    OnHelpAbout();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

BOOL CQuartoDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT( IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE );
  SetIcon(m_hIcon, FALSE);

  m_scene.initDevice(*this);
  m_editor.init(this);

  try {
    randomize();
    m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
    setWindowSize(this, CSize(947, 614));
    createScene();
    OnFileNew();
  } catch(Exception e) {
    showException(e, MB_ICONERROR);
    exit(-1);
  }
  return TRUE;  // return TRUE  unless you set the focus to a control
}

#define BACKGROUNDCOLOR RGB(153,217,234)
#define sCAM() m_editor.getSelectedCAM()

void CQuartoDlg::unInitDialog() {
  destroyScene();
  m_editor.close();
}

void CQuartoDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this);
    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width()  - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
    render(SC_RENDERALL);
  }
}

void CQuartoDlg::doRender(BYTE renderFlags, CameraSet cameraSet) {
  if(renderFlags & SC_RENDERNOW) {
    OnMsgRender(renderFlags, cameraSet);
  } else {
    PostMessage(ID_MSG_RENDER, renderFlags, cameraSet);
  }
}

LRESULT CQuartoDlg::OnMsgRender(WPARAM wp, LPARAM lp) {
  if(wp & SC_RENDER3D) {
    CameraSet cameraSet(lp);
    __super::doRender((BYTE)wp, cameraSet);
  }
  if(wp & SC_RENDERINFO) {
    if(m_editor.isEnabled()) {
      showEditorInfo();
    }
  }
  return 0;
}

LRESULT CQuartoDlg::OnMsgToggleEditMode(WPARAM wp, LPARAM lp) {
  setEditMode(!m_editor.isEnabled());
  render(SC_RENDERALL);
  return 0;
}

void CQuartoDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == m_boardObject) {
    switch(id) {
    case GB_CURRENTFIELD:
    case GB_CURRENTBRICK:
    case GB_BRICKPOSITIONS:
      render(SC_RENDERALL);
    }
  } else if(source == sCAM()) {
    switch(id) {
    case CAM_VIEW:
    case CAM_BACKGROUNDCOLOR:
      render(SC_RENDERALL);
      break;
    }
  }
}

void CQuartoDlg::startTimer(DialogState state, int msec) {
  m_state = state;
  startTimer(msec);
}

void CQuartoDlg::startTimer(int msec) {
  if(!m_timerRunning) {
    if(SetTimer(1, msec, NULL) == 1) {
      m_timerRunning = true;
    }
  }
}

void CQuartoDlg::stopTimer() {
  if(m_timerRunning) {
    KillTimer(1);
    m_timerRunning = false;
  }
}

void CQuartoDlg::OnTimer(UINT_PTR nIDEvent) {
  __super::OnTimer(nIDEvent);
  switch(m_state) {
  case DLG_IDLE        :
    stopTimer();
    break;
  case DLG_ONTIMERDOUSERMOVE:
    { const Move move(getSelectedField(), getSelectedBrick());
      executeMove(move);
      if(m_game.isGameOver()) {
        m_state = DLG_ONTIMERSHOWRESULT;
      } else {
        m_state = DLG_ONTIMERDOCOMPUTERMOVE;
      }
    }
    break;
  case DLG_ONTIMERDOCOMPUTERMOVE:
    executeMove(findMove());
    if(m_game.isGameOver()) {
      m_state = DLG_ONTIMERSHOWRESULT;
    } else {
      m_state = DLG_IDLE;
    }
    break;
  case DLG_ONTIMERSHOWRESULT:
    stopTimer();
    endGame();
    break;
  }
}

BOOL CQuartoDlg::PreTranslateMessage(MSG *pMsg) {
  D3Camera *cam;
  if((pMsg->message == WM_MOUSEMOVE) && ((cam = m_scene.getPickedCamera(pMsg->pt)) != NULL)) {
//    m_wndStatusBar.SetPaneText(0, toString(cam->screenToWin(pMsg->pt)).cstr());
  }
  const bool levelIncremented = pMsg->message != ID_MSG_RENDER;
  if(levelIncremented) incrLevel();
  BOOL result;
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    result = true;
  } else if(m_editor.PreTranslateMessage(pMsg)) {
    result = true;
  } else {
    result = __super::PreTranslateMessage(pMsg);
  }
  if(levelIncremented) decrLevel();
  return result;
}

void CQuartoDlg::createScene() {
  createBoard();
  createLight();
  resetCamera();
  sCAM()->addPropertyChangeListener(this);
}

void CQuartoDlg::destroyScene() {
  sCAM()->removePropertyChangeListener(this);
  destroyLight();
  destroyBoard();
}

void CQuartoDlg::createBoard() {
  m_boardObject = new GameBoardObject(m_scene); TRACE_NEW(m_boardObject);
  m_scene.addVisual(m_boardObject);
  m_boardObject->addPropertyChangeListener(this);
}

void CQuartoDlg::destroyBoard() {
  m_boardObject->removePropertyChangeListener(this);
  m_scene.removeVisual(m_boardObject);
  SAFEDELETE(m_boardObject);
}

void CQuartoDlg::createLight() {
  D3Camera *cam = sCAM();

  D3DLIGHT light    = D3Light::createDefaultLight(D3DLIGHT_DIRECTIONAL);
  light.Diffuse     = colorToColorValue(D3D_WHITE);
  light.Specular    = colorToColorValue(D3D_WHITE);
  light.Ambient     = colorToColorValue(D3D_WHITE);
  light.Direction   = cam->getUp();
  light.Direction.z *= -1;
  m_lightIndex[0]   = m_scene.addLight(light);

  light.Direction   = cam->getDir();
  m_lightIndex[1]   = m_scene.addLight(light);
}

void CQuartoDlg::destroyLight() {
  m_scene.removeLight(m_lightIndex[1]);
  m_scene.removeLight(m_lightIndex[0]);
}

void CQuartoDlg::resetCamera() {
  setCameraPosition(D3DXVECTOR3(-1.325f, -16.106f, 8.512f));
}

// always look directly to center of board
void CQuartoDlg::setCameraPosition(const D3DXVECTOR3 &pos) {
  D3Camera *cam = sCAM();
  D3World bw(m_boardObject->getWorld());
  cam->setLookAt(pos, bw.getPos(),D3DXVECTOR3(0,0,1));
}

void CQuartoDlg::selectField(const Field &f) {
  if(f.isValid()) {
    m_boardObject->markField(f);
  } else {
    m_boardObject->unmarkCurrentField();
  }
}

void CQuartoDlg::selectBrick(int b) {
  if(Brick::isValid(b)) {
    markBrick(b);
  } else {
    unmarkCurrentBrick();
  }
}

void CQuartoDlg::markBrick(int b) {
  m_boardObject->markBrick(b);
}

void CQuartoDlg::unmarkCurrentBrick() {
  m_boardObject->unmarkCurrentBrick();
}

CPoint CQuartoDlg::get3DPanelPoint(CPoint point) const {
  ClientToScreen(&point);
  GetDlgItem(IDC_STATICGAMEWINDOW)->ScreenToClient(&point);
  return point;
}

void CQuartoDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  __super::OnLButtonDown(nFlags, point);
  if(m_state != DLG_IDLE) return;
  if(m_game.isGameOver() || (m_game.getPlayerInTurn() != HUMAN_PLAYER)) {
    return;
  }
  point = get3DPanelPoint(point);
  const int b = getBrickFromPoint(point);
  if(b != NOBRICK) {
    if(m_game.isSelectableBrick(b)) {
      selectBrick(b);
      selectField(NOFIELD);
      return;
    }
  }
  const Field f = getFieldFromPoint(point);
  if(f.isField()) {
#ifdef _DEBUG
    if(nFlags & MK_CONTROL) {
      selectField(f);
      return;
    }
#endif
    if(!m_game.isEmpty(f) || (getSelectedBrick() == NOBRICK)) {
      return;
    }
    selectField(f);
    startTimer(DLG_ONTIMERDOUSERMOVE);
  }
}

int CQuartoDlg::getBrickFromPoint(const CPoint &p) const {
  return m_boardObject->getBrickFromPoint(p, sCAM());
}

Field CQuartoDlg::getFieldFromPoint(const CPoint &p) const {
  return m_boardObject->getFieldFromPoint(p,sCAM());
}

void CQuartoDlg::showEditorInfo() {
  showInfo(_T("%s"), m_editor.toString().cstr());
}

void CQuartoDlg::OnFileNew() {
  newGame(isMenuItemChecked(this,ID_OPTIONS_COLOREDGAME), m_startPlayer);
  m_startPlayer = OPPONENT(m_startPlayer);

  if(m_game.getPlayerInTurn() == HUMAN_PLAYER) {
    Invalidate();
    m_state = DLG_IDLE;
  } else {
    startTimer(DLG_ONTIMERDOCOMPUTERMOVE, 500);
  }
}

const TCHAR *extensions = _T("Quarto-files (*.qrt)\0*.qrt\0Text-files (*.txt)\0*.txt\0\0");

void CQuartoDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = extensions;
  dlg.m_ofn.lpstrTitle = _T("Load game");
  if(dlg.DoModal() != IDOK) {
    return;
  }

  if(_tcsclen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }
  FILE *f = NULL;
  try {
    f = FOPEN(dlg.m_ofn.lpstrFile, _T("r"));
    m_game = Game(readTextFile(f));
    fclose(f); f = NULL;
    resetScene();
    setGameName(dlg.m_ofn.lpstrFile);
  } catch(Exception e) {
    if(f != NULL) {
      fclose(f);
    }
    showException(e);
  }
}

void CQuartoDlg::OnFileSave() {
  if(!isGameTitled()) {
    OnFileSaveAs();
  } else {
    save(m_gameName);
  }
}

void CQuartoDlg::OnFileSaveAs() {
  CFileDialog dlg(FALSE, _T("*.qrt"), m_gameName.cstr());
  dlg.m_ofn.lpstrFilter = extensions;
  dlg.m_ofn.lpstrTitle  = _T("Save game");
  if(dlg.DoModal() != IDOK) {
    return;
  }

  if(_tcsclen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }
  save(dlg.m_ofn.lpstrFile);
}

void CQuartoDlg::save(const String &name) {
  FILE *f = NULL;
  try {
    f = FOPEN(name, _T("w"));
    _ftprintf(f,_T("%s\n"), m_game.toString().cstr());
    fclose(f); f = NULL;
    setGameName(name);
  } catch(Exception e) {
    if(f != NULL) {
      fclose(f);
    }
    showException(e);
  }
}

void CQuartoDlg::OnCancel() {
}

void CQuartoDlg::OnOK() {
}

void CQuartoDlg::OnClose() {
  OnFileExit();
}

void CQuartoDlg::OnFileExit() {
  stopTimer();
  unInitDialog();
  EndDialog(IDOK);
}

void CQuartoDlg::newGame(bool colored, Player startPlayer, const String &name) {
  m_game.newGame(colored, startPlayer);
  resetScene();
  setGameName(name);
}

void CQuartoDlg::executeMove(const Move &m) {
  m_game.executeMove(m);
  updateGraphicsDoingMove(m);
  selectField(NOFIELD);
  selectBrick(NOBRICK);
}

void CQuartoDlg::endGame() {
  TCHAR *text;
  if(m_game.hasWinner()) {
    if(m_game.getPlayerInTurn() == COMPUTER_PLAYER) {
      text = _T("I win!!");
    } else {
      text = _T("You win!!");
    }
    flashWinnerBlocks();
  } else {
    text = _T("Draw!!");
  }
  switch(MessageBox(format(_T("%s\nDo you want to play again?"),text).cstr(), _T("Game over"), MB_YESNO|MB_ICONQUESTION)) {
  case IDYES:
    OnFileNew();
    break;;
  case IDNO:
    break;
  }
}

void CQuartoDlg::resetScene() {
  resetBrickPositions(m_game.isColored());
  const MoveArray &list = m_game.getHistory();
  for(size_t i = 0; i < list.size(); i++) {
    updateGraphicsDoingMove(list[i]);
  }
  selectField(NOFIELD);
  selectBrick(NOBRICK);
}

void CQuartoDlg::resetBrickPositions(bool colored) {
  m_boardObject->resetBrickPositions(colored);
}

void CQuartoDlg::updateGraphicsDoingMove(const Move &m) {
  m_boardObject->setBrickOnField(m.m_brick, m.m_field);
}

void CQuartoDlg::setGameName(const String &name) {
  setWindowText(this, m_gameName = name);
}

void CQuartoDlg::flashWinnerBlocks() {
  const FieldArray wf = m_game.getWinnerFields();
  if(wf.size() != 4) {
    return;
  }
  BrickSet bset;
  for(int f = 0; f < wf.size(); f++) {
    const int b = m_game.getBrickOnField(wf[f]);
    if(b == NOBRICK) {
      return;
    }
    bset.add(b);
  }
  for(int i = 0; i < 8; i++) {
    m_boardObject->setBricksVisible(bset,(i&1)!=0);
    render(SC_RENDER3D|SC_RENDERNOW);
    Sleep(400);
  }
}

Move CQuartoDlg::findMove() {
  theApp.BeginWaitCursor();
  MoveEvaluator eval(m_game);
  int lookahead;
  if(m_game.isColored()) {
    lookahead = (m_game.getBrickCount() < 8) ? 2 : isMenuItemChecked(this,ID_OPTIONS_LEVEL_EXPERT) ? 6 : 4;
  } else {
    lookahead = (m_game.getBrickCount() < 8) ? 2 : isMenuItemChecked(this,ID_OPTIONS_LEVEL_EXPERT) ? 4 : 2;
  }
  const Move m = eval.findMove(lookahead);

  theApp.EndWaitCursor();
  showInfo(_T("Score:%d evalCount:%d"),eval.getBestScore(),eval.getEvalCount());
  return m;
}

void CQuartoDlg::showInfo(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  const String s = vformat(format, argptr);
  va_end(argptr);
  setWindowText(this, IDC_GAMEINFO, s);
}

void CQuartoDlg::OnViewLeft()  { turnBoard(-90); }
void CQuartoDlg::OnViewRight() { turnBoard( 90); }

void CQuartoDlg::turnBoard(int degree) {
  D3Camera *cam = sCAM();
  D3World bw(m_boardObject->getWorld());
  D3World cw(cam->getD3World());
  const D3DXVECTOR3     &axis    = bw.getUp();
  const D3DXQUATERNION   rot     = createRotation(axis, radians(degree));
  const D3DXVECTOR3      bPos    = bw.getPos(), cPos = cw.getPos();
  const D3DXVECTOR3      centerOfRotation(bPos.x, bPos.y, cPos.z);
  cam->setD3World(cw.rotate(rot, centerOfRotation));
}

void CQuartoDlg::OnViewResetView() {
  resetCamera();
}

void CQuartoDlg::OnOptionsLevelExpert() {
  checkMenuItem(this, ID_OPTIONS_LEVEL_BEGINNER, false);
  checkMenuItem(this, ID_OPTIONS_LEVEL_EXPERT  , true );
}

void CQuartoDlg::OnOptionsLevelBeginner() {
  checkMenuItem(this, ID_OPTIONS_LEVEL_BEGINNER, true );
  checkMenuItem(this, ID_OPTIONS_LEVEL_EXPERT  , false);
}

void CQuartoDlg::OnOptionsColoredGame() {
  toggleMenuItem(this, ID_OPTIONS_COLOREDGAME);
  OnFileNew();
}

void CQuartoDlg::OnDumpSetup() {
  const CSize    winSize = getWindowSize(this);
  const D3World &cw      = sCAM()->getD3World();
  const D3World  bw      = m_boardObject->getWorld();
  const String msg = format(_T("WinSize:(%d,%d)\r\nCameara:(Pos:%s,Dir:%s,Up:%s)\r\nBoard:(Pos:%s,Dir:%s,Up:%s)")
                           , winSize.cx, winSize.cy
                           , toString(cw.getPos(), 3).cstr()
                           , toString(cw.getDir(), 3).cstr()
                           , toString(cw.getUp() , 3).cstr()
                           , toString(bw.getPos()   ).cstr()
                           , toString(bw.getDir()   ).cstr()
                           , toString(bw.getUp()    ).cstr()
                           );
  showInformation(msg);
}
