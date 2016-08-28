#include "stdafx.h"
#include "LightDlg.h"
#include "MaterialDlg.h"
#include "ColorDlg.h"
#include "D3FunctionPlotter.h"
#include "D3FunctionPlotterDlg.h"
#include "Function2DSurfaceDlg.h"
#include "IsoSurfaceDlg.h"
#include "ProfileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CD3FunctionPlotterDlg::CD3FunctionPlotterDlg(CWnd *pParent) : CDialog(CD3FunctionPlotterDlg::IDD, pParent) {
  m_hIcon                = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_currentEditor        = NULL;
  m_calculatedObject     = NULL;
  m_selectedSceneObject  = NULL;
  m_lightDlgThread       = NULL;
  m_materialDlgThread    = NULL;
  m_mouseVisible         = true;
  m_infoVisible          = false;
}

void CD3FunctionPlotterDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CD3FunctionPlotterDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_CLOSE()
  ON_WM_SIZE()
  ON_WM_QUERYDRAGICON()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_MOUSEWHEEL()
  ON_WM_CONTEXTMENU()
  ON_COMMAND(ID_FILE_SAVESTATE                , OnFileSaveState                )
  ON_COMMAND(ID_FILE_LOADSTATE                , OnFileLoadState                )
  ON_COMMAND(ID_FILE_PLOTFUNCTION             , OnFilePlotFunction             )
  ON_COMMAND(ID_FILE_ISOSURFACE               , OnFileIsoSurface               )
  ON_COMMAND(ID_FILE_PROFILESURFACE           , OnFileProfileSurface           )
  ON_COMMAND(ID_FILE_READ3DPOINTSFROMFILE     , OnFileRead3DPointsFromFile     )
  ON_COMMAND(ID_FILE_EXIT                     , OnFileExit                     )
  ON_COMMAND(ID_FILE_NEXTTRY                  , OnFileNexttry                  )
  ON_COMMAND(ID_VIEW_FILLMODE_POINT           , OnViewFillmodePoint            )
  ON_COMMAND(ID_VIEW_FILLMODE_WIREFRAME       , OnViewFillmodeWireframe        )
  ON_COMMAND(ID_VIEW_FILLMODE_SOLID           , OnViewFillmodeSolid            )
  ON_COMMAND(ID_VIEW_SHADING_FLAT             , OnViewShadingFlat              )
  ON_COMMAND(ID_VIEW_SHADING_GOURAUD          , OnViewShadingGouraud           )
  ON_COMMAND(ID_VIEW_SHADING_PHONG            , OnViewShadingPhong             )
  ON_COMMAND(ID_VIEW_SPECULAR                 , OnViewSpecular                 )
  ON_COMMAND(ID_VIEW_SHOWCOORDINATESYSTEM     , OnViewShowCoordinateSystem     )
  ON_COMMAND(ID_VIEW_SHOW3DINFO               , OnViewShow3dinfo               )
  ON_COMMAND(ID_CONTROL_CAMERA_WALK           , OnControlCameraWalk            )
  ON_COMMAND(ID_CONTROL_MOVEROTATEOBJECT      , OnControlMoveRotateObject      )
  ON_COMMAND(ID_SHOW_LIGHTCONTROLS            , OnShowLightControls            )
  ON_COMMAND(ID_HIDE_LIGHTCONTROLS            , OnHideLightControls            )
  ON_COMMAND(ID_ADDLIGHT_DIRECTIONAL          , OnAddLightDirectional          )
  ON_COMMAND(ID_ADDLIGHT_POINT                , OnAddLightPoint                )
  ON_COMMAND(ID_ADDLIGHT_SPOT                 , OnAddLightSpot                 )
  ON_COMMAND(ID_EDIT_AMBIENTLIGHT             , OnEditAmbientLight             )
  ON_COMMAND(ID_EDIT_BACKGROUNDCOLOR          , OnEditBackgroundColor          )
  ON_COMMAND(ID_RESETPOSITIONS                , OnResetPositions               )
  ON_COMMAND(ID_CONTROL_OBJECT_POS            , OnControlObjectPos             )
  ON_COMMAND(ID_CONTROL_OBJECT_SCALE          , OnControlObjectScale           )
  ON_COMMAND(ID_CONTROL_OBJECT_KEEPFOCUS      , OnControlObjectKeepFocus       )
  ON_COMMAND(ID_CONTROL_OBJECT_SCALE_KEEPFOCUS, OnControlObjectScaleKeepFocus  )
  ON_COMMAND(ID_CONTROL_CAMERA_KEEPFOCUS      , OnControlCameraKeepFocus       )
  ON_COMMAND(ID_OBJECT_STARTANIMATION         , OnObjectStartAnimation         )
  ON_COMMAND(ID_OBJECT_STARTBCKANIMATION      , OnObjectStartBckAnimation      )
  ON_COMMAND(ID_OBJECT_STARTALTANIMATION      , OnObjectStartAltAnimation      )
  ON_COMMAND(ID_OBJECT_REVERSEANIMATION       , OnObjectReverseAnimation       )
  ON_COMMAND(ID_OBJECT_STOPANIMATION          , OnObjectStopAnimation          )
  ON_COMMAND(ID_OBJECT_CONTROL_SPEED          , OnObjectControlSpeed           )
  ON_COMMAND(ID_OBJECT_EDITFUNCTION           , OnObjectEditFunction           )
  ON_COMMAND(ID_OBJECT_ADJUSTMATERIAL         , OnObjectEditMaterial           )
  ON_COMMAND(ID_OBJECT_RESETSCALE             , OnObjectResetScale             )
  ON_COMMAND(ID_OBJECT_REMOVE                 , OnObjectRemove                 )
  ON_COMMAND(ID_OBJECT_INFO                   , OnObjectInfo                   )
  ON_COMMAND(ID_LIGHT_ENSABLE                 , OnLightEnable                  )
  ON_COMMAND(ID_LIGHT_DISABLE                 , OnLightDisable                 )
  ON_COMMAND(ID_LIGHT_ADJUSTCOLORS            , OnLightAdjustColors            )
  ON_COMMAND(ID_LIGHT_ADJUSTANGLES            , OnLightAdjustAngles            )
  ON_COMMAND(ID_LIGHTCONTROL_SPOTAT           , OnLightControlSpotAt           )
  ON_COMMAND(ID_LIGHTCONTROL_HIDE             , OnLightControlHide             )
  ON_COMMAND(ID_LIGHT_REMOVE                  , OnLightRemove                  )
  ON_MESSAGE(ID_MSG_RENDER                    , OnMsgRender                    )
END_MESSAGE_MAP()

#define REPAINT() Invalidate(FALSE)

BOOL CD3FunctionPlotterDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATIC_3DPANEL  , RELATIVE_SIZE                  );
  m_layoutManager.addControl(IDC_STATIC_INFOPANEL, RELATIVE_Y_POS | RELATIVE_WIDTH);

  m_scene.init(*get3DPanel());

  setFillMode(    m_scene.getFillMode()   );
  setShadeMode(   m_scene.getShadeMode()  );

  m_coordinateSystem  = new D3CoordinateSystem(m_scene);
  m_lightDlgThread    = CPropertyDlgThread::startThread(new CLightDlg(   m_scene, this));
  m_materialDlgThread = CPropertyDlgThread::startThread(new CMaterialDlg(m_scene, this));

  checkMenuItem(this, ID_VIEW_SPECULAR, m_scene.isSpecularEnabled());
  createInitialObject();

  m_currentControl = CONTROL_IDLE;

  m_scene.setLightDirection(0, rotate(m_scene.getCameraDir(), m_scene.getCameraRight(), 0.2f));
  m_scene.addPropertyChangeListener(this);

  setInfoVisible(isMenuItemChecked(this, ID_VIEW_SHOW3DINFO));
  m_memlogThread.start();
  m_propertyChangeHandlerEnabled = true;

  String title = getWindowText(this);
  String architecture, compileMode;
#ifdef IS32BIT
  architecture = _T("x86");
#else
  architecture = _T("x64");
#endif // IS32BIT
#ifdef _DEBUG
  compileMode = _T("Debug");
#else
  compileMode = _T("Release");
#endif
  title = format(_T("%s %s(%s)"), title.cstr(), compileMode.cstr(), architecture.cstr());
  setWindowText(this, title);
  return TRUE;
}

static D3DXVECTOR3  a(-0.500000f, -0.288675f , 0.204124f);
static D3DXVECTOR3  b(0.500000f , -0.288675f , 0.204124f);
static D3DXVECTOR3  c(0.000000f ,  0.577350f , 0.204124f);
static D3DXVECTOR3  d(0.000000f ,  0.000000f ,-0.612372f);

