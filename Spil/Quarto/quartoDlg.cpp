#include "stdafx.h"
#include <Random.h>
#include "Quarto.h"
#include "QuartoDlg.h"
#include <D3DGraphics/Cube3D.h>
#include <D3DGraphics/MeshBuilder.h>
#include "AboutBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DECLARE_THISFILE;

CQuartoDlg::CQuartoDlg(CWnd *pParent) : CDialog(CQuartoDlg::IDD, pParent) {
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
    m_initdone             = false;
    m_selectedBrick        = NOBRICK;
    m_startPlayer          = HUMAN_PLAYER;
    m_adjustingCameraFlags = 0;
//    memset(m_brickMarkerTable, 0, sizeof(m_brickMarkerTable));
}

void CQuartoDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQuartoDlg, CDialog)
    ON_WM_QUERYDRAGICON()
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_CLOSE()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
/*
    ON_COMMAND(ID_FILE_NEW              , OnFileNew             )
    ON_COMMAND(ID_FILE_OPEN             , OnFileOpen            )
    ON_COMMAND(ID_FILE_SAVE             , OnFileSave            )
    ON_COMMAND(ID_FILE_SAVEAS           , OnFileSaveAs          )
    ON_COMMAND(ID_FILE_EXIT             , OnFileExit            )
    ON_COMMAND(ID_VIEW_LEFT             , OnViewLeft            )
    ON_COMMAND(ID_VIEW_RIGHT            , OnViewRight           )
    ON_COMMAND(ID_VIEW_RESETVIEW        , OnViewResetView       )
*/

    ON_COMMAND(ID_VIEW_LIGHT1           , OnViewLight1          )
/*
    ON_COMMAND(ID_VIEW_LIGHT2           , OnViewLight2          )
    ON_COMMAND(ID_OPTIONS_LEVEL_BEGINNER, OnOptionsLevelBeginner)
    ON_COMMAND(ID_OPTIONS_LEVEL_EXPERT  , OnOptionsLevelExpert  )
    ON_COMMAND(ID_OPTIONS_COLOREDGAME   , OnOptionsColoredGame  )
*/
    ON_COMMAND(ID_HELP_ABOUTQUARTO      , OnHelpAboutquarto     )
    ON_COMMAND(ID_DUMP_SETUP            , OnDumpSetup           )
    ON_MESSAGE(ID_MSG_REFRESH_VIEW      , OnMsgRefreshView      )
END_MESSAGE_MAP()

#undef checkhres
#define checkhres() CHECKHRESULT(__FILE__,__LINE__,m_d3.getLastError())

HCURSOR CQuartoDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CQuartoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX)    {
    CAboutDlg().DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

BOOL CQuartoDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);           // Set big icon
  SetIcon(m_hIcon, FALSE);      // Set small icon

  m_scene.init(getGameWindow()->m_hWnd);

  try {
    randomize();
    m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
    createScene();
//    OnFileNew();
  } catch(Exception e) {
    Message(_T("%s"),e.what());
    exit(-1);
  }

  m_initdone = true;
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
    CDialog::OnPaint();
    PostMessage(ID_MSG_REFRESH_VIEW);
  }
}

LRESULT CQuartoDlg::OnMsgRefreshView(WPARAM wp, LPARAM lp) {
  CClientDC   dc(getGameWindow());
  const CSize sz = getGameRect().Size();
  dc.FillSolidRect( 0,0, sz.cx, sz.cy, BACKGROUNDCOLOR);
  render();
  return 0;
}

void CQuartoDlg::render() {
  m_scene.render();
#ifdef DEVELOPER_MODE
  showCameraData();
#endif
}

BOOL CQuartoDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CQuartoDlg::createScene() {
//  m_scene.setLightDirection(0, rotate(m_scene.getCameraDir(), m_scene.getCameraRight(), 0.2f));

  createBoard();
//  createBricks();
  resetCamera();
  createLight();
}

void CQuartoDlg::createBoard() {
  m_boardObject = new GameBoardObject(m_scene);
  m_scene.addSceneObject(m_boardObject);
  m_boardCenter = m_boardObject->getPos();
}

