#include "stdafx.h"
#include <Random.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/D3CoordinateSystem.h>
#include <D3DGraphics/D3Math.h>
#include "QuartoDlg.h"
#include "AboutBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DECLARE_THISFILE;

CQuartoDlg::CQuartoDlg(CWnd *pParent) : CDialog(CQuartoDlg::IDD, pParent) {
  m_hIcon           = theApp.LoadIcon(IDR_MAINFRAME);
  m_startPlayer     = HUMAN_PLAYER;
}

void CQuartoDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQuartoDlg, CDialog)
    ON_WM_QUERYDRAGICON()
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_CLOSE()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDBLCLK()
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
    ON_COMMAND(ID_HELP_ABOUTQUARTO      , OnHelpAboutquarto     )
    ON_COMMAND(ID_DUMP_SETUP            , OnDumpSetup           )
    ON_MESSAGE(ID_MSG_RENDER            , OnMsgRender           )
END_MESSAGE_MAP()

HCURSOR CQuartoDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CQuartoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX)    {
    CAboutDlg().DoModal();
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

  m_scene.init(get3DWindow()->m_hWnd);
  m_editor.init(this);

  try {
    randomize();
    m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
    createScene();
    OnFileNew();
  } catch(Exception e) {
    Message(_T("%s"), e.what());
    exit(-1);
  }
  return TRUE;  // return TRUE  unless you set the focus to a control
}

#define BACKGROUNDCOLOR RGB(153,217,234)

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
    render(RENDER_ALL);
  }
}

LRESULT CQuartoDlg::OnMsgRender(WPARAM wp, LPARAM lp) {
  BYTE flags = (BYTE)wp;
  if(flags & RENDER_3D) {
    m_scene.render();
  }
  if(flags & RENDER_INFO) {
    if(m_editor.isEnabled()) {
      showEditorInfo();
    }
  }
  return 0;
}