static D3DXVECTOR3  ab = (a+b)/2;
static D3DXVECTOR3  ac = (a+c)/2;
static D3DXVECTOR3  ad = (a+d)/2;
static D3DXVECTOR3  bc = (b+c)/2;
static D3DXVECTOR3  bd = (b+d)/2;
static D3DXVECTOR3  cd = (c+d)/2;

static Line tetraEder[] = {
  Line( a,b)
 ,Line( a,c)
 ,Line( a,d)
 ,Line( b,c)
 ,Line( b,d)
 ,Line( c,d)
 ,Line(ab,ac)
 ,Line(ac,ad)
 ,Line(ad,ab)

 ,Line(ab,bc)
 ,Line(bc,bd)
 ,Line(bd,ab)

 ,Line(ac,bc)
 ,Line(bc,cd)
 ,Line(cd,ac)

 ,Line(ad,bd)
 ,Line(bd,cd)
 ,Line(cd,ad)

};

void CD3FunctionPlotterDlg::createInitialObject() {
  try {
  D3LineArray *tt = new D3LineArray(m_scene, tetraEder, ARRAYSIZE(tetraEder));
  setCalculatedObject(tt);

//  createSaddle();
//    m_isoSurfaceParam.load("c:\\mytools\\D3FunctionPlotter\\samples\\jacktool.imp");
//  m_isoSurfaceParam.load("c:\\mytools\\D3FunctionPlotter\\samples\\wifflecube.imp");
//    setCalculatedObject(&m_isoSurfaceParam);
  } catch(Exception e) {
    showException(e);
  }
}

D3SceneObject *CD3FunctionPlotterDlg::createRotatedProfile() {
  CoInitialize(0);
  Profile prof;
  prof.load(_T("C:\\mytools\\D3FunctionPlotter\\SAMPLES\\bue.prf"));

  ProfileRotationParameters param;
  param.m_alignx     = 0;
  param.m_aligny     = 1;
  param.m_rad        = radians(360);
  param.m_edgeCount  = 20;
  param.m_smoothness = NORMALSMOOTH | ROTATESMOOTH;
  param.m_rotateAxis = 0;
  SceneObjectWithMesh *obj = new SceneObjectWithMesh(m_scene, rotateProfile(m_scene.getDevice(), prof, param, true));
  obj->setName(prof.getDisplayName());
  return obj;
}

void CD3FunctionPlotterDlg::createSaddle() {
  m_function2DSurfaceParam.setName("Saddle");
  m_function2DSurfaceParam.m_expr        = "(x*x-y*y)/2";
  m_function2DSurfaceParam.m_xInterval   =  DoubleInterval(-1,1);
  m_function2DSurfaceParam.m_yInterval   =  DoubleInterval(-1,1);
  m_function2DSurfaceParam.m_pointCount  = 20;
  m_function2DSurfaceParam.m_includeTime = false;
  m_function2DSurfaceParam.m_machineCode = true;
  m_function2DSurfaceParam.m_doubleSided = true;
  
  setCalculatedObject(&m_function2DSurfaceParam);
}

#define MK_CTRLSHIFT (MK_CONTROL | MK_SHIFT)

static unsigned short getCtrlShiftState() {
  unsigned short flags = 0;
  if(shiftKeyPressed()) flags |= MK_SHIFT;
  if(ctrlKeyPressed()) flags |= MK_CONTROL;
  return flags;
}

void CD3FunctionPlotterDlg::setMouseVisible(bool visible) {
  if(visible == m_mouseVisible) {
    return;
  }
  ShowCursor(visible?TRUE:FALSE);
  m_mouseVisible = visible;
}

BOOL CD3FunctionPlotterDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  switch(pMsg->message) {
  case WM_KEYDOWN:
  case WM_KEYUP  :
    switch(m_currentControl) {
    case CONTROL_OBJECT_POS:
      switch(getCtrlShiftState() & MK_CTRLSHIFT) {
      case 0           :
      case MK_CONTROL  :
      case MK_CTRLSHIFT:
        setMouseVisible(true);
        break;
      case MK_SHIFT    :
        setMouseVisible(false);
        break;
      }
    }
    break;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CD3FunctionPlotterDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

void CD3FunctionPlotterDlg::OnPaint() {
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
    CDialog::OnPaint();

    m_scene.render();
    show3DInfo();
  }
}

void CD3FunctionPlotterDlg::OnSize(UINT nType, int cx, int cy) {
  if(cx < 200 || cy < 200) {
    return;
  }
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
  m_scene.OnSize();
}

void CD3FunctionPlotterDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(!m_propertyChangeHandlerEnabled) {
    return;
  }
  if(source == &m_scene) {
    switch(id) {
    case SP_FILLMODE                :
    case SP_SHADEMODE               :
    case SP_CAMERAPOS               :
    case SP_CAMERAORIENTATION       :
    case SP_OBJECTPOS               :
    case SP_OBJECTORIENTATION       :
    case SP_OBJECTSCALE             :
    case SP_PROJECTIONTRANSFORMATION:
    case SP_LIGHTPARAMETERS         :
    case SP_AMBIENTLIGHT            :
    case SP_SPECULARENABLED         :
    case SP_MATERIALPARAMETERS      :
    case SP_BACKGROUNDCOLOR         :
      SendMessage(ID_MSG_RENDER, true);
      break;
    case SP_ANIMATIONFRAMEINDEX     :
      SendMessage(ID_MSG_RENDER, false);
      break;
    }
  } else if(source == m_lightDlgThread->getPropertyContainer()) {
    switch(id) {
    case SP_LIGHTPARAMETERS:
      { const LIGHT &lp = *(LIGHT*)newValue;
        m_scene.setLightParam(lp);
      }
      break;
    }
  } else if(source == m_materialDlgThread->getPropertyContainer()) {
    switch(id) {
    case SP_MATERIALPARAMETERS:
      m_scene.setMaterial(*(MATERIAL*)newValue);
      break;
    }
  } else if(source == m_currentEditor) {
    switch(id) {
    case SP_BACKGROUNDCOLOR:
      m_scene.setBackgroundColor(*(D3DCOLOR*)newValue);
      break;
    case SP_AMBIENTLIGHT           :
      m_scene.setGlobalAmbientColor(*(D3DCOLOR*)newValue);
      break;
    }
  }
}

LRESULT CD3FunctionPlotterDlg::OnMsgRender(WPARAM wp, LPARAM lp) {
  m_scene.render();
  if(wp) {
    show3DInfo();
  }
  return 0;
}

HCURSOR CD3FunctionPlotterDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CD3FunctionPlotterDlg::setCalculatedObject(Function2DSurfaceParameters *param) {
  DIRECT3DDEVICE device = m_scene.getDevice();
  if(param->m_includeTime) {
    setCalculatedObject(new D3AnimatedSurface(m_scene, createMeshArray(this, device, *param)), param);
  } else {
    setCalculatedObject(new SceneObjectWithMesh(m_scene, createMesh(device, *param)), param);
  }
}
/*
void CD3FunctionPlotterDlg::setCalculatedObject(IsoSurfaceParameters *param) {
  setCalculatedObject(createIsoSurfaceDebugObject(m_scene, *param), param);
}
*/

void CD3FunctionPlotterDlg::setCalculatedObject(IsoSurfaceParameters *param) {
  DIRECT3DDEVICE device = m_scene.getDevice();

//  if(param->m_adaptiveCellSize) {
//    setCalculatedObject(new SceneObjectWithMesh(m_scene, createMeshMarchingCube(device, *param)), param);
//  } else {
    if(param->m_includeTime) {
      setCalculatedObject(new D3AnimatedSurface(m_scene, createMeshArray(this, device, *param)), param);
    } else {
      setCalculatedObject(new SceneObjectWithMesh(m_scene, createMesh(device, *param)), param);
    }
//  }
}


void CD3FunctionPlotterDlg::setCalculatedObject(D3SceneObject *obj, PersistentParameter *param) {
  if(m_calculatedObject) {
    m_scene.removeSceneObject(m_calculatedObject);
    delete m_calculatedObject;
  }
  m_calculatedObject = obj;
  if(m_calculatedObject) {
    m_scene.addSceneObject(m_calculatedObject);
    m_calculatedObject->setUserData(param);
    if(param) {
      m_calculatedObject->setName(param->getDisplayName());
    }
  }
  setSelectedObject(obj);
}

