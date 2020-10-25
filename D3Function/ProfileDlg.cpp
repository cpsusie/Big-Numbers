#include "stdafx.h"
#include <FileNameSplitter.h>
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3SceneObjectVisualNormals.h>
#include <D3DGraphics/resource.h>
#include "ProfilePainter.h"
#include "D3ProfileObjectWithColor.h"
#include "BitmapRotate.h"
#include "DrawTools.h"
#include "ProfileDlg.h"
#include "SelectGlyphDlg.h"

class ValidationException : public Exception {
public:
  int m_field;
  ValidationException(int field, const String &msg) : Exception(msg), m_field(field) {
  }
};

void throwValidateException(int field, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  throw ValidationException(field, msg);
}

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CProfileDlg::CProfileDlg(CWnd *pParent) : CDialog(IDD, pParent) {
  m_doubleSided      = TRUE;
  m_3dmode           = 0;
  m_visual           = NULL;
  m_viewport         = NULL;
  m_currentDrawTool  = NULL;
  m_exceptionRaised  = false;
  m_lastSavedProfile = m_profile;
}

CProfileDlg::~CProfileDlg() {
  m_editor.setEnabled(false);
  setCurrentDrawTool(NULL);
  m_editor.close();
  destroy3DObject();
  m_scene.close();
  destroyViewport();
  destroyWorkBitmap();
}

void CProfileDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(    pDX, IDC_EDIT_DEGREES            , m_degrees           );
  DDX_Check(   pDX, IDC_CHECK_NORMALSMOOTH      , m_normalSmooth      );
  DDX_Check(   pDX, IDC_CHECK_ROTATESMOOTH      , m_rotateSmooth      );
  DDX_Text(    pDX, IDC_EDIT_EDGECOUNT          , m_edgeCount         );
  DDX_Radio(   pDX, IDC_RADIO_ROTATE            , m_3dmode            );
  DDX_Check(   pDX, IDC_CHECK_DOUBLESIDED       , m_doubleSided       );
  DDX_CBString(pDX, IDC_COMBO_ROTATEAXIS        , m_rotateAxis        );
  DDX_CBString(pDX, IDC_COMBO_ROTATEAXISALIGNSTO, m_rotateAxisAlignsTo);
  DDX_Check(   pDX, IDC_CHECK_USECOLOR          , m_useColor          );
}

