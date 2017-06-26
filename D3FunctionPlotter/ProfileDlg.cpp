#include "stdafx.h"
#include "ProfileDlg.h"
#include "ProfilePainter.h"
//#include "SelectGlyphDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CProfileDlg::CProfileDlg(const Profile &profile, CWnd* pParent) : CDialog(CProfileDlg::IDD, pParent), m_profile(profile) {
  ProfileRotationParameters parameters;
  m_degree           = RAD2GRAD(parameters.m_rad);
  m_normalSmooth     = parameters.m_smoothness & NORMALSMOOTH;
  m_rotateSmooth     = parameters.m_smoothness & ROTATESMOOTH;
  m_edgeCount        = parameters.m_edgeCount;
  m_rotateAxis       = parameters.m_rotateAxis;
  m_3dmode           = 0;
  m_3DObject         = NULL;
  m_viewport         = NULL;
  m_currentDrawTool  = NULL;
  m_lastSavedProfile = m_profile;
}

CProfileDlg::~CProfileDlg() {
  setCurrentDrawTool(NULL);
  set3DObject(NULL);
  destroyViewport();
  destroyWorkBitmap();
  destroyBitmapCache();
}

void CProfileDlg::DoDataExchange(CDataExchange* pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_DEGREES, m_degree);
    DDX_Check(pDX, IDC_CHECK_NORMALSMOOTH, m_normalSmooth);
    DDX_Check(pDX, IDC_CHECK_ROTATESMOOTH, m_rotateSmooth);
    DDX_Text( pDX, IDC_EDIT_EDGECOUNT    , m_edgeCount);
    DDV_MinMaxInt(pDX, m_edgeCount, 1, 100);
    DDX_Radio(pDX, IDC_RADIO_ROTATEXAXIS, m_rotateAxis);
    DDX_Radio(pDX, IDC_RADIO_ROTATE, m_3dmode);
}