void CD3FunctionPlotterDlg::setSelectedObject(D3SceneObject *obj) {
  m_selectedSceneObject = obj;
  const D3LightControl *lc = getSelectedLightControl();
  if(lc) {
    m_lightDlgThread->setCurrentDialogProperty(&lc->getLightParam());
  } else if(obj) {
    m_materialDlgThread->setCurrentDialogProperty(&m_scene.getMaterial());
  }
}

static const String stateFileName = "c:\\temp\\D3FunctionPlotter.dat";

void CD3FunctionPlotterDlg::OnFileSaveState() {
  try {
    m_scene.saveState(stateFileName);
    Message(_T("State saved"));
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFileLoadState() {
  try {
    m_scene.loadState(stateFileName);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFilePlotFunction() {
  try {
    CFunction2DSurfaceDlg dlg(m_function2DSurfaceParam);
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_function2DSurfaceParam = dlg.m_param;
    setCalculatedObject(&m_function2DSurfaceParam);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFileIsoSurface() {
  try {
    CIsoSurfaceDlg dlg(m_isoSurfaceParam);
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_isoSurfaceParam = dlg.m_param;
    setCalculatedObject(&m_isoSurfaceParam);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFileProfileSurface() {
  const String fileName = "C:\\mytools\\D3FunctionPlotter\\SAMPLES\\bue.prf";
  try {
    FILE *f = FOPEN(fileName, "r");
    Profile profile(readFile(f), fileName);
    fclose(f);
    CProfileDlg dlg(profile);
    dlg.DoModal();
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFileRead3DPointsFromFile() {
  try {
    static const TCHAR *fileExtensions = _T("Text files (*.txt)\0*.txt\0"
                                            "Data files (*.dat)\0*.dat\0"
                                            "All files (*.*)\0*.*\0\0");

    DIRECT3DDEVICE device = m_scene.getDevice();

    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrFilter = fileExtensions;
    dlg.m_ofn.lpstrTitle  = _T("Open Vertex file");
    if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
      return;
    } else {
      const String fileName = dlg.m_ofn.lpstrFile;
      setCalculatedObject(new SceneObjectWithMesh(m_scene, createMeshFromVertexFile(device, fileName, true)));
      REPAINT();
    }	
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFileNexttry() {
  try {
    setCalculatedObject(&m_isoSurfaceParam);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFileExit() {
  m_lightDlgThread->kill();
  m_materialDlgThread->kill();
  m_scene.removeAllSceneObjects();
  EndDialog(IDOK);
}

void CD3FunctionPlotterDlg::OnCancel() {}
void CD3FunctionPlotterDlg::OnOK()     {}

void CD3FunctionPlotterDlg::OnClose() {
  OnFileExit();
}

void CD3FunctionPlotterDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  m_lastMouse = point = get3DPanelPoint(point, false);
  m_pickedRay = m_scene.getPickRay(m_lastMouse);
  CDialog::OnLButtonDown(nFlags, point);
  switch(m_currentControl) {
  case CONTROL_SPOTLIGHTPOINT:
    setSpotToPointAt(point);
    break;

  default:
    { D3DXVECTOR3 hitPoint;
      D3SceneObject *pickedObj = m_scene.getPickedObject(point, PICK_ALL, &hitPoint);
      if(pickedObj == NULL) {
        if(m_currentControl != CONTROL_CAMERA_WALK) {
          setSelectedObject(NULL);
          setCurrentControl(CONTROL_IDLE);
        } else {
          setMouseVisible(false);
          SetCapture();
        }
      } else {
        m_pickedPoint = hitPoint;
        setSelectedObject(pickedObj);
        switch(pickedObj->getType()) {
        case SOTYPE_VISUALOBJECT  :
        case SOTYPE_ANIMATEDOBJECT:
          setCurrentControl(CONTROL_OBJECT_POS);
          break;
        case SOTYPE_LIGHTCONTROL  :
          setCurrentControl(CONTROL_LIGHT);
          break;
        }
      }
    }
    break;
  }
}

void CD3FunctionPlotterDlg::OnContextMenu(CWnd *pWnd, CPoint point) {
  m_lastMouse = get3DPanelPoint(point, true);
  m_pickedRay = m_scene.getPickRay(m_lastMouse);
  D3DXVECTOR3 hitPoint;
  D3SceneObject *pickedObject = m_scene.getPickedObject(m_lastMouse, PICK_ALL, &hitPoint);
  if(pickedObject == NULL) {
    onContextMenuBackground(point);
  } else {
    m_pickedPoint = hitPoint;
    setSelectedObject(pickedObject);
    onContextMenuSceneObject(point);
  }
}

void CD3FunctionPlotterDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  point = get3DPanelPoint(point, false);
  switch(m_currentControl) {
  case CONTROL_SPOTLIGHTPOINT:
    setCurrentControl(CONTROL_IDLE);
    break;
  case CONTROL_OBJECT_POS:
  case CONTROL_CAMERA_WALK:
    setMouseVisible(true);
    ReleaseCapture();
    break;
  }
  CDialog::OnLButtonUp(nFlags, point);
}

void CD3FunctionPlotterDlg::OnMouseMove(UINT nFlags, CPoint point) {
  if(!(nFlags & MK_LBUTTON)) {
    return;
  }
  point = get3DPanelPoint(point, false);
  switch(m_currentControl) {
  case CONTROL_CAMERA_WALK   :
    onMouseMoveCameraWalk(nFlags, point);
    break;
  case CONTROL_OBJECT_POS    :
    onMouseMoveObjectPos(nFlags, point);
    break;
  case CONTROL_LIGHT          :
    onMouseMoveLight(nFlags, point);
    break;
  case CONTROL_SPOTLIGHTPOINT:
    setSpotToPointAt(point);
    break;
  }
  m_lastMouse = point;
  CDialog::OnMouseMove(nFlags, point);
}

CPoint CD3FunctionPlotterDlg::get3DPanelPoint(CPoint point, bool screenRelative) const {
  if(!screenRelative) {
    ClientToScreen(&point);
  }
  get3DPanel()->ScreenToClient(&point);
  return point;
}

void CD3FunctionPlotterDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
/*
  point = get3DPanelPoint(point, false);

  bool repaint = false;
  static D3PickRayArrow *lastPickedRayArrow = NULL;
  if(lastPickedRayArrow != NULL) {
    m_scene.removeSceneObject(lastPickedRayArrow);
    delete lastPickedRayArrow;
    lastPickedRayArrow = NULL;
    repaint = true;
  }
  if(nFlags & MK_CONTROL) {
    const D3Ray ray = m_scene.getPickRay(point);
    showInfo("Ray:%s", ray.toString().cstr());
    lastPickedRayArrow = new D3PickRayArrow(m_scene, ray);
    m_scene.addSceneObject(lastPickedRayArrow);
    repaint = true;
  }   
  D3SceneObject *pickedObj = m_scene.getPickedObject(point);
  if(pickedObj != NULL) {
    setSelectedObject(pickedObj);
  }
  if(repaint) {
    REPAINT();
  }
*/
  CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL CD3FunctionPlotterDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  pt = get3DPanelPoint(pt, false);
  switch(m_currentControl) {
  case CONTROL_CAMERA_WALK           :
    onMouseWheelCameraWalk(          nFlags, zDelta, pt);
    break;
  case CONTROL_OBJECT_POS            :
    onMouseWheelObjectPos(           nFlags, zDelta, pt);
    break;
  case CONTROL_OBJECT_SCALE          :
    onMouseWheelObjectScale(         nFlags, zDelta, pt);
    break;
  case CONTROL_OBJECT_POS_KEEPFOCUS  :
    onMouseWheelObjectPosKeepFocus(  nFlags, zDelta, pt);
    break;
  case CONTROL_OBJECT_SCALE_KEEPFOCUS:
    onMouseWheelObjectScaleKeepFocus(nFlags, zDelta, pt);
    break;
  case CONTROL_CAMERA_KEEPFOCUS      :
    onMouseWheelCameraKeepFocus(     nFlags, zDelta, pt);
    break;
  case CONTROL_ANIMATION_SPEED       :
    onMouseWheelAnimationSpeed(      nFlags, zDelta, pt);
    break;
  case CONTROL_LIGHT                 :
    onMouseWheelLight(               nFlags, zDelta, pt);
    break;
  case CONTROL_SPOTANGLES            :
    onMouseWheelLightSpotAngle(      nFlags, zDelta, pt);
    break;
  }
  return CDialog::OnMouseWheel(      nFlags, zDelta, pt);
}

// ---------------------------------- Controlling object -----------------------------------------

void CD3FunctionPlotterDlg::onMouseMoveObjectPos(UINT nFlags, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveSceneObjectXY(pt, SOTYPE_VISUALOBJECT);
    break;
  case MK_SHIFT    :
    moveSceneObjectXZ(pt, SOTYPE_VISUALOBJECT);
    break;
  case MK_CONTROL  :
    rotateObjectFrwBckw(double(pt.x - m_lastMouse.x) / 100.0,
                        double(pt.y - m_lastMouse.y) / 100.0);
    break;
  case MK_CTRLSHIFT:
    rotateObjectLeftRight(double(pt.x - m_lastMouse.x) / 100.0);
    break;
  }
}

void CD3FunctionPlotterDlg::moveSceneObjectXY(CPoint pt, SceneObjectType type, int lightIndex) {
  const D3DXVECTOR3 dp             = getSceneObjectPos(type,lightIndex) - m_pickedPoint;
  const float       dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = m_scene.getPickRay(pt);
  D3DXVECTOR3       newPickedPoint = newPickedRay.m_orig + newPickedRay.m_dir * dist;
  setSceneObjectPos(newPickedPoint + dp, type, lightIndex);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

void CD3FunctionPlotterDlg::moveSceneObjectXZ(CPoint pt, SceneObjectType type, int lightIndex) {
  const D3DXVECTOR3 dp             = getSceneObjectPos(type,lightIndex) - m_pickedPoint;
  const float       dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = m_scene.getPickRay(pt);
  const D3Ray       ray1           = m_scene.getPickRay(CPoint(pt.x,pt.y+1));
  const double      dRaydPixel     = length((newPickedRay.m_orig + dist * newPickedRay.m_dir) - (ray1.m_orig + dist * ray1.m_dir));
  const CSize       dMouse         = pt - m_lastMouse;
  D3DXVECTOR3       newPickedPoint = m_pickedPoint 
                                   + (float)(-dMouse.cy * dRaydPixel) * m_scene.getCameraDir()
                                   + (float)( dMouse.cx * dRaydPixel) * m_scene.getCameraRight();

  setSceneObjectPos(newPickedPoint + dp, type, lightIndex);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

D3DXVECTOR3 CD3FunctionPlotterDlg::getSceneObjectPos(SceneObjectType type, int lightIndex) {
  switch(type) {
  case SOTYPE_VISUALOBJECT:
    return m_scene.getObjPos();
    break;
  case SOTYPE_LIGHTCONTROL:
    return m_scene.getLightParam(lightIndex).Position;
    break;
  default:
    throwInvalidArgumentException(_T("getSceneObjectPos"), _T("type=%d"), type);
    return D3DXVECTOR3(0,0,0);
  }
}

void CD3FunctionPlotterDlg::setSceneObjectPos(const D3DXVECTOR3 &pos, SceneObjectType type, int lightIndex) {
  switch(type) {
  case SOTYPE_VISUALOBJECT:
    m_scene.setObjPos(pos);
    break;
  case SOTYPE_LIGHTCONTROL:
    m_scene.setLightPosition(lightIndex, pos);
    break;
  default:
    throwInvalidArgumentException(_T("setSceneObjectPos"), _T("type=%d"), type);
  }
}

void CD3FunctionPlotterDlg::onMouseWheelObjectPos(UINT nFlags, short zDelta, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3DXVECTOR3 objPos = m_scene.getObjPos();
      const D3DXVECTOR3 camPos = m_scene.getCameraPos();
      const D3DXVECTOR3 dir    = objPos - camPos;
      const float       dist   = length(dir);
      if(dist > 0) {
        m_scene.setObjPos(objPos + unitVector(dir) * dist / 30.0 * (float)sign(zDelta));
      }
    }
    break;
  case MK_CONTROL  :
    rotateObjectFrwBckw(0, (double)sign(zDelta) / -50.0);
    break;
  case MK_SHIFT    :
    rotateObjectFrwBckw((double)sign(zDelta) / -50, 0);
    break;
  case MK_CTRLSHIFT:
    rotateObjectLeftRight((double)sign(zDelta) / 50.0);
    break;
  }
}

void CD3FunctionPlotterDlg::rotateObjectFrwBckw(double angle1, double angle2) {
  const D3DXVECTOR3 cameraUp    = m_scene.getCameraUp();
  const D3DXVECTOR3 cameraRight = m_scene.getCameraRight();
  const D3DXVECTOR3 objDir      = rotate(rotate(m_scene.getObjDir(),cameraUp,angle1),cameraRight,angle2);
  const D3DXVECTOR3 objUp       = rotate(rotate(m_scene.getObjUp() ,cameraUp,angle1),cameraRight,angle2);
  m_scene.setObjOrientation(objDir, objUp);
}

void CD3FunctionPlotterDlg::rotateObjectLeftRight(double angle) {
  const D3DXVECTOR3 cameraDir = m_scene.getCameraDir();
  m_scene.setObjOrientation(rotate(m_scene.getObjDir(),cameraDir, angle), rotate(m_scene.getObjUp(), cameraDir, angle));
}

#define VADJUST_X    0x01
#define VADJUST_Y    0x02
#define VADJUST_Z    0x04
#define VADJUST_ALL  (VADJUST_X | VADJUST_Y | VADJUST_Z)

void CD3FunctionPlotterDlg::onMouseWheelObjectScale(UINT nFlags, short zDelta, CPoint pt) {
  const double factor = (1.0+(double)sign(zDelta)*0.04);
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : adjustScale(VADJUST_ALL, factor); break;
  case MK_CONTROL  : adjustScale(VADJUST_X  , factor); break;
  case MK_SHIFT    : adjustScale(VADJUST_Y  , factor); break;
  case MK_CTRLSHIFT: adjustScale(VADJUST_Z  , factor); break;
  }
}

void CD3FunctionPlotterDlg::onMouseWheelObjectPosKeepFocus(UINT nFlags, short zDelta, CPoint pt) {
  const D3PosDirUpScale pdus = m_scene.getObjPDUS();

  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3DXVECTOR3 camPos  = m_scene.getCameraPos();
      const float       dist    = length(m_focusPoint - camPos);
      D3DXVECTOR3       dp      = pdus.getPos() - m_focusPoint;
      m_focusPoint = camPos + (float)(dist * (1.0 - (float)sign(zDelta)/30)) * m_scene.getCameraDir();
      m_scene.setObjPos(m_focusPoint + dp);
    }
    break;
  case MK_CONTROL  :
  case MK_SHIFT    :
  case MK_CTRLSHIFT:
    { m_propertyChangeHandlerEnabled = false;
      const D3DXVECTOR3 objFocus = invers(pdus.getRotationMatrix()) * (m_focusPoint - pdus.getPos());
      onMouseWheelObjectPos(nFlags, zDelta, pt);
      m_propertyChangeHandlerEnabled = true;
      m_scene.setObjPos(m_focusPoint - m_scene.getObjPDUS().getRotationMatrix() * objFocus);
    }
    break;
  }
}

void CD3FunctionPlotterDlg::onMouseWheelObjectScaleKeepFocus(UINT nFlags, short zDelta, CPoint pt) {
  const D3PosDirUpScale pdus = m_scene.getObjPDUS();
  m_propertyChangeHandlerEnabled = false;

  const D3DXVECTOR3 objFocus = invers(pdus.getScaleMatrix()) * (m_focusPoint - pdus.getPos());
  onMouseWheelObjectScale(nFlags, zDelta, pt);
  m_propertyChangeHandlerEnabled = true;
  m_scene.setObjPos(m_focusPoint - m_scene.getObjPDUS().getScaleMatrix() * objFocus);
}

void CD3FunctionPlotterDlg::onMouseWheelAnimationSpeed(UINT nFlags, short zDelta, CPoint pt) {
  const double factor = (1.0+(double)sign(zDelta)*0.04);
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj && obj->isRunning()) {
    obj->scaleSpeed(factor);
  }
}

void CD3FunctionPlotterDlg::adjustScale(int component, double factor) {
  D3DXVECTOR3 scale = m_scene.getObjScale();
  if(component & VADJUST_X) {
    scale.x *= (float)factor;
  }
  if(component & VADJUST_Y) {
    scale.y *= (float)factor;
  }
  if(component & VADJUST_Z) {
    scale.z *= (float)factor;
  }
  m_scene.setObjScale(scale);
}

// ------------------------------------- controlling camera -----------------------------------------

void CD3FunctionPlotterDlg::onMouseMoveCameraWalk(UINT nFlags, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    walkWithCamera(double(pt.y - m_lastMouse.y) / -20, double(pt.x - m_lastMouse.x) / -400.0);
    break;
  case MK_CONTROL  :
    rotateCameraUpDown(double(pt.y - m_lastMouse.y) / 400.0);
    break;
  case MK_SHIFT    :
    sidewalkWithCamera(double(pt.y - m_lastMouse.y) / -40, double(pt.x - m_lastMouse.x) / 40);
    break;
  case MK_CTRLSHIFT:
    rotateCameraLeftRight(double(pt.x - m_lastMouse.x) / 200.0);
    break;
  }
}

void CD3FunctionPlotterDlg::onMouseWheelCameraWalk( UINT nFlags, short zDelta, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    walkWithCamera((double)sign(zDelta) * 0.25, 0);
    break;
  case MK_CONTROL  :
    m_scene.setNearViewPlane(m_scene.getNearViewPlane() * (1 + (float)sign(zDelta)*0.05f));
    break;
  case MK_SHIFT    :
    walkWithCamera((double)sign(zDelta) * 1.5, 0);
    break;
  case MK_CTRLSHIFT:
    { const float a = m_scene.getViewAngel();
      const float d = ((a > D3DX_PI/2) ? (D3DX_PI - a) : a) / (D3DX_PI/2);
      m_scene.setViewAngel(a + d * sign(zDelta) * 0.04f);
    }
    break;
  }
}

void CD3FunctionPlotterDlg::onMouseWheelCameraKeepFocus(UINT nFlags, short zDelta, CPoint pt) {
  const D3DXVECTOR3 &center = m_focusPoint;
  const D3DXVECTOR3 camPos  = m_scene.getCameraPos();
  const float       radius  = length(center - camPos);
  
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    m_scene.setCameraPos(camPos + m_scene.getCameraDir() * radius / 30.0f * (float)sign(zDelta));
    break;
  case MK_CONTROL  :
    { const D3DXVECTOR3 up     = m_scene.getCameraUp();
            D3DXVECTOR3 newPos = camPos + unitVector(up) * radius / 20.0f * (float)sign(zDelta);
      const D3DXVECTOR3 newDir = unitVector(center - newPos);
      const D3DXVECTOR3 newUp  = crossProduct(m_scene.getCameraRight(), newDir);
      m_propertyChangeHandlerEnabled = false;
      m_scene.setCameraPos(center - newDir * radius);
      m_propertyChangeHandlerEnabled = true;
      m_scene.setCameraOrientation(newDir, newUp);
    }
    break;
  case MK_SHIFT    :
    { const D3DXVECTOR3 right  = m_scene.getCameraRight();
            D3DXVECTOR3 newPos = camPos + unitVector(right) * radius / 20.0f * (float)sign(zDelta);
      const D3DXVECTOR3 newDir = unitVector(center - newPos);
      m_propertyChangeHandlerEnabled = false;
      m_scene.setCameraPos(center - newDir * radius);
      m_propertyChangeHandlerEnabled = true;
      m_scene.setCameraOrientation(newDir, m_scene.getCameraUp());
    }
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void CD3FunctionPlotterDlg::walkWithCamera(double dist, double angle) {
  const D3DXVECTOR3 dir = m_scene.getCameraDir();
  const D3DXVECTOR3 up  = m_scene.getCameraUp();
  m_scene.setCameraPos(m_scene.getCameraPos() + (float)dist * dir);
  m_scene.setCameraOrientation(rotate(dir, up, angle), up);
}

void CD3FunctionPlotterDlg::sidewalkWithCamera(double upDist, double rightDist) {
  m_scene.setCameraPos(m_scene.getCameraPos() + (float)upDist * m_scene.getCameraUp() + (float)rightDist * m_scene.getCameraRight());
}

void CD3FunctionPlotterDlg::rotateCameraUpDown(double angle) {
  const D3DXVECTOR3 right = m_scene.getCameraRight();;
  m_scene.setCameraOrientation(rotate(m_scene.getCameraDir(), right, angle), rotate(m_scene.getCameraUp() , right, angle));
}

void CD3FunctionPlotterDlg::rotateCameraLeftRight(double angle) {
  const D3DXVECTOR3 dir = m_scene.getCameraDir();
  m_scene.setCameraOrientation(dir, rotate(m_scene.getCameraUp(), dir, angle));
}

// ------------------------------------- controlling lights -----------------------------------------

void CD3FunctionPlotterDlg::onMouseMoveLight(UINT nFlags, CPoint pt) {
  D3LightControl *lc = getSelectedLightControl();
  if(lc) {
    switch(lc->getLightType()) {
    case D3DLIGHT_POINT:
      onMouseMoveLightPoint(*(D3LightControlPoint*)lc, nFlags, pt);
      break;
    case D3DLIGHT_SPOT :
      onMouseMoveLightSpot(*(D3LightControlSpot*)lc, nFlags, pt);
      break;
    }
  }
}

void CD3FunctionPlotterDlg::onMouseMoveLightPoint(D3LightControlPoint &ctrl, UINT nFlags, CPoint pt) {
  const D3DXVECTOR3 pos = ctrl.getLightParam().Position;

  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveSceneObjectXY(pt, SOTYPE_LIGHTCONTROL, ctrl.getLightIndex());
    break;
  case MK_SHIFT    :
    moveSceneObjectXZ(pt, SOTYPE_LIGHTCONTROL, ctrl.getLightIndex());
    break;
  case MK_CONTROL  :
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void CD3FunctionPlotterDlg::onMouseMoveLightSpot(D3LightControlSpot &ctrl, UINT nFlags, CPoint pt) {
  const LIGHT       param = ctrl.getLightParam();
  const D3DXVECTOR3 pos   = param.Position;
  const D3DXVECTOR3 dir   = param.Direction;

  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveSceneObjectXY(pt, SOTYPE_LIGHTCONTROL, ctrl.getLightIndex());
    break;
  case MK_SHIFT    :
    moveSceneObjectXZ(pt, SOTYPE_LIGHTCONTROL, ctrl.getLightIndex());
    break;
  case MK_CONTROL  :
    { const double angle1 = (double)(pt.x - m_lastMouse.x) / 100.0;
      const double angle2 = (double)(pt.y - m_lastMouse.y) / 100.0;
      const D3DXVECTOR3 newDir = rotate(rotate(dir,m_scene.getCameraUp(),angle1),m_scene.getCameraRight(),angle2);
      m_scene.setLightDirection(ctrl.getLightIndex(), newDir);
    }
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void CD3FunctionPlotterDlg::onMouseWheelLight(UINT nFlags, short zDelta, CPoint pt) {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  switch(lc->getLightType()) {
  case D3DLIGHT_DIRECTIONAL: onMouseWheelLightDirectional(*(D3LightControlDirectional*)lc, nFlags, zDelta, pt); break;
  case D3DLIGHT_POINT      : onMouseWheelLightPoint(      *(D3LightControlPoint*)lc      , nFlags, zDelta, pt); break;
  case D3DLIGHT_SPOT       : onMouseWheelLightSpot(       *(D3LightControlSpot*)lc       , nFlags, zDelta, pt); break;
  }
}

void CD3FunctionPlotterDlg::onMouseWheelLightDirectional(D3LightControlDirectional &ctrl, UINT nFlags, short zDelta, CPoint pt) {
  const D3DXVECTOR3 dir = ctrl.getLightParam().Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : 
    ctrl.setSphereRadius(ctrl.getSphereRadius() * (1.0f-(float)sign(zDelta)*0.04f));
    m_scene.render();
    break;
  case MK_CONTROL  :
    m_scene.setLightDirection(ctrl.getLightIndex(), rotate(dir, m_scene.getCameraRight(), -(double)sign(zDelta)*0.06));
    break;
  case MK_SHIFT    : 
    m_scene.setLightDirection(ctrl.getLightIndex(), rotate(dir, m_scene.getCameraUp(),    -(double)sign(zDelta)*0.06));
    break;
  case MK_CTRLSHIFT: 
    m_scene.setLightDirection(ctrl.getLightIndex(), rotate(dir, m_scene.getCameraDir(),    (double)sign(zDelta)*0.06));
    break;
  }
}

void CD3FunctionPlotterDlg::onMouseWheelLightPoint(D3LightControlPoint &ctrl, UINT nFlags, short zDelta, CPoint pt) {
  const D3DXVECTOR3 pos = ctrl.getLightParam().Position;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : 
    m_scene.setLightPosition(ctrl.getLightIndex(), pos + m_scene.getCameraDir()   * (float)sign(zDelta)*0.04f);
    break;
  case MK_CONTROL  :
    m_scene.setLightPosition(ctrl.getLightIndex(), pos + m_scene.getCameraUp()    * (float)sign(zDelta)*0.04f);
    break;
  case MK_SHIFT    : 
    m_scene.setLightPosition(ctrl.getLightIndex(), pos + m_scene.getCameraRight() * (float)sign(zDelta)*0.04f);
    break;
  case MK_CTRLSHIFT: 
    break;
  }
}

void CD3FunctionPlotterDlg::onMouseWheelLightSpot(D3LightControlSpot &ctrl, UINT nFlags, short zDelta, CPoint pt) {
  const LIGHT       param = ctrl.getLightParam();
  const D3DXVECTOR3 pos   = param.Position;
  const D3DXVECTOR3 dir   = param.Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : 
    m_scene.setLightPosition( ctrl.getLightIndex(), pos + m_scene.getCameraUp() * (float)sign(zDelta)*0.04f);
    break;
  case MK_CONTROL  :
    m_scene.setLightDirection(ctrl.getLightIndex(), rotate(dir, m_scene.getCameraRight(), -(float)sign(zDelta)*0.06));
    break;
  case MK_SHIFT    : 
    m_scene.setLightPosition( ctrl.getLightIndex(), pos + m_scene.getCameraRight()   * (float)sign(zDelta)*0.04f);
    break;
  case MK_CTRLSHIFT: 
    m_scene.setLightDirection(ctrl.getLightIndex(), rotate(dir, m_scene.getCameraUp(), (float)sign(zDelta)*0.06));
    break;
  }
}

void CD3FunctionPlotterDlg::onMouseWheelLightSpotAngle(UINT nFlags, short zDelta, CPoint pt) {
  const D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL || lc->getLightType() != D3DLIGHT_SPOT) {
    return;
  }
  LIGHT light = lc->getLightParam();
  switch(nFlags) {
  case 0           :
    light.setInnerAngle(light.getInnerAngle() * (1.0f - (float)sign(zDelta) / 10.0f));
    break;
  case MK_SHIFT    :
    light.setOuterAngle(light.getOuterAngle() * (1.0f - (float)sign(zDelta) / 10.0f));
    break;
  default:
    return;
  }
  m_scene.setLightParam(light);
}
// ------------------------------------------------------------------------------------------------------------

void CD3FunctionPlotterDlg::OnViewFillmodePoint()     { setFillMode(D3DFILL_POINT     ); }
void CD3FunctionPlotterDlg::OnViewFillmodeWireframe() { setFillMode(D3DFILL_WIREFRAME ); }
void CD3FunctionPlotterDlg::OnViewFillmodeSolid()     { setFillMode(D3DFILL_SOLID     ); }

void CD3FunctionPlotterDlg::OnViewShadingFlat()       { setShadeMode(D3DSHADE_FLAT    ); }
void CD3FunctionPlotterDlg::OnViewShadingGouraud()    { setShadeMode(D3DSHADE_GOURAUD ); }
void CD3FunctionPlotterDlg::OnViewShadingPhong()      { setShadeMode(D3DSHADE_PHONG   ); }

void CD3FunctionPlotterDlg::OnViewSpecular() {
  m_scene.enableSpecular(toggleMenuItem(this, ID_VIEW_SPECULAR));
}

void CD3FunctionPlotterDlg::OnViewShow3dinfo() {
  setInfoVisible(toggleMenuItem(this, ID_VIEW_SHOW3DINFO));
}

void CD3FunctionPlotterDlg::setInfoVisible(bool visible) {
  const CRect cl = getClientRect(this);
  CRect upperRect, lowerRect;

  if(visible) {
    upperRect = getWindowRect(get3DPanel());
    upperRect.bottom = cl.Height() - 100;
    lowerRect = CRect(upperRect.left, upperRect.bottom, upperRect.right, cl.bottom);
  } else {
    upperRect = cl;
  }

  if(visible) {
    setWindowRect(get3DPanel()  , upperRect);
    setWindowRect(getInfoPanel(), lowerRect);
    getInfoPanel()->ShowWindow(SW_SHOW);
    m_scene.OnSize();
    m_infoVisible = visible;
    show3DInfo();
  } else {
    getInfoPanel()->ShowWindow(SW_HIDE);
    setWindowRect(get3DPanel()  , upperRect);
    m_scene.OnSize();
    m_infoVisible = visible;
  }
}

void CD3FunctionPlotterDlg::OnViewShowCoordinateSystem() {
  m_coordinateSystem->setVisible(toggleMenuItem(this, ID_VIEW_SHOWCOORDINATESYSTEM));
  m_scene.render();
}

void CD3FunctionPlotterDlg::setFillMode(D3DFILLMODE fillMode) {
  checkMenuItem(this, ID_VIEW_FILLMODE_POINT    , fillMode == D3DFILL_POINT    );
  checkMenuItem(this, ID_VIEW_FILLMODE_WIREFRAME, fillMode == D3DFILL_WIREFRAME);
  checkMenuItem(this, ID_VIEW_FILLMODE_SOLID    , fillMode == D3DFILL_SOLID    );
  m_scene.setFillMode(fillMode);
}

void CD3FunctionPlotterDlg::setShadeMode(D3DSHADEMODE shadeMode) {
  checkMenuItem(this, ID_VIEW_SHADING_FLAT      , shadeMode == D3DSHADE_FLAT    );
  checkMenuItem(this, ID_VIEW_SHADING_GOURAUD   , shadeMode == D3DSHADE_GOURAUD );
  checkMenuItem(this, ID_VIEW_SHADING_PHONG     , shadeMode == D3DSHADE_PHONG   );
  m_scene.setShadeMode(shadeMode);
}

void CD3FunctionPlotterDlg::OnAddLightDirectional() { addLight(D3DLIGHT_DIRECTIONAL); }
void CD3FunctionPlotterDlg::OnAddLightPoint()       { addLight(D3DLIGHT_POINT);       }
void CD3FunctionPlotterDlg::OnAddLightSpot()        { addLight(D3DLIGHT_SPOT);        }

void CD3FunctionPlotterDlg::addLight(D3DLIGHTTYPE type) {
  try {
    LIGHT lp = m_scene.getDefaultLightParam(type);
    switch(type) {
    case D3DLIGHT_DIRECTIONAL:
      break;
    case D3DLIGHT_POINT      :
      lp.Position = m_pickedRay.m_orig + m_pickedRay.m_dir * 2;
      break;
    case D3DLIGHT_SPOT       :
      lp.Position  = m_pickedRay.m_orig + m_pickedRay.m_dir * 3;
      lp.Direction = (m_scene.getCameraPos() + 5 * m_scene.getCameraDir()) - lp.Position;
      break;
    }
    m_scene.setLightParam(lp);
    m_scene.setLightControlVisible(lp.m_lightIndex, true);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

D3SceneObject *CD3FunctionPlotterDlg::getSelectedVisualObject() {
  if(m_selectedSceneObject == NULL) {
    return NULL;
  }
  switch(m_selectedSceneObject->getType()) {
  case SOTYPE_ANIMATEDOBJECT:
  case SOTYPE_VISUALOBJECT  :
    return m_selectedSceneObject;
  default:
    return NULL;
  }
}

D3AnimatedSurface *CD3FunctionPlotterDlg::getSelectedAnimatedobject() {
  if(m_selectedSceneObject == NULL || m_selectedSceneObject->getType() != SOTYPE_ANIMATEDOBJECT) {
    return NULL;
  }
  return (D3AnimatedSurface*)m_selectedSceneObject;
}

D3LightControl *CD3FunctionPlotterDlg::getSelectedLightControl() {
  if((m_selectedSceneObject == NULL) || (m_selectedSceneObject->getType() != SOTYPE_LIGHTCONTROL)) {
    return NULL;
  }
  return (D3LightControl*)m_selectedSceneObject;
}

const D3LightControl *CD3FunctionPlotterDlg::getSelectedLightControl() const {
  if((m_selectedSceneObject == NULL) || (m_selectedSceneObject->getType() != SOTYPE_LIGHTCONTROL)) {
    return NULL;
  }
  return (D3LightControl*)m_selectedSceneObject;
}

void CD3FunctionPlotterDlg::setLightControlsVisible(bool visible) {
  BitSet visibleLightSet = m_scene.getLightsVisible();
  if(visible) {
    BitSet missing = m_scene.getLightsDefined() - visibleLightSet;
    if(missing.isEmpty()) {
      return;
    }
    for(Iterator<size_t> it = missing.getIterator(); it.hasNext();) {
      m_scene.setLightControlVisible((unsigned int)it.next(), true);
    }
  } else {
    if(visibleLightSet.isEmpty()) {
      return;
    }
    for(Iterator<size_t> it = visibleLightSet.getIterator(); it.hasNext();) {
      m_scene.setLightControlVisible((unsigned int)it.next(), false);
    }
  }
  REPAINT();
}

void CD3FunctionPlotterDlg::setSpotToPointAt(CPoint point) {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL || lc->getLightType() != D3DLIGHT_SPOT) {
    return;
  }
  D3DXVECTOR3 pointInSpace;
  D3SceneObject *obj = m_scene.getPickedObject(point, ~PICK_LIGHTCONTROL, &pointInSpace);
  if(obj == NULL) {
    return;
  }
  LIGHT param = lc->getLightParam();
  param.Direction = unitVector(pointInSpace - param.Position);
  m_scene.setLightParam(param);
}

void CD3FunctionPlotterDlg::onContextMenuSceneObject(CPoint point) {
  switch(m_selectedSceneObject->getType()) {
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_ANIMATEDOBJECT:
    onContextMenuVisualObject(point);
    break;
  case SOTYPE_LIGHTCONTROL:
    onContextMenuLightControl(point);
    break;
  }
}

static void removeMenuCommand(CMenu &menu, int command) {
  menu.RemoveMenu(command, MF_BYCOMMAND);
}

CMenu &CD3FunctionPlotterDlg::loadMenu(CMenu &menu, int id) {
  int ret = menu.LoadMenu(id);
  if(!ret) {
    throwException(_T("Loadmenu(%d) failed"), id);
  }
  return menu;
}

void CD3FunctionPlotterDlg::showContextMenu(CMenu &menu, CPoint point) {
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
}

void CD3FunctionPlotterDlg::onContextMenuBackground(CPoint point) {
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_BACKGROUND);
  int checkedItem = -1;
  switch(m_currentControl) {
  case CONTROL_CAMERA_WALK     : checkedItem = ID_CONTROL_CAMERA_WALK     ; break;
  case CONTROL_CAMERA_KEEPFOCUS: checkedItem = ID_CONTROL_CAMERA_KEEPFOCUS; break;
  }
  if(checkedItem != -1) {
    checkMenuItem(menu, checkedItem, true);
  }
  const int visibleLightCount = (int)m_scene.getLightsVisible().size();
  if(visibleLightCount == 0) {
    removeMenuCommand(menu,ID_HIDE_LIGHTCONTROLS);
  }
  if(visibleLightCount == m_scene.getLightCount()) {
    removeMenuCommand(menu, ID_SHOW_LIGHTCONTROLS);
  }
  showContextMenu(menu, point);
}

void CD3FunctionPlotterDlg::onContextMenuVisualObject(CPoint point) {
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_VISUALOBJECT );

  int checkedItem = -1;
  switch(m_currentControl) {
  case CONTROL_OBJECT_POS      : checkedItem = ID_CONTROL_OBJECT_POS      ; break;
  case CONTROL_OBJECT_SCALE    : checkedItem = ID_CONTROL_OBJECT_SCALE    ; break;
  }
  if(checkedItem != -1) {
    checkMenuItem(menu, checkedItem, true);
  }

  switch(m_selectedSceneObject->getType()) {
  case SOTYPE_VISUALOBJECT  :
    removeMenuCommand(menu, ID_OBJECT_STARTANIMATION   );
    removeMenuCommand(menu, ID_OBJECT_STARTBCKANIMATION);
    removeMenuCommand(menu, ID_OBJECT_STARTALTANIMATION);
    removeMenuCommand(menu, ID_OBJECT_REVERSEANIMATION );
    removeMenuCommand(menu, ID_OBJECT_STOPANIMATION    );
    removeMenuCommand(menu, ID_OBJECT_CONTROL_SPEED    );
    break;

  case SOTYPE_ANIMATEDOBJECT:
    { D3AnimatedSurface *obj = getSelectedAnimatedobject();
      if(obj->isRunning()) {
        removeMenuCommand(menu, ID_OBJECT_STARTANIMATION   );
        removeMenuCommand(menu, ID_OBJECT_STARTBCKANIMATION);
        removeMenuCommand(menu, ID_OBJECT_STARTALTANIMATION);
        switch(obj->getAnimationType()) {
        case ANIMATE_ALTERNATING:
          removeMenuCommand(menu, ID_OBJECT_REVERSEANIMATION );
          break;
        }
      } else {
        removeMenuCommand(menu, ID_OBJECT_REVERSEANIMATION );
        removeMenuCommand(menu, ID_OBJECT_STOPANIMATION    );
        removeMenuCommand(menu, ID_OBJECT_CONTROL_SPEED    );
      }
    }
    break;
  }
  if((m_selectedSceneObject != m_calculatedObject) || (m_calculatedObject->getUserData() == NULL)) {
    removeMenuCommand(menu, ID_OBJECT_EDITFUNCTION);
  }
  showContextMenu(menu, point);
}

void CD3FunctionPlotterDlg::onContextMenuLightControl(CPoint point) {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_LIGHTCONTROL);
  if(m_scene.isLightEnabled(lc->getLightIndex())) { // light is on
    removeMenuCommand(menu, ID_LIGHT_ENSABLE);
  } else {                                          // light is off
    removeMenuCommand(menu, ID_LIGHT_DISABLE      );
    removeMenuCommand(menu, ID_LIGHT_ADJUSTCOLORS );
    removeMenuCommand(menu, ID_LIGHTCONTROL_SPOTAT);
  }
  if(lc->getLightType() != D3DLIGHT_SPOT) {
    removeMenuCommand(menu, ID_LIGHTCONTROL_SPOTAT);
    removeMenuCommand(menu, ID_LIGHT_ADJUSTANGLES );
  }
  showContextMenu(menu, point);
}