BEGIN_MESSAGE_MAP(CProfileDlg, CDialog)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_MOUSEWHEEL()
  ON_WM_SIZE()
  ON_COMMAND(      ID_FILE_NEW                 , OnFileNew                            )
  ON_COMMAND(      ID_FILE_OPEN                , OnFileOpen                           )
  ON_COMMAND(      ID_FILE_SAVE                , OnFileSave                           )
  ON_COMMAND(      ID_FILE_SAVE_AS             , OnFileSaveAs                         )
  ON_COMMAND(      ID_FILE_SELECTFROMFONT      , OnFileSelectFromFont                 )
  ON_COMMAND(      ID_EDIT_CUT                 , OnEditCut                            )
  ON_COMMAND(      ID_EDIT_COPY                , OnEditCopy                           )
  ON_COMMAND(      ID_EDIT_PASTE               , OnEditPaste                          )
  ON_COMMAND(      ID_EDIT_DELETE              , OnEditDelete                         )
  ON_COMMAND(      ID_EDIT_CONNECT             , OnEditConnect                        )
  ON_COMMAND(      ID_EDIT_INVERTNORMALS       , OnEditInvertnormals                  )
  ON_COMMAND(      ID_EDIT_MIRROR_HORIZONTAL   , OnEditMirrorHorizontal               )
  ON_COMMAND(      ID_EDIT_MIRROR_VERTICAL     , OnEditMirrorVertical                 )
  ON_COMMAND(      ID_VIEW_SHOWPOINTS          , OnViewShowPoints                     )
  ON_COMMAND(      ID_VIEW_SHOWNORMALS         , OnViewShowNormals                    )
  ON_COMMAND(      ID_VIEW_AUTO_UPDATE_3D      , OnViewAutoUpdate3D                   )
  ON_COMMAND(      ID_TOOLS_LINE               , OnToolsLine                          )
  ON_COMMAND(      ID_TOOLS_BEZIERCURVE        , OnToolsBezierCurve                   )
  ON_COMMAND(      ID_TOOLS_RECTANGLE          , OnToolsRectangle                     )
  ON_COMMAND(      ID_TOOLS_POLYGON            , OnToolsPolygon                       )
  ON_COMMAND(      ID_TOOLS_ELLIPSE            , OnToolsEllipse                       )
  ON_COMMAND(      ID_TOOLS_SELECT             , OnToolsSelect                        )
  ON_BN_CLICKED(   IDC_RADIO_ROTATE            , OnRadioRotate                        )
  ON_BN_CLICKED(   IDC_RADIO_STRETCH           , OnRadioStretch                       )
  ON_BN_CLICKED(   IDC_CHECK_DOUBLESIDED       , OnCheckDoubleSided                   )
  ON_BN_CLICKED(   IDC_BUTTON_REFRESH          , OnButtonRefresh                      )
  ON_BN_CLICKED(   IDC_CHECK_ROTATESMOOTH      , OnCheckRotateSmooth                  )
  ON_BN_CLICKED(   IDC_CHECK_NORMALSMOOTH      , OnCheckNormalSmooth                  )
  ON_EN_SETFOCUS(  IDC_EDIT_DEGREES            , OnSetfocusEditDegrees                )
  ON_EN_KILLFOCUS( IDC_EDIT_DEGREES            , OnKillfocusEditDegrees               )
  ON_EN_KILLFOCUS( IDC_EDIT_EDGECOUNT          , OnKillfocusEditEdgeCount             )
  ON_EN_SETFOCUS(  IDC_EDIT_EDGECOUNT          , OnSetfocusEditEdgeCount              )
  ON_BN_CLICKED(   IDC_CHECK_USECOLOR          , OnBnClickedCheckUseColor             )
  ON_EN_CHANGE(    IDC_EDIT_EDGECOUNT          , OnEnChangeEditEdgeCount              )
  ON_EN_CHANGE(    IDC_EDIT_DEGREES            , OnEnChangeEditDegrees                )
  ON_BN_CLICKED(   IDC_MFCCOLORBUTTON          , OnBnClickedMFCColorButton            )
  ON_CBN_SELCHANGE(IDC_COMBO_ROTATEAXIS        , OnCbnSelchangeComboRotateAxis        )
  ON_CBN_SELCHANGE(IDC_COMBO_ROTATEAXISALIGNSTO, OnCbnSelchangeComboRotateAxisAlignsTo)
  ON_MESSAGE(      ID_MSG_RENDER               , OnMsgRender                          )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EDGECOUNT   , OnDeltaposSpinEdgeCount              )
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

  setCurrentDrawToolId(ID_TOOLS_LINE);
  ProfileRotationParameters param;
  rotateParamToWin(param);
  UpdateData(false);
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
  if(profileRect.getArea() == 0) {
    profileRect = Rectangle2D::getUnit();
  } else {
    profileRect.p0()[0]   -= profileRect.getWidth()  / 20;
    profileRect.p0()[1]   -= profileRect.getHeight() / 20;
    profileRect.size()[0] += profileRect.getWidth()  / 10;
    profileRect.size()[1] += profileRect.getHeight() / 10;
  }

  profileRect.p0()[1]   += profileRect.size()[1];
  profileRect.size()[1] *= -1;

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
  setProfileName(m_profile.getName());
  m_currentDrawTool->unselectAll();
  m_currentDrawTool->initState();
  renderAll();
}

void CProfileDlg::repaintAll() {
  repaintProfile();
  renderAll();
}

void CProfileDlg::repaintProfile() {
  m_currentDrawTool->repaintProfile();
}

void CProfileDlg::render2D() {
  render(SC_RENDER2D);
}

void CProfileDlg::render3D() {
  render(SC_RENDER3D);
}

void CProfileDlg::renderAll() {
  render(SC_RENDER2D | SC_RENDER3D);
}

bool CProfileDlg::isAutoUpdate3DChecked() const {
  return isMenuItemChecked(this, ID_VIEW_AUTO_UPDATE_3D);
}