BEGIN_MESSAGE_MAP(CProfileDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEWHEEL()
    ON_WM_HSCROLL()
    ON_COMMAND(     ID_FILE_NEW              , OnFileNew                  )
    ON_COMMAND(     ID_FILE_OPEN             , OnFileOpen                 )
    ON_COMMAND(     ID_FILE_SAVE             , OnFileSave                 )
    ON_COMMAND(     ID_FILE_SAVE_AS          , OnFileSaveAs               )
    ON_COMMAND(     ID_FILE_SELECTFROMFONT   , OnFileSelectFromFont       )
    ON_COMMAND(     ID_EDIT_CUT              , OnEditCut                  )
    ON_COMMAND(     ID_EDIT_COPY             , OnEditCopy                 )
    ON_COMMAND(     ID_EDIT_PASTE            , OnEditPaste                )
    ON_COMMAND(     ID_EDIT_DELETE           , OnEditDelete               )
    ON_COMMAND(     ID_EDIT_CONNECT          , OnEditConnect              )
    ON_COMMAND(     ID_EDIT_INVERTNORMALS    , OnEditInvertnormals        )
    ON_COMMAND(     ID_EDIT_MIRROR_HORIZONTAL, OnEditMirrorHorizontal     )
    ON_COMMAND(     ID_EDIT_MIRROR_VERTICAL  , OnEditMirrorVertical       )
    ON_COMMAND(     ID_VIEW_SHOWPOINTS       , OnViewShowPoints           )
    ON_COMMAND(     ID_VIEW_SHOWNORMALS      , OnViewShowNormals          )
    ON_COMMAND(     ID_VIEW_AUTO_UPDATE_3D   , OnViewAutoUpdate3D         )
    ON_COMMAND(     ID_TOOLS_LINE            , OnToolsLine                )
    ON_COMMAND(     ID_TOOLS_BEZIERCURVE     , OnToolsBezierCurve         )
    ON_COMMAND(     ID_TOOLS_RECTANGLE       , OnToolsRectangle           )
    ON_COMMAND(     ID_TOOLS_POLYGON         , OnToolsPolygon             )
    ON_COMMAND(     ID_TOOLS_ELLIPSE         , OnToolsEllipse             )
    ON_COMMAND(     ID_TOOLS_SELECT          , OnToolsSelect              )
    ON_BN_CLICKED(  IDC_RADIO_ROTATE         , OnRadioRotate              )
    ON_BN_CLICKED(  IDC_RADIO_STRETCH        , OnRadioStretch             )
    ON_BN_CLICKED(  IDC_RADIO_ROTATEXAXIS    , OnRadioRotatexaxis         )
    ON_BN_CLICKED(  IDC_RADIO_ROTATEYAXIS    , OnRadioRotateyaxis         )
    ON_BN_CLICKED(  IDC_BUTTON_REFRESH       , OnButtonRefresh            )
    ON_BN_CLICKED(  IDC_CHECK_ROTATESMOOTH   , OnCheckRotateSmooth        )
    ON_BN_CLICKED(  IDC_CHECK_NORMALSMOOTH   , OnCheckNormalSmooth        )
    ON_EN_SETFOCUS( IDC_EDIT_DEGREES         , OnSetfocusEditDegrees      )
    ON_EN_KILLFOCUS(IDC_EDIT_DEGREES         , OnKillfocusEditDegrees     )
    ON_EN_KILLFOCUS(IDC_EDIT_EDGECOUNT       , OnKillfocusEditEdgeCount   )
    ON_EN_SETFOCUS( IDC_EDIT_EDGECOUNT       , OnSetfocusEditEdgeCount    )
END_MESSAGE_MAP()

static LOGFONT makeDefaultFont() {
  CFont f;
  f.CreateFont( 10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                         DEFAULT_PITCH | FF_MODERN,
                         _T("Times new roman")
                        );
  LOGFONT lf;
  f.GetLogFont(&lf);
  f.DeleteObject();
  return lf;
}

BOOL CProfileDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_PROFILE));
  m_logFont  = makeDefaultFont();
  UpdateData(false);
  createWorkBitmap();
  CWnd *wnd3d = GetDlgItem(IDC_STATIC_PROFILEIMAGE3D);
  m_scene.init(*wnd3d);
  createBitmapCache(*GetDlgItem(IDC_STATIC_DEBUG3DFRAME));
  refresh3d();
  setCurrentDrawToolId(ID_TOOLS_LINE);
  CSliderCtrl *slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEGREE);
  slider->SetRange(0,360);
  m_testBitmap.LoadBitmap(IDB_TESTBITMAP);
  showSliderPos();
  return FALSE;
}

void CProfileDlg::createWorkBitmap() {
  destroyWorkBitmap();
  CClientDC screen(this);
  GetDlgItem(IDC_STATIC_PROFILEIMAGE2D)->GetClientRect(&m_workRect);
  int w = m_workRect.Width();
  int h = m_workRect.Height();
  m_workDC.CreateCompatibleDC(&screen);
  m_workBitmap.CreateBitmap(w,h,screen.GetDeviceCaps(PLANES),screen.GetDeviceCaps(BITSPIXEL),NULL);
  m_workBitmap.SetBitmapDimension(w,h);
  m_workDC.SelectObject(m_workBitmap);
  initViewport();
}

void CProfileDlg::destroyWorkBitmap() {
  if(m_workBitmap.m_hObject != NULL) {
    m_workBitmap.DeleteObject();
  }
  if(m_workDC.m_hDC != NULL) {
    m_workDC.DeleteDC();
  }
}

