#include "stdafx.h"
#include <FileNameSplitter.h>
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3SceneObjectVisualNormals.h>
#include <D3DGraphics/resource.h>
#include "ProfilePainter.h"
#include "D3ProfileObjectWithColor.h"
#include "ProfileDlg.h"
//#include "SelectGlyphDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CProfileDlg::CProfileDlg(CWnd *pParent) : CDialog(IDD, pParent) {
  m_doubleSided      = TRUE;
  m_3dmode           = 0;
  m_visual           = NULL;
  m_viewport         = NULL;
  m_currentDrawTool  = NULL;
  m_lastSavedProfile = m_profile;
  ProfileRotationParameters param;
  rotateParamToWin(param);
}

CProfileDlg::~CProfileDlg() {
  m_editor.setEnabled(false);
  setCurrentDrawTool(NULL);
  m_editor.close();
  destroy3DObject();
  m_scene.close();
  destroyViewport();
  destroyWorkBitmap();
  destroyBitmapCache();
}

void CProfileDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_DEGREES, m_degree);
  DDX_Check(pDX, IDC_CHECK_NORMALSMOOTH, m_normalSmooth);
  DDX_Check(pDX, IDC_CHECK_ROTATESMOOTH, m_rotateSmooth);
  DDX_Text(pDX, IDC_EDIT_EDGECOUNT, m_edgeCount);
  DDV_MinMaxInt(pDX, m_edgeCount, 1, 100);
  DDX_Radio(pDX, IDC_RADIO_ROTATE, m_3dmode);
  DDX_Check(pDX, IDC_CHECKDOUBLESIDED, m_doubleSided);
  DDX_CBString(pDX, IDC_COMBOROTATEAXIS, m_rotateAxis);
  DDX_CBString(pDX, IDC_COMBOROTATEAXISALIGNSTO, m_rotateAxisAlignsTo);
  DDX_Check(pDX, IDC_CHECK_USECOLOR, m_useColor);
  DDX_Text(pDX, IDC_MFCCOLORBUTTON, m_color);
}

BEGIN_MESSAGE_MAP(CProfileDlg, CDialog)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_MOUSEWHEEL()
  ON_WM_HSCROLL()
  ON_COMMAND(     ID_FILE_NEW                , OnFileNew                  )
  ON_COMMAND(     ID_FILE_OPEN               , OnFileOpen                 )
  ON_COMMAND(     ID_FILE_SAVE               , OnFileSave                 )
  ON_COMMAND(     ID_FILE_SAVE_AS            , OnFileSaveAs               )
  ON_COMMAND(     ID_FILE_SELECTFROMFONT     , OnFileSelectFromFont       )
  ON_COMMAND(     ID_EDIT_CUT                , OnEditCut                  )
  ON_COMMAND(     ID_EDIT_COPY               , OnEditCopy                 )
  ON_COMMAND(     ID_EDIT_PASTE              , OnEditPaste                )
  ON_COMMAND(     ID_EDIT_DELETE             , OnEditDelete               )
  ON_COMMAND(     ID_EDIT_CONNECT            , OnEditConnect              )
  ON_COMMAND(     ID_EDIT_INVERTNORMALS      , OnEditInvertnormals        )
  ON_COMMAND(     ID_EDIT_MIRROR_HORIZONTAL  , OnEditMirrorHorizontal     )
  ON_COMMAND(     ID_EDIT_MIRROR_VERTICAL    , OnEditMirrorVertical       )
  ON_COMMAND(     ID_VIEW_SHOWPOINTS         , OnViewShowPoints           )
  ON_COMMAND(     ID_VIEW_SHOWNORMALS        , OnViewShowNormals          )
  ON_COMMAND(     ID_VIEW_AUTO_UPDATE_3D     , OnViewAutoUpdate3D         )
  ON_COMMAND(     ID_TOOLS_LINE              , OnToolsLine                )
  ON_COMMAND(     ID_TOOLS_BEZIERCURVE       , OnToolsBezierCurve         )
  ON_COMMAND(     ID_TOOLS_RECTANGLE         , OnToolsRectangle           )
  ON_COMMAND(     ID_TOOLS_POLYGON           , OnToolsPolygon             )
  ON_COMMAND(     ID_TOOLS_ELLIPSE           , OnToolsEllipse             )
  ON_COMMAND(     ID_TOOLS_SELECT            , OnToolsSelect              )
  ON_BN_CLICKED(  IDC_RADIO_ROTATE           , OnRadioRotate              )
  ON_BN_CLICKED(  IDC_RADIO_STRETCH          , OnRadioStretch             )
  ON_BN_CLICKED(  IDC_CHECKDOUBLESIDED       , OnCheckDoubleSided         )
  ON_BN_CLICKED(  IDC_BUTTON_REFRESH         , OnButtonRefresh            )
  ON_BN_CLICKED(  IDC_CHECK_ROTATESMOOTH     , OnCheckRotateSmooth        )
  ON_BN_CLICKED(  IDC_CHECK_NORMALSMOOTH     , OnCheckNormalSmooth        )
  ON_EN_SETFOCUS( IDC_EDIT_DEGREES           , OnSetfocusEditDegrees      )
  ON_EN_KILLFOCUS(IDC_EDIT_DEGREES           , OnKillfocusEditDegrees     )
  ON_EN_KILLFOCUS(IDC_EDIT_EDGECOUNT         , OnKillfocusEditEdgeCount   )
  ON_EN_SETFOCUS( IDC_EDIT_EDGECOUNT         , OnSetfocusEditEdgeCount    )