BOOL CQuartoDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  if(m_editor.PreTranslateMessage(pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CQuartoDlg::createScene() {
  createBoard();
  resetCamera();
  createLight();
}

void CQuartoDlg::createBoard() {
  m_boardObject = new GameBoardObject(m_scene);
  m_scene.addSceneObject(m_boardObject);
}

void CQuartoDlg::createLight() {
  D3DLIGHT light = D3Scene::getDefaultLight();
  light.Diffuse     = colorToColorValue(D3D_WHITE);
  light.Specular    = colorToColorValue(D3D_WHITE);
  light.Ambient     = colorToColorValue(D3D_WHITE);
  light.Direction   = m_scene.getCameraUp();
  light.Direction.y *= -1;
  m_scene.addLight(light);

  light.Direction   = m_scene.getCameraDir();
  m_scene.addLight(light);
}

void CQuartoDlg::resetCamera() {
  m_scene.setCameraPos(D3DXVECTOR3(-1.325f, 8.512f, -16.106f));
  D3DXVECTOR3 camDir = unitVector(D3DXVECTOR3(0, -0.455f, 0.891f));
  D3DXVECTOR3 camUp(0,0.891f,0.455f);
  camUp -= camDir * (camDir*camUp);
  camUp = unitVector(camUp);
//  float s = camDir * camUp;

  m_scene.setCameraOrientation(camDir,camUp);
  setWindowSize(this, CSize(947, 614));
}

// always look directly to center of board
void CQuartoDlg::setCameraPosition(const D3DXVECTOR3 &pos) {
  m_scene.setCameraPos(pos);
  m_scene.setCameraLookAt(m_boardObject->getPos());
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

CPoint CQuartoDlg::get3DPanelPoint(CPoint point, bool screenRelative) const {
  if(!screenRelative) {
    ClientToScreen(&point);
  }
  ((CQuartoDlg*)this)->get3DWindow()->ScreenToClient(&point);
  return point;
}

void CQuartoDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  __super::OnLButtonDown(nFlags, point);
  point = get3DPanelPoint(point, false);
  if(m_game.isGameOver() || (m_game.getPlayerInTurn() != HUMAN_PLAYER)) {
    return;
  }
  const int b = getBrickFromPoint(point);
  if(b != NOBRICK) {
    if(m_game.isSelectableBrick(b)) {
      selectBrick(b);
      selectField(NOFIELD);
      render(RENDER_3D);
      return;
    }
  }
  const Field f = getFieldFromPoint(point);
  if(f.isField()) {
#ifdef _DEBUG
    if(nFlags & MK_CONTROL) {
      selectField(f);
      render(RENDER_3D);
      return;
    }
#endif
    if(!m_game.isEmpty(f) || (getSelectedBrick() == NOBRICK)) {
      return;
    }
    selectField(f);
    m_scene.render();

    Sleep(500);
    const Move move(getSelectedField(), getSelectedBrick());
    executeMove(move);

    if(m_game.isGameOver()) {
      endGame();
      return;
    }
    Sleep(500);
    executeMove(findMove());
    if(m_game.isGameOver()) {
      endGame();
    }
  }
}

void CQuartoDlg::OnRButtonDblClk(UINT nFlags, CPoint point) {
  if ((nFlags&MK_CONTROL) && (nFlags&MK_SHIFT)) {
    setEditMode(!m_editor.isEnabled());
    render(RENDER_3D);
  }
  __super::OnRButtonDblClk(nFlags, point);
}

int CQuartoDlg::getBrickFromPoint(const CPoint &p) const {
  return m_boardObject->getBrickFromPoint(p);
}

Field CQuartoDlg::getFieldFromPoint(const CPoint &p) const {
  return m_boardObject->getFieldFromPoint(p);
}

void CQuartoDlg::showEditorInfo() {
  showInfo(_T("%s"), m_editor.toString().cstr());
}

void CQuartoDlg::OnFileNew() {
  newGame(isMenuItemChecked(this,ID_OPTIONS_COLOREDGAME), m_startPlayer);
  m_startPlayer = OPPONENT(m_startPlayer);

  if(m_game.getPlayerInTurn() == HUMAN_PLAYER) {
    Invalidate();
  } else {
    executeMove(findMove());
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
    render(RENDER_3D);
  } catch(Exception e) {
    if(f != NULL) {
      fclose(f);
    }
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
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
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
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
  m_editor.close();
  EndDialog(IDOK);
}

void CQuartoDlg::newGame(bool colored, Player startPlayer, const String &name) {
  m_game.newGame(colored, startPlayer);
  resetScene();
  setGameName(name);
  render(RENDER_3D);
}

void CQuartoDlg::executeMove(const Move &m) {
  m_game.executeMove(m);
  updateGraphicsDoingMove(m);
  selectField(NOFIELD);
  selectBrick(NOBRICK);
  m_scene.render();
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
    m_scene.render();
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

void CQuartoDlg::showInfo(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  const String s = vformat(format, argptr);
  va_end(argptr);
  setWindowText(this, IDC_GAMEINFO, s);
}

void CQuartoDlg::OnViewLeft() {
  turnBoard(-90);
}

void CQuartoDlg::OnViewRight() {
  turnBoard(90);
}

void CQuartoDlg::turnBoard(int degree) {
  const D3DXVECTOR3     &axis    = m_boardObject->getUp();
  const D3DXVECTOR3     &origo   = m_boardObject->getPos();
  D3PosDirUpScale        camPDUS = m_scene.getCameraPDUS();
  const D3DXMATRIX       m       = createRotationMatrix(axis, GRAD2RAD(degree));
  const D3DXVECTOR3      newPos  = m * (camPDUS.getPos()-origo) + origo;
  const D3DXVECTOR3      newDir  = m * camPDUS.getDir();
  const D3DXVECTOR3      newUp   = m * camPDUS.getUp();
  m_scene.setCameraPDUS(camPDUS.setPos(newPos).setOrientation(newDir, newUp));
  render(RENDER_3D);
}

void CQuartoDlg::OnViewResetView() {
  resetCamera();
  render(RENDER_3D);
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

void CQuartoDlg::OnHelpAboutquarto() {
  CAboutDlg().DoModal();
}

void CQuartoDlg::OnDumpSetup() {
  const CSize winSize = getWindowSize(this);
  D3DXVECTOR3 camPos  = m_scene.getCameraPos();
  D3DXVECTOR3 dir     = m_scene.getCameraDir();
  D3DXVECTOR3 up      = m_scene.getCameraUp();
  const String msg = format(_T("WinSize:(%d,%d)\r\nCameara:(Pos:%s,Dir:%s,Up:%s)\r\nBoard:(Pos:%s,Dir:%s,Up:%s)")
                           , winSize.cx, winSize.cy
                           , toString(camPos, 3).cstr()
                           , toString(dir   , 3).cstr()
                           , toString(up    , 3).cstr()
                           , toString(m_boardObject->getPos()).cstr()
                           , toString(m_boardObject->getDir()).cstr()
                           , toString(m_boardObject->getUp() ).cstr()
                           );
  MessageBox(msg.cstr(), _T("Info"));
}