void CQuartoDlg::createLight() {
  LIGHT light = m_scene.getDefaultLightParam(D3DLIGHT_DIRECTIONAL);
  D3DXVECTOR3 pos = D3DXVECTOR3(-2*BOARDSIZE+HALFSIZE,BOARDSIZE,HALFSIZE);
  D3DXVECTOR3 dir = unitVector(m_boardCenter - pos);
  light.Diffuse   = colorToColorValue(D3DCOLOR_XRGB(192,192,192));
  light.Position  = pos;
  light.Direction = dir;
  m_scene.setLightParam(light);
  m_scene.setLightEnabled(light.m_lightIndex, true);
}

void CQuartoDlg::toggleLight(int index, bool on) {
  m_scene.setLightEnabled(index, on);
  m_scene.render();
}

void CQuartoDlg::resetCamera() {
  m_scene.setCameraPos(D3DXVECTOR3(0, 13.48f, 9.86f));
  m_scene.setCameraOrientation(D3DXVECTOR3(0,-0.754f, -0.657f), D3DXVECTOR3(0,0,1));
  setWindowSize(this, CSize(947, 614));
}

// always look directly to center of board
void CQuartoDlg::setCameraPosition(const D3DXVECTOR3 &pos) {
  m_scene.setCameraPos(pos);
  m_scene.setCameraLookAt(getBoardCenter());
}

D3DXVECTOR3 CQuartoDlg::getCameraPosition() {
  return m_scene.getCameraPos();
}

#ifdef __HIDE__
void CQuartoDlg::initFieldCenter() {
  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      LPDIRECT3DRMFACE face = m_boardFace[r][c];
      int n = face->GetVertexCount();
      D3DVECTOR middle;
      middle.x = middle.y = middle.z = 0;
      for(int i = 0; i < n; i++) {
        D3DVECTOR vertex, normal;
        face->GetVertex(i,&vertex,&normal);
        middle += vertex;
      }
      middle /= n;
      m_fieldCenter[r][c] = middle;
    }
  }
}
#endif

void CQuartoDlg::selectField(const Field &f) {
  if(f.isValid()) {
    m_boardObject->markField(f);
  } else {
    m_boardObject->unmarkCurrent();
  }
}

#ifdef __HIDE__
void CQuartoDlg::selectBrick(int b) {
  if(m_selectedBrick != NOBRICK) {
    unmarkBrick(m_selectedBrick);
  }
  m_selectedBrick = b;
  if(m_selectedBrick != NOBRICK) {
    markBrick(m_selectedBrick);
  }
}
#endif

#ifdef __HIDE__
void CQuartoDlg::markBrick(int b) {
  m_d3.getSceneFrame()->AddVisual(getBrickMarker(b));
}

void CQuartoDlg::unmarkBrick(int b) {
  m_d3.getSceneFrame()->DeleteVisual(getBrickMarker(b));
}

static void addVertex(LPDIRECT3DRMMESHBUILDER meshBuilder, const D3DVECTOR &p) {
  int i = meshBuilder->AddVertex(p.x,p.y,p.z);
}

LPDIRECT3DRMMESHBUILDER CQuartoDlg::getBrickMarker(int b) {
  if(m_brickMarkerTable[b] == NULL) {
    LPDIRECT3DRMFRAME scene = m_d3.getSceneFrame();
    LPDIRECT3DRMFRAME frame = m_brickFrame[b];

    D3DRMBOX box = m_d3.getBoundingBox(frame);
    D3DVECTOR pos;
    m_brickFrame[b]->GetPosition(scene, &pos);
    box.min += pos;
    box.max += pos;

    D3DVECTOR c1 = createVector(box.min.x,box.min.y,box.min.z);
    D3DVECTOR c2 = createVector(box.max.x,box.min.y,box.min.z);
    D3DVECTOR c3 = createVector(box.max.x,box.min.y,box.max.z);
    D3DVECTOR c4 = createVector(box.min.x,box.min.y,box.max.z);

    LPDIRECT3DRMMESHBUILDER meshBuilder = m_d3.createMeshBuilder(format(_T("brick %d"), b));

    addVertex(meshBuilder, c1);
    addVertex(meshBuilder, c2);
    addVertex(meshBuilder, c3);
    addVertex(meshBuilder, c4);
    meshBuilder->AddNormal(0,1,0);

    LPDIRECT3DRMFACE face;
    CHECK3DRESULT(meshBuilder->CreateFace(&face));
    face->AddVertexNormalIndexed(3,0);
    face->AddVertexNormalIndexed(2,0);
    face->AddVertexNormalIndexed(1,0);
    face->AddVertexNormalIndexed(0,0);

    face->SetColor(RGBA_MAKE(250,0,0,255));
    face->Release();

    CHECK3DRESULT(meshBuilder->SetQuality(D3DRMFILL_SOLID|D3DRMSHADE_GOURAUD|D3DRMLIGHT_ON));

    m_brickMarkerTable[b] = meshBuilder;
  }
  return m_brickMarkerTable[b];
}