//    ON_CBN_SELENDOK(IDC_COMBOROTATEAXIS        , OnCbnSelendokComboRotateAxis)
//    ON_CBN_SELENDOK(IDC_COMBOROTATEAXISALIGNSTO, OnCbnSelendokComboRotateAxisAlignsTo)
  ON_BN_CLICKED(  IDC_CHECK_USECOLOR         , OnBnClickedCheckUseColor   )
  ON_MESSAGE(     ID_MSG_RENDER              , OnMsgRender                )
END_MESSAGE_MAP()

static LOGFONT makeDefaultFont() {
  CFont f;
  f.CreateFont( 10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
               ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
               ,DEFAULT_PITCH | FF_MODERN
               ,_T("Times new roman")
              );
  LOGFONT lf;
  f.GetLogFont(&lf);
  f.DeleteObject();
  return lf;
}

#define sCAM() m_editor.getSelectedCAM()

BOOL CProfileDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_PROFILE));
  m_logFont    = makeDefaultFont();
  createWorkBitmap();
  m_scene.initDevice(get3DWindow(0));
  m_editor.init(this);
  m_editor.setEnabled(true);
  D3Camera *cam = sCAM();
  if(cam) {
    m_scene.setLightDirection(0, rotate(cam->getDir(), cam->getRight(), 0.2f));
  }

  createBitmapCache(*GetDlgItem(IDC_STATIC_DEBUG3DFRAME));
  setCurrentDrawToolId(ID_TOOLS_LINE);
  CSliderCtrl *slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DEGREE);
  slider->SetRange(0,360);
  m_testBitmap.LoadBitmap(IDB_TESTBITMAP);
  showSliderPos();
  return FALSE;
}