void CD3FunctionPlotterDlg::OnControlCameraWalk()      {  setCurrentControl(CONTROL_CAMERA_WALK        ); }

void CD3FunctionPlotterDlg::OnControlObjectPos()       {  setCurrentControl(CONTROL_OBJECT_POS         ); }
void CD3FunctionPlotterDlg::OnControlObjectScale()     {  setCurrentControl(CONTROL_OBJECT_SCALE       ); }

void CD3FunctionPlotterDlg::OnControlObjectKeepFocus() {
  if(moveLastMouseToFocusPoint()) {
    setCurrentControl(CONTROL_OBJECT_POS_KEEPFOCUS);
  }
}

void CD3FunctionPlotterDlg::OnControlObjectScaleKeepFocus() {
  if(moveLastMouseToFocusPoint()) {
    setCurrentControl(CONTROL_OBJECT_SCALE_KEEPFOCUS);
  }
}

bool CD3FunctionPlotterDlg::moveLastMouseToFocusPoint() {
  D3DXVECTOR3 hitPoint;
  D3SceneObject *obj = m_scene.getPickedObject(m_lastMouse, ~PICK_LIGHTCONTROL, &hitPoint);
  if((obj == NULL) || (obj != m_calculatedObject)) {
    return false;
  }

  const D3DXVECTOR3 dp       = obj->getPos() - hitPoint;
  const D3DXVECTOR3 camPos   = m_scene.getCameraPos();
  const float       distance = length(camPos - hitPoint);
  m_focusPoint = camPos + distance * m_scene.getCameraDir();

  m_scene.setObjPos(m_focusPoint + dp);
  return true;
}