void CProfileDlg::initViewport() {
  Rectangle2D profileRect = m_profile.getBoundingBox();
  if(profileRect.getWidth() == 0 || profileRect.getHeight() == 0) {
    profileRect = Rectangle2D(0,0,1,1);
  } else {
    profileRect.m_x -= profileRect.getWidth()/20;
    profileRect.m_y -= profileRect.getHeight()/20;
    profileRect.m_w += profileRect.getWidth()/10;
    profileRect.m_h += profileRect.getHeight()/10;
  }

  profileRect.m_y += profileRect.m_h;
  profileRect.m_h *= -1;

  destroyViewport();

  m_viewport = new Viewport2D(m_workDC,profileRect,m_workRect,true);
  m_viewport->clear(RGB(255,255,255));
}

void CProfileDlg::destroyViewport() {
  if(m_viewport != NULL) {
    delete m_viewport;
    m_viewport = NULL;
  }
}

void CProfileDlg::resetView() {
  m_lastSavedProfile = m_profile;
  initViewport();
  setProfileName(m_profile.m_name);
  m_currentDrawTool->unselectAll();
  m_currentDrawTool->initState();
  refresh2d();
  refresh3d();
  repaintScreen();
}

void CProfileDlg::OnPaint() {
  __super::OnPaint();
  repaintScreen(); // refresh(CPaintDC(GetDlgItem(IDC_STATIC_PROFILEIMAGE2D)));
}

void CProfileDlg::repaintScreen() {
  repaint2d(CClientDC(GetDlgItem(IDC_STATIC_PROFILEIMAGE2D)));
  repaint3d();
  enableMenuItem(this,ID_EDIT_CONNECT          ,m_currentDrawTool->canConnect());
  enableMenuItem(this,ID_EDIT_INVERTNORMALS    ,m_currentDrawTool->canInvertNormals());
  enableMenuItem(this,ID_EDIT_MIRROR_HORIZONTAL,m_currentDrawTool->canMirror());
  enableMenuItem(this,ID_EDIT_MIRROR_VERTICAL  ,m_currentDrawTool->canMirror());
  enableMenuItem(this,ID_EDIT_DELETE           ,m_currentDrawTool->canDelete());
  enableMenuItem(this,ID_EDIT_COPY             ,m_currentDrawTool->canCopy());
  enableMenuItem(this,ID_EDIT_CUT              ,m_currentDrawTool->canCut());
}

void CProfileDlg::refresh2d() {
  m_currentDrawTool->repaintProfile();
}

void CProfileDlg::repaint2d(CDC &dc) {
  dc.BitBlt(0,0,m_workRect.Width(),m_workRect.Height(),&m_workDC,0,0,SRCCOPY);
}

void CProfileDlg::repaint3d() {
  if(isMenuItemChecked(this,ID_VIEW_AUTO_UPDATE_3D)) {
    refresh3d();
  }
  m_scene.render();
}

void CProfileDlg::refresh3d() {
  if(m_profile == m_lastRotatedProfile && getRotateParameters() == m_lastRotationParameters && m_3dmode == m_last3dmode) {
    return;
  }
  if(!validate()) {
    return;
  }

  switch(m_3dmode) {
  case 0:
    rotateProfile();
    break;
  case 1:
    stretchProfile();
    break;
  }
  m_lastRotatedProfile     = m_profile;
  m_lastRotationParameters = getRotateParameters();
  m_last3dmode             = m_3dmode;
}