void CProfileDlg::createWorkBitmap() {
  destroyWorkBitmap();
  CClientDC   screen(this);
  m_workRect = getClientRect(this, IDC_STATIC_PROFILEIMAGE2D);
  const CSize sz = m_workRect.Size();
  m_workDC.CreateCompatibleDC(&screen);
  m_workBitmap.CreateBitmap(sz.cx,sz.cy,screen.GetDeviceCaps(PLANES),screen.GetDeviceCaps(BITSPIXEL),NULL);
  m_workBitmap.SetBitmapDimension(sz.cx,sz.cy);
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

D3Device &CProfileDlg::getDevice() {
  return m_scene.getDevice();
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

  m_viewport = new Viewport2D(m_workDC,profileRect,m_workRect,true); TRACE_NEW(m_viewport);
  m_viewport->clear(RGB(255,255,255));
}

void CProfileDlg::destroyViewport() {
  SAFEDELETE(m_viewport);
}

void CProfileDlg::resetView() {
  m_lastSavedProfile = m_profile;
  initViewport();
  setProfileName(m_profile.m_name);
  m_currentDrawTool->unselectAll();
  m_currentDrawTool->initState();
  render(SC_RENDERALL);
}

void CProfileDlg::updateAndRender3D() {
  UpdateData();
  render(SC_RENDER3D);
}

void CProfileDlg::render(BYTE renderFlags) {
  __super::render(renderFlags, m_scene.getCameraArray().getActiveCameraSet());
}

void CProfileDlg::doRender(BYTE renderFlags, CameraSet cameraSet) {
  if(renderFlags & SC_RENDERNOW) {
    OnMsgRender(renderFlags, cameraSet);
  } else {
    PostMessage(ID_MSG_RENDER, renderFlags, cameraSet);
  }
}

static UINT render2DCount = 0, render3DCount = 0;
LRESULT CProfileDlg::OnMsgRender(WPARAM wp, LPARAM lp) {
  if(wp & SC_RENDER2D) {
    render2DCount++;
    CClientDC dc(GetDlgItem(IDC_STATIC_PROFILEIMAGE2D));
    dc.BitBlt(0,0,m_workRect.Width(),m_workRect.Height(),&m_workDC,0,0,SRCCOPY);
    if(needUpdate3DObject()) {
      create3DObject();
      saveCurrentProfVars();
      wp |= SC_RENDER3D;
    }
  }
  if(wp & SC_RENDER3D) {
    if(!(wp & SC_RENDER2D) && needUpdate3DObject()) {
      create3DObject();
      saveCurrentProfVars();
    }
    CameraSet cameraSet(lp);
    __super::doRender((BYTE)wp, cameraSet);
    render3DCount++;
  }
  enableWindowItems();
  return 0;
}

void CProfileDlg::enableWindowItems() {
  enableMenuItem(this,ID_EDIT_CONNECT          ,m_currentDrawTool->canConnect());
  enableMenuItem(this,ID_EDIT_INVERTNORMALS    ,m_currentDrawTool->canInvertNormals());
  enableMenuItem(this,ID_EDIT_MIRROR_HORIZONTAL,m_currentDrawTool->canMirror());
  enableMenuItem(this,ID_EDIT_MIRROR_VERTICAL  ,m_currentDrawTool->canMirror());
  enableMenuItem(this,ID_EDIT_DELETE           ,m_currentDrawTool->canDelete());
  enableMenuItem(this,ID_EDIT_COPY             ,m_currentDrawTool->canCopy());
  enableMenuItem(this,ID_EDIT_CUT              ,m_currentDrawTool->canCut());
  GetDlgItem(IDC_MFCCOLORBUTTON)->EnableWindow(m_useColor);
}

void CProfileDlg::stretchProfile() {
/*
  try {
    m_imageFrame->AddVisual(m_d3.stretchProfile(m_profile,getStretchParameters()));
    m_d3.addInnerSide(m_imageFrame);
  } catch(Exception e) {
    showException(e);
  }
*/
}

ProfileDialogVariables &CProfileDlg::getAllProfVars(ProfileDialogVariables &profVars) {
  profVars.m_3dmode      = m_3dmode;
  profVars.m_doubleSided = m_doubleSided;
  rotateWinToParam(profVars.m_rotationParameters);
  profVars.m_profile     = m_profile;
  return profVars;
}

bool operator==(const ProfileDialogVariables &v1, const ProfileDialogVariables &v2) {
  return v1.m_3dmode             == v2.m_3dmode
      && v1.m_doubleSided        == v2.m_doubleSided
      && v1.m_rotationParameters == v2.m_rotationParameters
      && v1.m_profile            == v2.m_profile;
}

void CProfileDlg::saveCurrentProfVars() {
  m_lastProfVars = m_currentProfVars;
}

bool CProfileDlg::needUpdate3DObject() {
  return getAllProfVars(m_currentProfVars) != m_lastProfVars;
}

void CProfileDlg::create3DObject() {
  try {
    D3SceneObjectVisual *visual = NULL;
    if(!m_profile.isEmpty()) {
      visual = new D3ProfileObjectWithColor(this); TRACE_NEW(visual);
    }
    setVisual(visual);
  } catch(Exception e) {
    showException(e);
  }
}

void CProfileDlg::setVisual(D3SceneObjectVisual *visual) {
  const bool  hadOldVisual = m_visual != NULL;
  D3DXMATRIX  oldWorld;
  if(hadOldVisual) {
    oldWorld = m_visual->getWorld();
  }
  destroy3DObject();
  if(visual != NULL) {
    if(hadOldVisual) {
      visual->getWorld() = oldWorld;
    }
    m_scene.addVisual(visual);
  }
  m_visual = visual;
}

void CProfileDlg::destroy3DObject() {
  m_scene.removeAllVisuals();
  SAFEDELETE(m_visual);
}

void CProfileDlg::rotateParamToWin(const ProfileRotationParameters &param) {
  m_normalSmooth       = param.isSet(PRROT_NORMALSMOOTH);
  m_rotateSmooth       = param.isSet(PRROT_ROTATESMOOTH);
  m_useColor           = param.isSet(PRROT_USECOLOR    );
  m_degree             = RAD2GRAD(param.m_rad);
  m_edgeCount          = param.m_edgeCount;
  m_rotateAxis         = format(_T("%c"), param.m_rotateAxis).cstr();
  m_rotateAxisAlignsTo = format(_T("%c"), param.m_rotateAxisAlignsTo).cstr();
  m_color              = D3DCOLOR2COLORREF(param.m_color);
}

void CProfileDlg::rotateWinToParam(ProfileRotationParameters &param) {
  UpdateData();
  param.m_flags              = 0;
  if(m_normalSmooth) {
    param.setFlag(PRROT_NORMALSMOOTH);
  }
  if(m_rotateSmooth) {
    param.setFlag(PRROT_ROTATESMOOTH);
  }
  if(m_useColor) {
    param.setFlag(PRROT_USECOLOR);
  }
  param.m_rad                = (float)GRAD2RAD(m_degree);
  param.m_edgeCount          = m_edgeCount;
  param.m_rotateAxis         = (char)m_rotateAxis.GetAt(0);
  param.m_rotateAxisAlignsTo = (char)m_rotateAxisAlignsTo.GetAt(0);
  param.m_color              = COLORREF2D3DCOLOR(m_color);
}

ProfileRotationParameters CProfileDlg::getRotateParameters() const {
  ProfileRotationParameters param;
  ((CProfileDlg*)this)->rotateWinToParam(param);
  return param;
}

ProfileStretchParameters CProfileDlg::getStretchParameters() const {
  ProfileStretchParameters param;
  return param;
}

bool CProfileDlg::isDoubleSided() const {
  return m_doubleSided;
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
#ifndef __TODO__
  showWarning(_T("%s:Function not implemented"), __TFUNCTION__);
#else
  CString objName = m_profile.m_name.cstr();
  CFileDialog dlg(FALSE,_T("*.prf"),objName);
  dlg.m_ofn.lpstrFilter = profileFileExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save Profile");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile);
#endif
}

void CProfileDlg::save(const String &fileName) {
#ifndef __TODO__
  showWarning(_T("%s:Function not implemented"), __TFUNCTION__);
#else
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
#endif
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
    SAFEDELETE(profile);
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

BOOL CProfileDlg::PreTranslateMessage(MSG *pMsg) {
  const bool levelIncremented = pMsg->message != ID_MSG_RENDER;
  if(levelIncremented) incrLevel();
  BOOL result;
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    result = true;
  } else {
    switch(pMsg->message) {
    case WM_KEYDOWN:
      switch(m_currentControl) {
      case IDC_STATIC_PROFILEIMAGE2D:
        if(m_currentDrawTool->OnKeyDown((UINT)pMsg->wParam, 1, (UINT)pMsg->lParam)) {
          result = true;
          goto End;
        }
        break;
      case IDC_STATIC_PROFILEIMAGE3D:
        if(m_editor.PreTranslateMessage(pMsg)) {
          result = true;
          goto End;
        }
        break;
      }
    }
    if(m_editor.PreTranslateMessage(pMsg)) {
      result = true;
    } else {
      result = __super::PreTranslateMessage(pMsg);
    }
  }
End:
  if(levelIncremented) {
    decrLevel();
  }
  return result;
}

void CProfileDlg::OnEditCut() {
  m_currentDrawTool->cutSelected().repaintScreen();
}

void CProfileDlg::OnEditCopy() {
  m_currentDrawTool->copySelected().repaintScreen();
}

void CProfileDlg::OnEditDelete() {
  m_currentDrawTool->deleteSelected().repaintScreen();
}

void CProfileDlg::OnEditPaste() {
  try {
    m_currentDrawTool->paste().repaintScreen();
  } catch(Exception e) {
    showException(e);
  }
}

void CProfileDlg::OnEditConnect() {
  m_currentDrawTool->connect().repaintScreen();
}

void CProfileDlg::OnEditInvertnormals() {
  m_currentDrawTool->invertNormals().repaintScreen();
}

void CProfileDlg::OnEditMirrorHorizontal() {
  m_currentDrawTool->mirror(true).repaintScreen();
}

void CProfileDlg::OnEditMirrorVertical() {
  m_currentDrawTool->mirror(false).repaintScreen();
}

void CProfileDlg::OnViewShowPoints() {
  toggleMenuItem(this,ID_VIEW_SHOWPOINTS);
  m_currentDrawTool->repaintProfile().repaintScreen();
}

void CProfileDlg::OnViewShowNormals() {
  toggleMenuItem(this,ID_VIEW_SHOWNORMALS);
  m_currentDrawTool->repaintProfile().repaintScreen();
}

void CProfileDlg::OnViewAutoUpdate3D() {
  toggleMenuItem(this,ID_VIEW_AUTO_UPDATE_3D);
  render(SC_RENDER3D);
}

void CProfileDlg::repaintViewport() {
  render(SC_RENDER2D | SC_RENDER3D);
}

NormalsMode CProfileDlg::getNormalsMode() {
  if(!isMenuItemChecked(this,ID_VIEW_SHOWNORMALS)) {
    return NORMALS_INVISIBLE;
  }
  return IsDlgButtonChecked(IDC_CHECK_NORMALSMOOTH) ? NORMALS_SMOOTH : NORMALS_FLAT;
}

bool CProfileDlg::getShowPoints() {
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
  for(size_t i = 0; i < ARRAYSIZE(toolItems); i++) {
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
  m_currentDrawTool->repaintProfile().repaintScreen();
}

void CProfileDlg::setCurrentDrawTool(DrawTool *newDrawTool) {
  SAFEDELETE(m_currentDrawTool);
  TRACE_NEW(newDrawTool);
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
  }
  __super::OnLButtonDown(nFlags, point);
}

void CProfileDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
  switch(getControlAtPoint(point)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    m_currentDrawTool->OnLButtonDblClk(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,point));
    clipCursor();
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
  }
  __super::OnLButtonUp(nFlags, point);
}