void CD3FunctionPlotterDlg::OnControlCameraKeepFocus() {
  D3SceneObject *obj = m_scene.getPickedObject(m_lastMouse, ~PICK_LIGHTCONTROL, &m_focusPoint);
  if(obj == NULL) {
    return;
  }
  m_scene.setCameraLookAt(m_focusPoint);
  setCurrentControl(CONTROL_CAMERA_KEEPFOCUS);
}

void CD3FunctionPlotterDlg::OnObjectControlSpeed() {
  setCurrentControl(CONTROL_ANIMATION_SPEED);
}

void CD3FunctionPlotterDlg::OnControlMoveRotateObject() {
  setSelectedObject(m_calculatedObject);
  setCurrentControl(CONTROL_OBJECT_POS);
}

void CD3FunctionPlotterDlg::setCurrentControl(CurrentObjectControl control) {
  m_currentControl = control;

  switch(m_currentControl) {
  case CONTROL_OBJECT_POS:
    setWindowCursor(this, MAKEINTRESOURCE(OCR_HAND));
    break;
  case CONTROL_CAMERA_KEEPFOCUS:
    showInfo(_T(""));
    // nb continue case
  case CONTROL_CAMERA_WALK     :
    setSelectedObject(NULL);
    setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
    break;
  case CONTROL_SPOTLIGHTPOINT:
    setWindowCursor(this, MAKEINTRESOURCE(OCR_CROSS));
    break;
  default:
    setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
    break;
  }
  show3DInfo();
}