void CProfileDlg::rotateProfile() {
  try {
    if(!m_profile.isEmpty()) {
      LPD3DXMESH mesh = ::rotateProfile(m_scene, m_profile, getRotateParameters(), true);
      set3DObject(new SceneObjectWithMesh(m_scene, mesh));
    }
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CProfileDlg::stretchProfile() {
/*
  try {
    m_imageFrame->AddVisual(m_d3.stretchProfile(m_profile,getStretchParameters()));
    m_d3.addInnerSide(m_imageFrame);
  } catch(Exception e) {
    MessageBox(e.what());
  }
*/
}

void CProfileDlg::set3DObject(D3SceneObject *obj) {
  m_scene.removeAllSceneObjects();
  if(m_3DObject != NULL) {
    delete m_3DObject;
    m_3DObject = NULL;
  }
  if(obj != NULL) {
    m_scene.addSceneObject(obj);
  }
  m_3DObject = obj;
}

ProfileRotationParameters CProfileDlg::getRotateParameters() {
  ProfileRotationParameters param;
  param.m_rad          = GRAD2RAD(m_degree);
  param.m_smoothness   = 0;
  if(IsDlgButtonChecked(IDC_CHECK_NORMALSMOOTH)) {
    param.m_smoothness |= NORMALSMOOTH;
  }
  if(IsDlgButtonChecked(IDC_CHECK_ROTATESMOOTH)) {
    param.m_smoothness |= ROTATESMOOTH;
  }

  param.m_alignx       = 0;
  param.m_aligny       = 1;
  param.m_rotateAxis   = m_rotateAxis;
  param.m_edgeCount    = m_edgeCount;
  return param;
}

ProfileStretchParameters CProfileDlg::getStretchParameters() {
  ProfileStretchParameters param;
  return param;
}

void CProfileDlg::setProfileName(const String &name) {
  m_profile.m_name = name;
  SetWindowText(name.cstr());
}

bool CProfileDlg::validate() {
  return true;
}

void CProfileDlg::OnFileSave() {
  if(!validate()) {
    return;
  }
  if(!m_profile.hasDefaultName()) {
    saveAs();
  } else {
    save(m_profile.m_name);
  }
}

void CProfileDlg::OnFileSaveAs() {
  if(!validate()) {
    return;
  }
  saveAs();
}

void CProfileDlg::saveAs() {
  CString objName = m_profile.m_name.cstr();
  CFileDialog dlg(FALSE,_T("*.prf"),objName);
  dlg.m_ofn.lpstrFilter = profileFileExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save Profile");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile);
}

void CProfileDlg::save(const String &fileName) {
  FILE *file = NULL;
  try {
    String extension = FileNameSplitter(fileName).getExtension();
    String fullName = fileName;
    if(extension.length() == 0) {
      fullName = FileNameSplitter(fileName).setExtension(_T("prf")).getFullPath();
    }
    file = FOPEN(fullName,_T("w"));
    m_profile.write(file);
    setProfileName(fullName);
    m_lastSavedProfile = m_profile;
  } catch(Exception e) {
    showException(e);
  }
  if(file != NULL) {
    fclose(file);
  }
}

bool CProfileDlg::dirtyCheck() {
  if(!isDirty()) {
    return true;
  }

  if(MessageBox(_T("Save changes"),_T("Profile"),MB_YESNO + MB_ICONQUESTION) != IDYES) {
    return true;
  }
  OnFileSave();
  if(isDirty()) {
    return false;
  }
  return true;
}

void CProfileDlg::OnFileNew() {
  if(!dirtyCheck()) {
    return;
  }
  m_profile.init();
  resetView();
}

void CProfileDlg::OnFileOpen() {
  if(!dirtyCheck()) {
    return;
  }
  Profile *profile = selectAndLoadProfile();
  if(profile != NULL) {
    m_profile = *profile;
    delete profile;
    resetView();
  }
}

void CProfileDlg::OnFileSelectFromFont() {
/*
  if(!dirtyCheck()) {
    return;
  }
  CFontDialog dlg(&m_logFont,CF_EFFECTS | CF_SCREENFONTS,NULL,this);
  if(dlg.DoModal() == IDOK) {
    dlg.GetCurrentFont(&m_logFont);
    CSelectGlyphDialog selectGlyphDlg(m_logFont,this);
    if(selectGlyphDlg.DoModal() == IDOK) {
      m_profile = Profile("Untitled",selectGlyphDlg.getSelectedGlyphCurveData());
      resetView();
    }
  }
*/
}

BOOL CProfileDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }

  switch(pMsg->message) {
  case WM_KEYDOWN:
    switch(m_currentControl) {
    case IDC_STATIC_PROFILEIMAGE2D:
      if(m_currentDrawTool->OnKeyDown((UINT)pMsg->wParam, 1, (UINT)pMsg->lParam)) {
        return true;
      }
      break;
    case IDC_STATIC_PROFILEIMAGE3D:
      break;
    }
  }
  return __super::PreTranslateMessage(pMsg);
}