void CQuartoDlg::createBricks() {
  m_brickMaterial[0] = createMaterial(RGB(0,0,155), RGB(0,0,255));
  m_brickMaterial[1] = createMaterial(RGB(155,0,0), RGB(255,0,0));

  for(int i = 0; i < FIELDCOUNT; i++) {
    BYTE attr = Brick::attr[i];
    LPDIRECT3DRMMESHBUILDER m = createBrick(ISBIG(attr),ISBLACK(attr),ISSQUARE(attr),ISWITHTOP(attr));
    LPDIRECT3DRMFRAME frame = m_d3.createFrame(_T("BRICK"),m_d3.getSceneFrame());
    frame->AddVisual(m);
    m_d3.addInnerSide(frame);
    frame->SetMaterialMode(D3DRMMATERIAL_FROMMESH);
    m_brickFrame[i] = frame;
//    m_d3.createShadow(m_brickFrame[i],m_light);
  }
}

static const Point2D smallProfile1[] = { 
  Point2D( 0   ,0   ),
  Point2D( 0.6 ,0   ),
  Point2D( 0.6 ,1   ),
  Point2D( 0   ,1   )
};

static const Point2D smallProfile2[] = { 
  Point2D( 0    ,0  ),
  Point2D( 0.6  ,0  ),
  Point2D( 0.6  ,0.6),
  Point2D( 0.75 ,0.6),
  Point2D( 0.75 ,1  ),
  Point2D( 0    ,1  )
};

static const Point2D bigProfile1[] = { 
  Point2D( 0   ,0   ),
  Point2D( 0.6 ,0   ),
  Point2D( 0.6 ,2   ),
  Point2D( 0   ,2   )
};

static const Point2D bigProfile2[] = { 
  Point2D( 0   ,0   ),
  Point2D( 0.6 ,0   ),
  Point2D( 0.6 ,1.6 ),
  Point2D( 0.75,1.6 ),
  Point2D( 0.75,2   ),
  Point2D( 0   ,2   )
};

LPDIRECT3DRMMATERIAL CQuartoDlg::createMaterial(COLORREF emissive, COLORREF specular) {
  LPDIRECT3DRMMATERIAL material = m_d3.createMaterial(5);
  setMaterialColor(material,emissive,specular);
  return material;
}

void CQuartoDlg::setMaterialColor(LPDIRECT3DRMMATERIAL material, COLORREF emissive, COLORREF specular) {
  float r = (float)GetRValue(emissive) / 255;
  float g = (float)GetGValue(emissive) / 255;
  float b = (float)GetBValue(emissive) / 255;
  material->SetEmissive(r,g,b);
  r = (float)GetRValue(specular) / 255;
  g = (float)GetGValue(specular) / 255;
  b = (float)GetBValue(specular) / 255;
  material->SetSpecular(r,g,b);
}

LPDIRECT3DRMMESHBUILDER CQuartoDlg::createBrick(bool big, bool black,  bool square, bool top) {
  Profile profile;
  if(big){
    if(top) {
      profile = createProfile(bigProfile2,ARRAYSIZE(bigProfile2));
    } else {
      profile = createProfile(bigProfile1,ARRAYSIZE(bigProfile1));
    }
  } else {
    if(top) {
      profile = createProfile(smallProfile2,ARRAYSIZE(smallProfile2));
    } else {
      profile = createProfile(smallProfile1,ARRAYSIZE(smallProfile1));
    }
  }

  ProfileRotationParameters parm;
  parm.m_alignx     = 0;
  parm.m_aligny     = 1;
  parm.m_rotateAxis = 1;
  parm.m_rad        = 2*M_PI;
  parm.m_edgeCount  = square?4:20;
  parm.m_smoothness = square?0:ROTATESMOOTH;
  LPDIRECT3DRMMESHBUILDER mesh = m_d3.rotateProfile(profile,parm);
  mesh->SetMaterial(m_brickMaterial[black?1:0]);

  return mesh;
}