void CProfileDlg::OnRButtonDown(UINT nFlags, CPoint point) {
  switch(getControlAtPoint(point)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    m_currentDrawTool->OnRButtonDown(nFlags,getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,point));
    clipCursor();
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
  }
  __super::OnMouseMove(nFlags, point);
}

BOOL CProfileDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  switch(getControlAtPoint(pt)) {
  case IDC_STATIC_PROFILEIMAGE2D:
    m_currentDrawTool->OnMouseWheel(nFlags,zDelta,getRelativePoint(IDC_STATIC_PROFILEIMAGE2D,pt));
    break;
  }
  return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CProfileDlg::showMousePosition(const CPoint &p) {
  Point2D fp = m_viewport->backwardTransform(p);
  setWindowText(this, IDC_STATIC_INFO,format(_T("%.2lf,%.2lf"),fp.x,fp.y));
}

void CProfileDlg::OnCancel() {
  if(!dirtyCheck()) return;
  __super::OnCancel();
}

void CProfileDlg::OnOK() {
  if(!dirtyCheck()) return;
  __super::OnOK();
}

void CProfileDlg::OnCheckDoubleSided()                   { updateAndRender3D(); }
void CProfileDlg::OnCheckRotateSmooth()                  { updateAndRender3D(); }

void CProfileDlg::OnCheckNormalSmooth()                  {
  UpdateData();
  m_currentDrawTool->repaintProfile().repaintScreen();
}

void CProfileDlg::OnCbnSelendokComboRotateAxisAlignsTo() { updateAndRender3D(); }
void CProfileDlg::OnCbnSelendokComboRotateAxis()         { updateAndRender3D(); }
void CProfileDlg::OnButtonRefresh()                      { updateAndRender3D(); }

void CProfileDlg::OnBnClickedCheckUseColor() {
  updateAndRender3D();
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