void CProfileDlg::render(BYTE renderFlags) {
  __super::render(renderFlags, m_scene.getActiveCameraSet());
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
  try {
    if(wp & SC_RENDER2D) {
      checkWorkRectSize();
      CClientDC dc(GetDlgItem(IDC_STATIC_PROFILEIMAGE2D));
      dc.BitBlt(0, 0, m_workRect.Width(), m_workRect.Height(), &m_workDC, 0, 0, SRCCOPY);
      render2DCount++;
      if(isAutoUpdate3DChecked()) {
        wp |= SC_RENDER3D;
      }
    }
    if(wp & SC_RENDER3D) {
      if(needUpdate3DObject()) {
        create3DObject();
      }
      CameraSet cameraSet(lp);
      __super::doRender((BYTE)wp, cameraSet);
      render3DCount++;
    }
    enableWindowItems();
    if(m_exceptionRaised) {
      clearException();
    }
  } catch (Exception e) {
    raiseException(e);
    disableAll();
  }
  return 0;
}

void CProfileDlg::checkWorkRectSize() {
  if(getClientRect(this, IDC_STATIC_PROFILEIMAGE2D).Size() != m_workRect.Size()) {
    createWorkBitmap();
    repaintProfile();
  }
}

void CProfileDlg::enableWindowItems() {
  enableMenuItem(this, ID_EDIT_CONNECT          , m_currentDrawTool->canConnect()      );
  enableMenuItem(this, ID_EDIT_INVERTNORMALS    , m_currentDrawTool->canInvertNormals());
  enableMenuItem(this, ID_EDIT_MIRROR_HORIZONTAL, m_currentDrawTool->canMirror()       );
  enableMenuItem(this, ID_EDIT_MIRROR_VERTICAL  , m_currentDrawTool->canMirror()       );
  enableMenuItem(this, ID_EDIT_DELETE           , m_currentDrawTool->canDelete()       );
  enableMenuItem(this, ID_EDIT_COPY             , m_currentDrawTool->canCopy()         );
  enableMenuItem(this, ID_EDIT_CUT              , m_currentDrawTool->canCut()          );
  getColorButton()->EnableWindow(m_useColor);
}

void CProfileDlg::disableAll() {
  enableWindowList(*this, false
                  ,ID_EDIT_CONNECT
                  ,ID_EDIT_INVERTNORMALS
                  ,ID_EDIT_MIRROR_HORIZONTAL
                  ,ID_EDIT_MIRROR_VERTICAL
                  ,ID_EDIT_DELETE
                  ,ID_EDIT_COPY
                  ,ID_EDIT_CUT
                  ,0);
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
  validateAndUpdate();
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
  m_editor.pushStateFlags().setEnabled(false, SE_RENDER);
  D3ProfileObjectWithColor *visual = NULL;
  if(!m_profile.isEmpty()) {
    visual = new D3ProfileObjectWithColor(this); TRACE_NEW(visual);
  }
  setVisual(visual);
  saveCurrentProfVars();
  m_editor.popStateFlags();
}