Profile CQuartoDlg::createProfile(const Point2D *data, int n) {
  Point2DArray points;
  for(int i = 0; i < n; i++) {
    points.add(data[i]);
  }
  return createProfile(points);
}

Profile CQuartoDlg::createProfile(const Point2DArray &points) {
  Profile result;
  ProfilePolygon polygon;
  ProfileCurve curve(TT_PRIM_LINE);
  polygon.m_start = points[0];
  polygon.m_closed = false;
  for(int i = 1; i < points.size(); i++) {
    curve.addPoint(points[i]);
  }
  polygon.addCurve(curve);
  polygon.reverseOrder();
  result.addPolygon(polygon);
  
  return result;
}

int CQuartoDlg::getBrickFromPoint(const CPoint &p) const {
  LPDIRECT3DRMFRAME frame = ((C3D&)m_d3).pickFrame(p);
  if(frame == NULL) {
    return NOBRICK;
  }
  for(int b = 0; b < FIELDCOUNT; b++) {
    if(m_brickFrame[b] == frame) {
      return b;
    }
  }
  return NOBRICK;
}
#endif

Field CQuartoDlg::getFieldFromPoint(const CPoint &p) const {
  CPoint np = p;
  ClientToScreen(&np);
  getGameWindow()->ScreenToClient(&np);
  return m_boardObject->getFieldFromPoint(np);
/*
  Field f;
  for(f.m_row = 0; f.m_row < ROWCOUNT; f.m_row++) {
    for(f.m_col = 0; f.m_col < COLCOUNT; f.m_col++) {
      if(m_boardFace[f.m_row][f.m_col] == e.m_face) {
        return f;
      }
    }
  }
*/
  return NOFIELD;
}

void CQuartoDlg::OnRButtonDown(UINT nFlags, CPoint point) {
  CDialog::OnRButtonDown(nFlags, point);
  if(nFlags & (MK_CONTROL|MK_SHIFT)) {
    startAdjustCamera(point, nFlags);
  }
}

void CQuartoDlg::OnRButtonUp(UINT nFlags, CPoint point) {
  endAdjustCamera();
  CDialog::OnRButtonUp(nFlags, point);
}

void CQuartoDlg::OnMouseMove(UINT nFlags, CPoint point) {
  CDialog::OnMouseMove(nFlags, point);
  if(nFlags & MK_RBUTTON) {
    if(nFlags & MK_CONTROL) {
      if((m_adjustingCameraFlags & MK_CONTROL) == 0) {
        startAdjustCamera(point, MK_CONTROL);
      } else {
        adjustCameraAngle(point);
        showCursor(false);
      }
      return;
    } if(nFlags & MK_SHIFT) {
      if((m_adjustingCameraFlags & MK_SHIFT) == 0) {
        startAdjustCamera(point, MK_SHIFT);
      } else {
        adjustCameraPos(point);
        showCursor(false);
      }
      return;
    }
  }
  endAdjustCamera();
}

BOOL CQuartoDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  float factor = (zDelta > 0) ? 0.3f : -0.3f;
  D3DXVECTOR3 dir = m_scene.getCameraDir();
  m_scene.setCameraPos(getCameraPosition() + factor * dir);
  render();
  return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CQuartoDlg::startAdjustCamera(const CPoint &p, int flags) {
  m_adjustingCameraFlags = flags;
  m_rbuttonDownPoint     = p;
  m_startCamPos          = getCameraPosition();
  ::SetCapture(*this);
  showCursor(false);
}

void CQuartoDlg::endAdjustCamera() {
  m_adjustingCameraFlags = 0;
  ReleaseCapture();
  showCursor(true);
}

void CQuartoDlg::showCursor(bool show) {
  if(show) {
    while(ShowCursor(TRUE) <= 0);
  } else {
    while(ShowCursor(FALSE) > 0);
  }
}