void CProfileDlg::OnEditCut() {
  m_currentDrawTool->cutSelected();
}

void CProfileDlg::OnEditCopy() {
  m_currentDrawTool->copySelected();
}

void CProfileDlg::OnEditDelete() {
  m_currentDrawTool->deleteSelected();
}

void CProfileDlg::OnEditPaste() {
  try {
    m_currentDrawTool->paste();
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CProfileDlg::OnEditConnect() {
  m_currentDrawTool->connect();
}

void CProfileDlg::OnEditInvertnormals() {
  m_currentDrawTool->invertNormals();
}

void CProfileDlg::OnEditMirrorHorizontal() {
  m_currentDrawTool->mirror(true);
}

void CProfileDlg::OnEditMirrorVertical() {
  m_currentDrawTool->mirror(false);
}

void CProfileDlg::OnViewShowPoints() {
  toggleMenuItem(this,ID_VIEW_SHOWPOINTS);
  m_currentDrawTool->repaintProfile();
  Invalidate();
}

void CProfileDlg::OnViewShowNormals() {
  toggleMenuItem(this,ID_VIEW_SHOWNORMALS);
  m_currentDrawTool->repaintProfile();
  Invalidate();
}

void CProfileDlg::OnViewAutoUpdate3D() {
  toggleMenuItem(this,ID_VIEW_AUTO_UPDATE_3D);
  Invalidate();
}

int CProfileDlg::showNormals() {
  if(!isMenuItemChecked(this,ID_VIEW_SHOWNORMALS)) {
    return 0;
  }
  return IsDlgButtonChecked(IDC_CHECK_NORMALSMOOTH) ? 2 : 1;
}

bool CProfileDlg::showPoints() {
  return isMenuItemChecked(this,ID_VIEW_SHOWPOINTS);
}

void CProfileDlg::setMousePosition(const Point2D &p) {
  CPoint sp = getViewport().forwardTransform(p);
  GetDlgItem(IDC_STATIC_PROFILEIMAGE2D)->ClientToScreen(&sp);
  ::SetCursorPos(sp.x,sp.y);
}

static int toolItems[] = {
  ID_TOOLS_LINE
 ,ID_TOOLS_BEZIERCURVE
 ,ID_TOOLS_RECTANGLE
 ,ID_TOOLS_POLYGON
 ,ID_TOOLS_ELLIPSE
 ,ID_TOOLS_SELECT
};

void CProfileDlg::checkToolItem(int id) {
  for(int i = 0; i < ARRAYSIZE(toolItems); i++) {
    checkMenuItem(this,toolItems[i],false);
  }
  checkMenuItem(this,id,true);
}

void CProfileDlg::setCurrentDrawToolId(int id) {
  checkToolItem(id);
  switch(id) {
  case ID_TOOLS_LINE:
    setCurrentDrawTool(new LineTool(this));
    break;
  case ID_TOOLS_BEZIERCURVE:
    setCurrentDrawTool(new BezierCurveTool(this));
    break;
  case ID_TOOLS_RECTANGLE:
    setCurrentDrawTool(new RectangleTool(this));
    break;
  case ID_TOOLS_POLYGON:
    setCurrentDrawTool(new PolygonTool(this));
    break;
  case ID_TOOLS_ELLIPSE:
    setCurrentDrawTool(new EllipseTool(this));
    break;
  case ID_TOOLS_SELECT:
    setCurrentDrawTool(new SelectTool(this));
    break;
  default:
    setCurrentDrawTool(new NullTool());
    break;
  }
  m_currentDrawTool->repaintProfile();
  repaintScreen();
}

void CProfileDlg::setCurrentDrawTool(DrawTool *newDrawTool) {
  if(m_currentDrawTool != NULL) {
    delete m_currentDrawTool;
  }
  m_currentDrawTool = newDrawTool;
}

void CProfileDlg::OnToolsLine() {
  setCurrentDrawToolId(ID_TOOLS_LINE);
}

void CProfileDlg::OnToolsBezierCurve() {
  setCurrentDrawToolId(ID_TOOLS_BEZIERCURVE);
}

void CProfileDlg::OnToolsRectangle() {
  setCurrentDrawToolId(ID_TOOLS_RECTANGLE);
}

void CProfileDlg::OnToolsPolygon() {
  setCurrentDrawToolId(ID_TOOLS_POLYGON);
}

void CProfileDlg::OnToolsEllipse() {
  setCurrentDrawToolId(ID_TOOLS_ELLIPSE);
}

void CProfileDlg::OnToolsSelect() {
  setCurrentDrawToolId(ID_TOOLS_SELECT);
}

CPoint CProfileDlg::getRelativePoint(int id, const CPoint &p) {
  CPoint result = p;
  ClientToScreen(&result);
  GetDlgItem(id)->ScreenToClient(&result);
  return result;
}

void CProfileDlg::clipCursor() {
  CRect cr;
  CWnd *wnd = GetDlgItem(IDC_STATIC_PROFILEIMAGE2D);
  wnd->GetClientRect(&cr);
  wnd->ClientToScreen(cr);
  ClipCursor(&cr);
}

void CProfileDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  switch(getControlAtPoint(point)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    m_currentDrawTool->OnLButtonDown(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,point));
    clipCursor();
    break;
  case IDC_STATIC_PROFILEIMAGE3D:
//    m_d3.OnLButtonDown(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE3D,point));
    break;
  }

  __super::OnLButtonDown(nFlags, point);
}

void CProfileDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
  switch(getControlAtPoint(point)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    m_currentDrawTool->OnLButtonDblClk(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,point));
    clipCursor();
    break;
  case IDC_STATIC_PROFILEIMAGE3D:
//    m_d3.OnLButtonDblClk(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE3D,point));
    break;
  }
  __super::OnLButtonDblClk(nFlags, point);
}

void CProfileDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  switch(getControlAtPoint(point)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    m_currentDrawTool->OnLButtonUp(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,point));
    ClipCursor(NULL);
    break;
  case IDC_STATIC_PROFILEIMAGE3D:
//    m_d3.OnLButtonUp(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE3D,point));
    break;
  }

  __super::OnLButtonUp(nFlags, point);
}

void CProfileDlg::OnRButtonDown(UINT nFlags, CPoint point) {
  switch(getControlAtPoint(point)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    m_currentDrawTool->OnRButtonDown(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,point));
    clipCursor();
    break;
  case IDC_STATIC_PROFILEIMAGE3D:
//    m_d3.OnRButtonDown(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE3D,point));
    break;
  }
  __super::OnRButtonDown(nFlags, point);
}

void CProfileDlg::OnRButtonUp(UINT nFlags, CPoint point) {
  switch(getControlAtPoint(point)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    m_currentDrawTool->OnRButtonUp(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,point));
    ClipCursor(NULL);
    break;
  case IDC_STATIC_PROFILEIMAGE3D:
//    m_d3.OnRButtonUp(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE3D,point));
    break;
  }
  __super::OnRButtonUp(nFlags, point);
}

void CProfileDlg::OnMouseMove(UINT nFlags, CPoint point) {
  switch(getControlAtPoint(point)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    { CPoint p = getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,point);
      showMousePosition(p);
      m_currentDrawTool->OnMouseMove(nFlags,p);
    }
    break;
  case IDC_STATIC_PROFILEIMAGE3D:
//    m_d3.OnMouseMove(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE3D,point));
    break;
  }
  __super::OnMouseMove(nFlags, point);
}