void CProfileDlg::setVisual(D3ProfileObjectWithColor *visual) {
  const bool hadOldVisual = m_visual != NULL;
  ProfileObjectProperties oldProperties;
  if(hadOldVisual) {
    m_visual->getAllProperties(oldProperties);
  }
  destroy3DObject();
  if(visual != NULL) {
    if(hadOldVisual) {
      visual->setAllProperties(oldProperties);
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
  m_degrees            = degrees(param.m_rad);
  m_edgeCount          = param.m_edgeCount;
  m_rotateAxis         = format(_T("%c"), param.m_converter.getRotateAxis()).cstr();
  m_rotateAxisAlignsTo = format(_T("%c"), param.m_converter.getRotateAxisAlignsTo()).cstr();
  COLORREF mfcColor    = D3DCOLOR2COLORREF(param.m_color);
  getColorButton()->SetColor(mfcColor);
}

void CProfileDlg::rotateWinToParam(ProfileRotationParameters &param) {
  validateAndUpdate();
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
  param.m_rad                = radians(m_degrees);
  param.m_edgeCount          = m_edgeCount;
  param.m_converter          = Point2DTo3DConverter((char)m_rotateAxis.GetAt(0),(char)m_rotateAxisAlignsTo.GetAt(0));
  const COLORREF mfcColor    = getColorButton()->GetColor();
  param.m_color              = COLORREF2D3DCOLOR(mfcColor);
}

CMFCColorButton *CProfileDlg::getColorButton() const {
  return (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON);
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
  m_profile.setName(name);
  setWindowText(this, name);
}

void CProfileDlg::validateAndUpdate() {
  validate();
  UpdateData();
}

void CProfileDlg::validate() {
  if(!getEditValue(this, IDC_EDIT_DEGREES, m_degrees, false)
    || (m_degrees <= 0) || (m_degrees > 360)) {
    throwValidateException(IDC_EDIT_DEGREES,_T("Degrees must be in ]0..360]"));
  }
  if(!getEditValue(this, IDC_EDIT_EDGECOUNT, m_edgeCount, false)) {
    throwValidateException(IDC_EDIT_EDGECOUNT, _T("Invalid input"));
  }
  try {
    ProfileRotationParameters::validateEdgeCount(m_edgeCount);
  } catch(Exception e) {
    throwValidateException(IDC_EDIT_EDGECOUNT, _T("%s"), e.what());
  }
}

void CProfileDlg::showValidateError(const ValidationException &e) {
  gotoEditBox(this, e.m_field);
  showWarning(e.what());
}

void CProfileDlg::clearException() {
  m_exceptionRaised = false;
  setWindowText(this, IDC_STATIC_INFO, EMPTYSTRING);
}

void CProfileDlg::raiseException(const Exception &e) {
  m_exceptionRaised = true;
  setWindowText(this, IDC_STATIC_INFO, e.what());
}

bool CProfileDlg::getUintValue(int id, UINT &value) {
  const String str = getWindowText(this, id);
  return _stscanf(str.cstr(), _T("%u"), &value) == 1;
}

bool CProfileDlg::getUintEmptyZero(int id, UINT &value) {
  const String str = getWindowText(this, id).trim();
  if(str.length() == 0) {
    value = 0;
    return true;
  } else {
    return _stscanf(str.cstr(), _T("%u"), &value) == 1;
  }
}

void CProfileDlg::setUintEmptyZero(int id, UINT value) {
  setWindowText(this, id, value?format(_T("%u"),value):EMPTYSTRING);
}

void CProfileDlg::showMousePosition(const CPoint &p) {
  if(!m_exceptionRaised) {
    const Point2D fp = m_viewport->backwardTransform(p);
    setWindowText(this, IDC_STATIC_INFO, format(_T("%.2lf,%.2lf"), fp[0], fp[1]));
  }
}

void CProfileDlg::OnFileSave() {
  try {
    validateAndUpdate();
    if(m_profile.hasDefaultName()) {
      saveAs();
    } else {
      save(m_profile.getName());
    }
  } catch(ValidationException e) {
    showValidateError(e);
  } catch(Exception e) {
    showWarning(_T("%s"), e.what());
  }
}

void CProfileDlg::OnFileSaveAs() {
  try {
    validateAndUpdate();
    saveAs();
  } catch(ValidationException e) {
    showValidateError(e);
  } catch(Exception e) {
    showWarning(_T("%s"), e.what());
  }
}

void CProfileDlg::saveAs() {
  CString objName = m_profile.getName().cstr();
  CFileDialog dlg(FALSE,_T("*.prf"),objName);
  dlg.m_ofn.lpstrFilter = Profile2D::s_profileFileExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save Profile");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile);
}

void CProfileDlg::save(const String &fileName) {
  try {
    String extension = FileNameSplitter(fileName).getExtension();
    String fullName = fileName;
    if(extension.length() == 0) {
      fullName = FileNameSplitter(fileName).setExtension(_T("prf")).getFullPath();
    }
    m_profile.save(fullName);
    setProfileName(fullName);
    m_lastSavedProfile = m_profile;
  } catch(Exception e) {
    showException(e);
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
  repaintAll();
}

void CProfileDlg::OnFileOpen() {
  if(!dirtyCheck()) {
    return;
  }
  const String fileName = selectAndValidateProfileFile();
  if(fileName.length() != 0) {
    m_profile.load(fileName);
    repaintAll();
  }
}

void CProfileDlg::OnFileSelectFromFont() {
  if(!dirtyCheck()) {
    return;
  }
  CFontDialog dlg(&m_logFont,CF_EFFECTS | CF_SCREENFONTS,NULL,this);
  if(dlg.DoModal() == IDOK) {
    dlg.GetCurrentFont(&m_logFont);
    CSelectGlyphDlg selectGlyphDlg(m_logFont,this);
    if(selectGlyphDlg.DoModal() == IDOK) {
      m_profile = Profile2D(selectGlyphDlg.getSelectedGlyphCurveData());
      resetView();
    }
  }
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
  m_currentDrawTool->connect();
  repaintAll();
}

void CProfileDlg::OnEditInvertnormals() {
  m_currentDrawTool->invertNormals();
  repaintAll();
}

void CProfileDlg::OnEditMirrorHorizontal() {
  m_currentDrawTool->mirror(true);
  repaintAll();
}

void CProfileDlg::OnEditMirrorVertical() {
  m_currentDrawTool->mirror(false);
  repaintAll();
}

void CProfileDlg::OnViewShowPoints() {
  toggleMenuItem(this,ID_VIEW_SHOWPOINTS);
  repaintAll();
}

void CProfileDlg::OnViewShowNormals() {
  toggleMenuItem(this,ID_VIEW_SHOWNORMALS);
  repaintAll();
}

void CProfileDlg::OnViewAutoUpdate3D() {
  if(toggleMenuItem(this, ID_VIEW_AUTO_UPDATE_3D)) {
    render(SC_RENDER3D);
  }
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
  CPoint sp = (CPoint)getViewport().forwardTransform(p);
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
    setCurrentDrawTool(new DrawToolLine(this));
    break;
  case ID_TOOLS_BEZIERCURVE:
    setCurrentDrawTool(new DrawToolBezierCurve(this));
    break;
  case ID_TOOLS_RECTANGLE:
    setCurrentDrawTool(new DrawToolRectangle(this));
    break;
  case ID_TOOLS_POLYGON:
    setCurrentDrawTool(new DrawToolPolygon(this));
    break;
  case ID_TOOLS_ELLIPSE:
    setCurrentDrawTool(new DrawToolEllipse(this));
    break;
  case ID_TOOLS_SELECT:
    setCurrentDrawTool(new DrawToolSelect(this));
    break;
  default:
    setCurrentDrawTool(new DrawToolNull());
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

void CProfileDlg::OnCancel() {
  if(!dirtyCheck()) return;
  __super::OnCancel();
}

void CProfileDlg::OnOK() {
  if(!dirtyCheck()) return;
  __super::OnOK();
}

void CProfileDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  renderAll();
}

void CProfileDlg::OnCheckDoubleSided()                    { render3D();   }
void CProfileDlg::OnCheckRotateSmooth()                   { render3D();   }
void CProfileDlg::OnEnChangeEditEdgeCount()               { render3D();   }
void CProfileDlg::OnEnChangeEditDegrees()                 { render3D();   }
void CProfileDlg::OnCbnSelchangeComboRotateAxis()         { render3D();   }
void CProfileDlg::OnCbnSelchangeComboRotateAxisAlignsTo() { render3D();   }
void CProfileDlg::OnCheckNormalSmooth()                   { repaintAll(); }
void CProfileDlg::OnButtonRefresh()                       { render3D();   }
void CProfileDlg::OnBnClickedCheckUseColor()              { render3D();   }
void CProfileDlg::OnBnClickedMFCColorButton()             { render3D();   }

void CProfileDlg::OnDeltaposSpinEdgeCount(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UINT value;
  const IntInterval &legalInterval = ProfileRotationParameters ::s_legalEdgeCountInterval;
  if(getUintEmptyZero(IDC_EDIT_EDGECOUNT, value)) {
    value -= pNMUpDown->iDelta;
    value  = minMax((int)value, legalInterval.getFrom(), legalInterval.getTo());
    setEditValue(this, IDC_EDIT_EDGECOUNT, value);
  }
  *pResult = 0;
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