void CD3FunctionPlotterDlg::OnResetPositions() {
  m_scene.initTrans();
}

void CD3FunctionPlotterDlg::OnObjectEditFunction() {
  if((getSelectedVisualObject() != m_calculatedObject) || (m_calculatedObject->getUserData() == NULL)) {
    return;
  }
  PersistentParameter *param = (PersistentParameter*)m_calculatedObject->getUserData();
  switch(param->getType()) {
  case PP_2DFUNCTION:
    OnFilePlotFunction();
    break;
  case PP_ISOSURFACE:
    OnFileIsoSurface();
    break;
  default:
    AfxMessageBox(format(_T("Unknown PersistentParameterType:%d"), param->getType()).cstr());
    break;
  }
}

void CD3FunctionPlotterDlg::OnObjectEditMaterial() {
  if(m_selectedSceneObject && m_selectedSceneObject == m_calculatedObject) {
    m_materialDlgThread->setCurrentDialogProperty(&m_scene.getMaterial());
    m_materialDlgThread->setDialogVisible(true);
    m_lightDlgThread->setDialogVisible(false);
  }
}

void CD3FunctionPlotterDlg::OnObjectResetScale() {
  m_scene.setObjScale(D3DXVECTOR3(1,1,1));
}

void CD3FunctionPlotterDlg::OnObjectStartAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj == NULL) return;
  obj->startAnimation(ANIMATE_FORWARD);
}