BOOL CProfileDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  switch(getControlAtPoint(pt)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    m_currentDrawTool->OnMouseWheel(nFlags,zDelta,getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,pt));
    break;
  case IDC_STATIC_PROFILEIMAGE3D:
//    m_d3.OnMouseWheel(nFlags,zDelta,getRelativePoint(IDC_STATIC_PROFILEIMAGE3D,pt));
    break;
  }
  return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CProfileDlg::showMousePosition(const CPoint &p) {
  Point2D fp = m_viewport->backwardTransform(p);
  GetDlgItem(IDC_STATIC_INFO)->SetWindowText(format(_T("%.2lf,%.2lf"),fp.x,fp.y).cstr());
}

void CProfileDlg::OnCancel() {
  if(!dirtyCheck()) return;
  __super::OnCancel();
}

void CProfileDlg::OnOK() {
  if(!dirtyCheck()) return;
  __super::OnOK();
}

void CProfileDlg::OnRadioRotatexaxis()  { updateAndRefresh(); }
void CProfileDlg::OnRadioRotateyaxis()  { updateAndRefresh(); }
void CProfileDlg::OnCheckRotateSmooth() { updateAndRefresh(); }
void CProfileDlg::OnCheckNormalSmooth() { updateAndRefresh(); }
void CProfileDlg::OnButtonRefresh()     { updateAndRefresh(); }

void CProfileDlg::updateAndRefresh() {
  UpdateData();
  refresh2d();
  refresh3d();
  Invalidate(false);
}

void CProfileDlg::OnRadioRotate() {
  m_currentControl = IDC_RADIO_ROTATE;
}

void CProfileDlg::OnRadioStretch() {
  m_currentControl = IDC_RADIO_STRETCH;
}

void CProfileDlg::OnSetfocusEditDegrees() {
  m_currentControl = IDC_EDIT_DEGREES;
}

void CProfileDlg::OnKillfocusEditDegrees() {
  m_currentControl = -1;
}

void CProfileDlg::OnSetfocusEditEdgeCount() {
  m_currentControl = IDC_EDIT_EDGECOUNT;
}

void CProfileDlg::OnKillfocusEditEdgeCount() {
  m_currentControl = -1;
}

int CProfileDlg::getControlAtPoint(const CPoint &point) {
  CWnd *w = ChildWindowFromPoint(point);
  m_currentControl = (w == NULL) ? -1 : w->GetDlgCtrlID();
  return m_currentControl;
}


void CProfileDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
  __super::OnHScroll(nSBCode, nPos, pScrollBar);


  CSliderCtrl *slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEGREE);
  int degree = slider->GetPos();

  static const CSize testSize = getBitmapSize(m_testBitmap);
  static const CSize maxSize(2*testSize.cx, 2*testSize.cy);


  HBITMAP rotBM = rotateBitmap(getBitmapCacheDevice(), m_testBitmap, degree);
  CBitmap *bm = CBitmap::FromHandle(rotBM);
  CDC *srcDC = CDC::FromHandle(CreateCompatibleDC(NULL));
  srcDC->SelectObject(bm);
  CSize bmSize = getBitmapSize(*bm);

  CClientDC dstDC(GetDlgItem(IDC_STATIC_PROFILEIMAGE2D));

  dstDC.FillSolidRect(0,0,maxSize.cx, maxSize.cy, RGB(255,255,255));
  dstDC.BitBlt(0,0,bmSize.cx, bmSize.cy, srcDC, 0,0, SRCCOPY);

  DeleteObject(rotBM);
  showSliderPos();
}

void CProfileDlg::showSliderPos() {
  CSliderCtrl *slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEGREE);
  int sliderPos = slider->GetPos();
  setWindowText(this, IDC_STATIC_DEGREE, format(_T("%d"), sliderPos).cstr());
}