void CQuartoDlg::adjustCameraAngle(const CPoint &p) {
  const D3DXVECTOR3  bc = getBoardCenter();
  D3DXVECTOR3        cp = getCameraPosition();
  D3DXVECTOR3        cd = m_scene.getCameraDir();
  D3DXVECTOR3        cu = m_scene.getCameraUp();

  const D3DXVECTOR3 cP0Bc    = m_startCamPos - bc;
  const D3DXVECTOR3 cp0BcWCT(cP0Bc.z, cP0Bc.x, cP0Bc.y);

  Spherical sph0(cp0BcWCT.x, cp0BcWCT.y, cp0BcWCT.z);
  Spherical sph(sph0);

  const double dx = p.x - m_rbuttonDownPoint.x;
  const double dy = p.y - m_rbuttonDownPoint.y;

  sph.theta += dx / 400;
  sph.fi    -= dy / 400;
//  sph.fi = minMax((double)sph.fi, GRAD2RAD(25), GRAD2RAD(70));

  Point3D cP1WCT = sph;

  D3DXVECTOR3 cP1((float)cP1WCT.y, (float)cP1WCT.z, (float)cP1WCT.x);
  D3DXVECTOR3 cP1Bc = cP1 + bc;

  setCameraPosition(cP1Bc);
  render();
}

#ifdef DEVELOPER_MODE
void CQuartoDlg::showCameraData() {
  const D3DXVECTOR3  bc = getBoardCenter();
  D3DXVECTOR3        cp = getCameraPosition();
  D3DXVECTOR3        cd = m_scene.getCameraDir();
  D3DXVECTOR3        cu = m_scene.getCameraUp();

  showInfo(_T("BoardCenter:%s\r\nCamera(Pos:%s, Dir:%s, Up%s")
          ,toString(bc).cstr()
          ,toString(cp).cstr()
          ,toString(cd).cstr()
          ,toString(cu).cstr()
          );
}
#endif

void CQuartoDlg::adjustCameraPos(const CPoint &p) {
  D3DXVECTOR3        cp = getCameraPosition();
  D3DXVECTOR3        cr = m_scene.getCameraRight();
  D3DXVECTOR3        cu = m_scene.getCameraUp();

  const float dx = (float)(p.x - m_rbuttonDownPoint.x)/10.0f;
  const float dy = (float)(p.y - m_rbuttonDownPoint.y)/10.0f;
  m_scene.setCameraPos(m_startCamPos + dx * cr - dy * cu);
  render();
}

#ifdef TESTMARKFIELD
void CQuartoDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  if (m_boardObject->hasCurrentField()) {
    Field f = m_boardObject->getCurrentField();
    f.m_col++;
    if(f.isValid()) {
      m_boardObject->markField(f);
    } else {
      f.m_col = 0;
      f.m_row++;
      if(f.isValid()) {
        m_boardObject->markField(f);
      } else {
        m_boardObject->unmarkCurrent();
      }
    }
  } else {
    Field f;
    f.m_row = f.m_col = 0;
    m_boardObject->markField(f);
  }
  render();
}
#else
void CQuartoDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  if(m_game.isGameOver() || m_game.getPlayerInTurn() != HUMAN_PLAYER) {
    return;
  }
/*
  const int b = getBrickFromPoint(point);
  if(b != NOBRICK) {
    if(m_game.isSelectableBrick(b)) {
      selectBrick(b);
      selectField(NOFIELD);
      m_d3.paint();
      return;
    }
  }
*/
  const Field f = getFieldFromPoint(point);
//  if(f.isField()) {
//    if(!m_game.isEmpty(f) || (getSelectedBrick() == NOBRICK)) {
//      return;
//    }
    selectField(f);
    render();
/*
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
*/
}
#endif

#ifdef _HIDE_
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
    refreshGraphics();
    setGameName(dlg.m_ofn.lpstrFile);
    render();
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
#endif

void CQuartoDlg::OnCancel() {
}

void CQuartoDlg::OnOK() {
}

void CQuartoDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CQuartoDlg::OnClose() {
  OnFileExit();
}
/*
void CQuartoDlg::newGame(bool colored, Player startPlayer, const String &name) {
  m_game.newGame(colored, startPlayer);
  refreshGraphics();
  setGameName(name);
  render();
}

void CQuartoDlg::executeMove(const Move &m) {
  m_game.executeMove(m);
  updateGraphicsDoingMove(m);
  selectField(NOFIELD);
  selectBrick(NOBRICK);
  render();
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
*/
#ifdef _HIDE_
void CQuartoDlg::refreshGraphics() {
  resetBrickPositions(m_game.isColored());
  const CompactArray<Move> &list = m_game.getHistory();
  for(int i = 0; i < list.size(); i++) {
    updateGraphicsDoingMove(list[i]);
  }
  selectField(NOFIELD);
  selectBrick(NOBRICK);
}

void CQuartoDlg::resetBrickPositions(bool colored) {
  selectBrick(NOBRICK);
  LPDIRECT3DRMFRAME scene = m_d3.getSceneFrame();
  for(int i = 0; i < FIELDCOUNT; i++) {
    LPDIRECT3DRMFRAME b = m_brickFrame[i];
    m_boardFrame->DeleteChild(b);
    m_d3.getSceneFrame()->AddChild(b);
    if(colored) {
      b->SetPosition(scene,(float)((i%8)*2+0.85),(float)0.01,(float)((i/8)*17.3-0.6) );
    } else {
      b->SetPosition(scene,(float)((i%8)*2+0.85),(float)0.01,(float)((i/8)*2-0.8) );
    }
    b->SetOrientation(scene,0,0,1, 0,1,0);
  }
}

void CQuartoDlg::updateGraphicsDoingMove(const Move &m) {
  LPDIRECT3DRMFRAME b = m_brickFrame[m.m_brick];
  m_d3.getSceneFrame()->DeleteChild(b);
  m_boardFrame->AddChild(b);
  const D3DVECTOR &pos = getFieldCenter(m.m_field);
  b->SetPosition(m_boardFrame,pos.x,pos.y,pos.z);
}

void CQuartoDlg::setGameName(const String &name) {
  m_gameName = name;
  SetWindowText(name.cstr());
}

void CQuartoDlg::flashWinnerBlocks() {
  const FieldArray wf = m_game.getWinnerFields();
  if(wf.size() != 4) {
    return;
  }
  CompactArray<LPDIRECT3DRMFRAME> wba;
  for(int f = 0; f < wf.size(); f++) {
    const int b = m_game.getBrickOnField(wf[f]);
    if(b == NOBRICK) {
      return;
    }
    wba.add(m_brickFrame[b]);
  }
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < wba.size(); j++) {
      m_boardFrame->DeleteChild(wba[j]);
    }
    m_d3.paint();
    Sleep(300);
    for(j = 0; j < wba.size(); j++) {
      m_boardFrame->AddChild(wba[j]);
    }
    m_d3.paint();
    Sleep(300);
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
#endif

void CQuartoDlg::showInfo(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  const String s = vformat(format, argptr);
  va_end(argptr);
  GetDlgItem(IDC_GAMEINFO)->SetWindowText(s.cstr());
}

#ifdef __HIDE__
void CQuartoDlg::OnViewLeft() {
  turnBoard(-90);
}

void CQuartoDlg::OnViewRight() {
  turnBoard(90);
}

void CQuartoDlg::turnBoard(int degree) {
  LPDIRECT3DRMFRAME scene = m_d3.getSceneFrame();
  D3DVECTOR dir,up,axis;
  axis.x = axis.z = 0;
  axis.y = 1;
  m_boardFrame->GetOrientation(scene,&dir,&up);
  dir = rotate(dir,axis,GRAD2RAD(degree));
  m_boardFrame->SetOrientation(scene,dir.x,dir.y,dir.z,up.x,up.y,up.z);
  m_d3.paint();
}

void CQuartoDlg::OnViewResetView() {
  resetCamera();
  m_d3.paint();
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
#endif

void CQuartoDlg::OnViewLight1() {
  toggleLight(0, toggleMenuItem(this, ID_VIEW_LIGHT1));
}

#ifdef _HIDE_
void CQuartoDlg::OnViewLight2() {
  toggleLight(1, toggleMenuItem(this, ID_VIEW_LIGHT2));
}
#endif

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