void CD3FunctionPlotterDlg::OnObjectStartBckAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj == NULL) return;
  obj->startAnimation(ANIMATE_BACKWARD);
}

void CD3FunctionPlotterDlg::OnObjectStartAltAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj == NULL) return;
  obj->startAnimation(ANIMATE_ALTERNATING);
}

void CD3FunctionPlotterDlg::OnObjectReverseAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if((obj == NULL) || !obj->isRunning()) return;
  switch(obj->getAnimationType()) {
  case ANIMATE_FORWARD    :
    obj->stopAnimation();
    obj->startAnimation(ANIMATE_BACKWARD);
    break;
  case ANIMATE_BACKWARD   :
    obj->stopAnimation();
    obj->startAnimation(ANIMATE_FORWARD);
    break;
  case ANIMATE_ALTERNATING:
    break;
  }
}

void CD3FunctionPlotterDlg::OnObjectStopAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj == NULL) return;
  obj->stopAnimation();
}

void CD3FunctionPlotterDlg::OnObjectRemove() {
  m_scene.removeSceneObject(m_selectedSceneObject);
  if(m_calculatedObject == m_selectedSceneObject) {
    m_calculatedObject = NULL;
  }
  delete m_selectedSceneObject;
  setSelectedObject(NULL);
  REPAINT();
}

void CD3FunctionPlotterDlg::OnObjectInfo() {
  D3DXVECTOR3 pointInSpace;
  D3PickedInfo info;
  D3SceneObject *obj = m_scene.getPickedObject(m_lastMouse, ~PICK_LIGHTCONTROL, &pointInSpace, &info);
  if(obj == NULL) {
    return;
  }
  MessageBox(format(_T("Face:%d, (vertices:(%d,%d,%d)\nPoint:%s")
            , info.m_faceIndex
            , info.m_i1
            , info.m_i2
            , info.m_i3
            , toString(pointInSpace,5).cstr()
            ).cstr()
            ,_T("Info")
            ,MB_ICONINFORMATION);
}

void CD3FunctionPlotterDlg::OnLightEnable() {
  m_scene.setLightEnabled(getSelectedLightControl()->getLightIndex(), true);
  REPAINT();
}

void CD3FunctionPlotterDlg::OnLightDisable() {
  m_scene.setLightEnabled(getSelectedLightControl()->getLightIndex(), false);
  REPAINT();
}

void CD3FunctionPlotterDlg::OnShowLightControls() {
  setLightControlsVisible(true);
}

void CD3FunctionPlotterDlg::OnHideLightControls() {
  setLightControlsVisible(false);
}

void CD3FunctionPlotterDlg::OnLightAdjustColors() {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  m_lightDlgThread->setCurrentDialogProperty(&lc->getLightParam());
  m_lightDlgThread->setDialogVisible(true);
  m_materialDlgThread->setDialogVisible(false);
}

void CD3FunctionPlotterDlg::OnLightAdjustAngles() {
  setCurrentControl(CONTROL_SPOTANGLES);
}

void CD3FunctionPlotterDlg::OnLightControlSpotAt() {
  setCurrentControl(CONTROL_SPOTLIGHTPOINT);
}

void CD3FunctionPlotterDlg::OnLightControlHide() {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  m_scene.setLightControlVisible(lc->getLightIndex(), false);
  REPAINT();
}

void CD3FunctionPlotterDlg::OnLightRemove() {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  const int lightIndex = lc->getLightIndex();
  m_scene.removeLight(lightIndex);
  setSelectedObject(NULL);
  REPAINT();
}

void CD3FunctionPlotterDlg::OnEditBackgroundColor() {
  const D3DCOLOR oldColor = m_scene.getBackgroundColor();
  CColorDlg dlg("Background color", SP_BACKGROUNDCOLOR, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  if(dlg.DoModal() != IDOK) {
    m_scene.setBackgroundColor(oldColor);
    m_scene.render();
  }
  m_currentEditor = NULL;
}

void CD3FunctionPlotterDlg::OnEditAmbientLight() {
  const D3DCOLOR oldColor = m_scene.getGlobalAmbientColor();
  CColorDlg dlg("Ambient color", SP_AMBIENTLIGHT, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  if(dlg.DoModal() != IDOK) {
    m_scene.setGlobalAmbientColor(oldColor);
    m_scene.render();
  }
  m_currentEditor = NULL;
}

void CD3FunctionPlotterDlg::showInfo(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  getInfoPanel()->SetWindowText(msg.cstr());
}

String CD3FunctionPlotterDlg::getSelectedString() const {
  if(m_selectedSceneObject == NULL) {
    return "--";
  } else {
    return m_selectedSceneObject->getName();
  }
}

const TCHAR *controlString(CurrentObjectControl control) {
  switch(control) {
#define caseStr(s) case CONTROL_##s: return _T(#s);
  caseStr(IDLE                   )
  caseStr(CAMERA_WALK            )
  caseStr(OBJECT_POS             )
  caseStr(OBJECT_SCALE           )
  caseStr(OBJECT_POS_KEEPFOCUS   )
  caseStr(OBJECT_SCALE_KEEPFOCUS )
  caseStr(CAMERA_KEEPFOCUS       )
  caseStr(LIGHT                  )
  caseStr(SPOTLIGHTPOINT         )
  caseStr(SPOTANGLES             )
  caseStr(ANIMATION_SPEED        )
  default: return _T("?");
  }
}

void CD3FunctionPlotterDlg::show3DInfo() {
  if(!m_infoVisible) {
    return;
  }
  const String camStr      = m_scene.getCameraString();
  const String objStr      = m_scene.getObjString();
  const String lightStr    = m_scene.getLightString();
  const String materialStr = m_scene.getMaterialString();
  const float  viewAngel   = m_scene.getViewAngel();
  double       zn          = m_scene.getNearViewPlane();
  const String focusStr    = hasFocusPoint() 
                           ? format(_T("Focuspoint:%s\n"), toString(m_focusPoint).cstr())
                           : _T("");
  const String rayStr      = m_pickedRay.isSet() ? format(_T("Picked ray:%s\n"), m_pickedRay.toString().cstr()) : _T("");

  showInfo(_T("Current Motion:%s Selected:%s\n%s View angel:%.1lf, zn:%.3lf\n%s\n%s%s%s\n%s")
           ,controlString(m_currentControl), getSelectedString().cstr()
           ,camStr.cstr(), degrees(viewAngel), zn
           ,objStr.cstr()
           ,focusStr.cstr()
           ,rayStr.cstr()
           ,lightStr.cstr()
           ,materialStr.cstr());
}
