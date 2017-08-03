#include "stdafx.h"
#include <D3DGraphics/MeshCreators.h>
#include "Function2DSurfaceDlg.h"
#include "ParametricSurfaceDlg.h"
#include "IsoSurfaceDlg.h"
#include "ProfileDlg.h"
#include "D3FunctionPlotterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

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

CD3FunctionPlotterDlg::CD3FunctionPlotterDlg(CWnd *pParent) : CDialog(CD3FunctionPlotterDlg::IDD, pParent) {
  m_hIcon                = theApp.LoadIcon(IDR_MAINFRAME);
  m_infoVisible          = false;
}

void CD3FunctionPlotterDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
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
  ON_COMMAND(ID_FILE_FUNCTIONSURFACE          , OnFileFunctionSurface          )
  ON_COMMAND(ID_FILE_PARAMETRICSURFACE        , OnFileParametricSurface        )
  ON_COMMAND(ID_FILE_ISOSURFACE               , OnFileIsoSurface               )
  ON_COMMAND(ID_FILE_PROFILESURFACE           , OnFileProfileSurface           )
  ON_COMMAND(ID_FILE_READ3DPOINTSFROMFILE     , OnFileRead3DPointsFromFile     )
  ON_COMMAND(ID_FILE_READOBJFILE              , OnFileReadObjFile              )
  ON_COMMAND(ID_FILE_EXIT                     , OnFileExit                     )
  ON_COMMAND(ID_FILE_NEXTTRY                  , OnFileNexttry                  )
  ON_COMMAND(ID_VIEW_SHOW3DINFO               , OnViewShow3dinfo               )
  ON_COMMAND(ID_RESETPOSITIONS                , OnResetPositions               )
  ON_COMMAND(ID_OBJECT_EDITFUNCTION           , OnObjectEditFunction           )
  ON_COMMAND(ID_ADDBOXOBJECT                  , OnAddBoxObject                 )
  ON_MESSAGE(ID_MSG_RENDER                    , OnMsgRender                    )
END_MESSAGE_MAP()

#define REPAINT() Invalidate(FALSE)

BOOL CD3FunctionPlotterDlg::OnInitDialog() {
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
  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATIC_3DPANEL  , RELATIVE_SIZE                  );
  m_layoutManager.addControl(IDC_STATIC_INFOPANEL, RELATIVE_Y_POS | RELATIVE_WIDTH);

  m_scene.init(*get3DWindow());
  m_editor.init(this);
  m_editor.setEnabled(true);

  createInitialObject();

  m_scene.setLightDirection(0, rotate(m_scene.getCameraDir(), m_scene.getCameraRight(), 0.2f));

  setInfoVisible(isMenuItemChecked(this, ID_VIEW_SHOW3DINFO));
#ifdef LOGMEMORY
  m_memlogThread.start();
#endif

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

static Line3D tetraEder[] = {
  Line3D( a,b)
 ,Line3D( a,c)
 ,Line3D( a,d)
 ,Line3D( b,c)
 ,Line3D( b,d)
 ,Line3D( c,d)
 ,Line3D(ab,ac)
 ,Line3D(ac,ad)
 ,Line3D(ad,ab)

 ,Line3D(ab,bc)
 ,Line3D(bc,bd)
 ,Line3D(bd,ab)

 ,Line3D(ac,bc)
 ,Line3D(bc,cd)
 ,Line3D(cd,ac)

 ,Line3D(ad,bd)
 ,Line3D(bd,cd)
 ,Line3D(cd,ad)

};

void CD3FunctionPlotterDlg::createInitialObject() {
  try {
//  D3LineArray *tt = new D3LineArray(m_scene, tetraEder, ARRAYSIZE(tetraEder)); TRACE_NEW(tt);
//  setCalculatedObject(tt);

  createSaddle();
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
  SceneObjectWithMesh *obj = new SceneObjectWithMesh(m_scene, rotateProfile(m_scene, prof, param, true)); TRACE_NEW(obj);
  obj->setName(prof.getDisplayName());
  return obj;
}

void CD3FunctionPlotterDlg::createSaddle() {
  m_function2DSurfaceParam.setName(_T("Saddle"));
  m_function2DSurfaceParam.m_expr        = _T("(x*x-y*y)/2");
  m_function2DSurfaceParam.m_xInterval   =  DoubleInterval(-1,1);
  m_function2DSurfaceParam.m_yInterval   =  DoubleInterval(-1,1);
  m_function2DSurfaceParam.m_pointCount  = 20;
  m_function2DSurfaceParam.m_includeTime = false;
  m_function2DSurfaceParam.m_machineCode = true;
  m_function2DSurfaceParam.m_doubleSided = true;

  setCalculatedObject(&m_function2DSurfaceParam);
}

BOOL CD3FunctionPlotterDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  if(m_editor.PreTranslateMessage(pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CD3FunctionPlotterDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CD3FunctionPlotterDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width() - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
    render(RENDER_ALL);
  }
}

void CD3FunctionPlotterDlg::OnSize(UINT nType, int cx, int cy) {
  if(cx < 200 || cy < 200) {
    return;
  }
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
  m_scene.OnSize();
}

LRESULT CD3FunctionPlotterDlg::OnMsgRender(WPARAM wp, LPARAM lp) {
  if(wp & RENDER_3D) {
    m_scene.render();
  }
  if(wp & RENDER_INFO) {
    if(m_editor.isEnabled()) {
      show3DInfo();
    }
  }
  return 0;
}

HCURSOR CD3FunctionPlotterDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

class D3AnimatedFunctionSurface : public D3AnimatedSurface {
public:
  D3AnimatedFunctionSurface(D3Scene &scene, const MeshArray &ma)
    : D3AnimatedSurface(scene, ma) {
  }
  void modifyContextMenu(CMenu &menu) {
    appendMenuItem(menu, _T("Edit function"), ID_OBJECT_EDITFUNCTION);
  }
};

class D3FunctionSurface : public SceneObjectWithMesh {
public:
  D3FunctionSurface(D3Scene &scene, LPD3DXMESH mesh)
    : SceneObjectWithMesh(scene, mesh)
  {
  }
  void modifyContextMenu(CMenu &menu) {
    appendMenuItem(menu, _T("Edit function"), ID_OBJECT_EDITFUNCTION);
  }
};

void CD3FunctionPlotterDlg::setCalculatedObject(Function2DSurfaceParameters *param) {
  if(param->m_includeTime) {
    D3AnimatedFunctionSurface *obj = new D3AnimatedFunctionSurface(m_scene, createMeshArray(this, m_scene, *param)); TRACE_NEW(obj);
    setCalculatedObject(obj, param);
  } else {
    D3FunctionSurface *obj = new D3FunctionSurface(m_scene, createMesh(m_scene, *param)); TRACE_NEW(obj);
    setCalculatedObject(obj, param);
  }
}
/*
void CD3FunctionPlotterDlg::setCalculatedObject(IsoSurfaceParameters *param) {
  setCalculatedObject(createIsoSurfaceDebugObject(m_scene, *param), param);
}
*/

void CD3FunctionPlotterDlg::setCalculatedObject(ParametricSurfaceParameters *param) {
  if(param->m_includeTime) {
    D3AnimatedFunctionSurface *obj = new D3AnimatedFunctionSurface(m_scene, createMeshArray(this, m_scene, *param)); TRACE_NEW(obj);
    setCalculatedObject(obj, param);
  } else {
    D3FunctionSurface *obj = new D3FunctionSurface(m_scene, createMesh(m_scene, *param)); TRACE_NEW(obj);
    setCalculatedObject(obj, param);
  }
}

void CD3FunctionPlotterDlg::setCalculatedObject(IsoSurfaceParameters *param) {
  if(param->m_includeTime) {
    D3AnimatedFunctionSurface *obj = new D3AnimatedFunctionSurface(m_scene, createMeshArray(this, m_scene, *param)); TRACE_NEW(obj);
    setCalculatedObject(obj, param);
  } else {
    D3FunctionSurface *obj = new D3FunctionSurface(m_scene, createMesh(m_scene, *param)); TRACE_NEW(obj);
    setCalculatedObject(obj, param);
  }
}

void CD3FunctionPlotterDlg::deleteCalculatedObject() {
  D3SceneObject *oldObj = getCalculatedObject();
  if(oldObj) {
    m_scene.removeSceneObject(oldObj);
    SAFEDELETE(oldObj);
  }
}

void CD3FunctionPlotterDlg::setCalculatedObject(D3SceneObject *obj, PersistentData *param) {
  deleteCalculatedObject();
  if(obj) {
    obj->setUserData(param);
    if(param) {
      obj->setName(param->getDisplayName());
    }
    m_scene.addSceneObject(obj);
  }
  m_editor.setCurrentObject(obj);
}

D3SceneObject *CD3FunctionPlotterDlg::getCalculatedObject() const {
  for(Iterator<D3SceneObject*> it = m_scene.getObjectIterator(); it.hasNext();) {
    D3SceneObject *obj = it.next();
    if(obj->getUserData() != NULL) {
      return obj;
    }
  }
  return NULL;
}

static const String stateFileName = _T("c:\\temp\\D3FunctionPlotter.dat");

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

void CD3FunctionPlotterDlg::OnFileFunctionSurface() {
  try {
    CFunction2DSurfaceDlg dlg(m_function2DSurfaceParam);
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_function2DSurfaceParam = dlg.getData();
    setCalculatedObject(&m_function2DSurfaceParam);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFileParametricSurface() {
  try {
    CParametricSurfaceDlg dlg(m_parametricSurfaceParam);
    if(dlg.DoModal() != IDOK) {
      return;
    }
    m_parametricSurfaceParam = dlg.getData();
    setCalculatedObject(&m_parametricSurfaceParam);
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
    m_isoSurfaceParam = dlg.getData();
    setCalculatedObject(&m_isoSurfaceParam);
    REPAINT();
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFileProfileSurface() {
  const String fileName = _T("C:\\mytools\\D3FunctionPlotter\\SAMPLES\\bue.prf");
  try {
    FILE *f = FOPEN(fileName, _T("r"));
    Profile profile(readTextFile(f), fileName);
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

    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrFilter = fileExtensions;
    dlg.m_ofn.lpstrTitle  = _T("Open Vertex file");
    if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
      return;
    } else {
      const String fileName = dlg.m_ofn.lpstrFile;
      SceneObjectWithMesh *obj = new SceneObjectWithMesh(m_scene, createMeshFromVertexFile(m_scene, fileName, true)); TRACE_NEW(obj);
      setCalculatedObject(obj);
      REPAINT();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CD3FunctionPlotterDlg::OnFileReadObjFile() {
  try {
    static const TCHAR *fileExtensions = _T("Obj files (*.obj)\0*.obj\0"
                                            "All files (*.*)\0*.*\0\0");

    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrFilter = fileExtensions;
    dlg.m_ofn.lpstrTitle  = _T("Open Obj file");
    if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
      return;
    } else {
      const String fileName = dlg.m_ofn.lpstrFile;
      SceneObjectWithMesh *obj = new SceneObjectWithMesh(m_scene, createMeshFromObjFile(m_scene, fileName, false)); TRACE_NEW(obj);
      setCalculatedObject(obj);
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
  m_editor.close();
  deleteCalculatedObject();
  m_scene.close();
  EndDialog(IDOK);
}

void CD3FunctionPlotterDlg::OnCancel() {}
void CD3FunctionPlotterDlg::OnOK()     {}

void CD3FunctionPlotterDlg::OnClose() {
  OnFileExit();
}

CPoint CD3FunctionPlotterDlg::get3DPanelPoint(CPoint point, bool screenRelative) const {
  if(!screenRelative) {
    ClientToScreen(&point);
  }
  ((CD3FunctionPlotterDlg*)this)->get3DWindow()->ScreenToClient(&point);
  return point;
}

void CD3FunctionPlotterDlg::OnViewShow3dinfo() {
  setInfoVisible(toggleMenuItem(this, ID_VIEW_SHOW3DINFO));
}

void CD3FunctionPlotterDlg::setInfoVisible(bool visible) {
  const CRect cl = getClientRect(this);
  CRect upperRect, lowerRect;

  if(visible) {
    upperRect = getWindowRect(get3DWindow());
    upperRect.bottom = cl.Height() - 220;
    lowerRect = CRect(upperRect.left, upperRect.bottom, upperRect.right, cl.bottom);
  } else {
    upperRect = cl;
  }

  if(visible) {
    setWindowRect(get3DWindow()  , upperRect);
    setWindowRect(getInfoPanel(), lowerRect);
    getInfoPanel()->ShowWindow(SW_SHOW);
    m_scene.OnSize();
    m_infoVisible = visible;
    show3DInfo();
  } else {
    getInfoPanel()->ShowWindow(SW_HIDE);
    setWindowRect(get3DWindow()  , upperRect);
    m_scene.OnSize();
    m_infoVisible = visible;
  }
}

void CD3FunctionPlotterDlg::OnResetPositions() {
  m_scene.initTrans();
}

void CD3FunctionPlotterDlg::OnObjectEditFunction() {
  D3SceneObject *calcObj = getCalculatedObject();
  if(!calcObj) return;
  PersistentData *param = (PersistentData*)calcObj->getUserData();
  switch(param->getType()) {
  case PP_2DFUNCTION       :
    OnFileFunctionSurface();
    break;
  case PP_PARAMETRICSURFACE:
    OnFileParametricSurface();
    break;
  case PP_ISOSURFACE       :
    OnFileIsoSurface();
    break;
  default:
    AfxMessageBox(format(_T("Unknown PersistentDataType:%d"), param->getType()).cstr());
    break;
  }
}

void CD3FunctionPlotterDlg::OnAddBoxObject() {
  D3DXCube3 cube(D3DXVECTOR3(-1,-1,-1), D3DXVECTOR3(1,1,1));
  const int matIndex = m_scene.addMaterial(D3Scene::getDefaultMaterial());
  SceneObjectSolidBox *box = new SceneObjectSolidBox(m_scene, cube, matIndex); TRACE_NEW(box);
  m_scene.addSceneObject(box);
  render(RENDER_ALL);
}

void CD3FunctionPlotterDlg::show3DInfo() {
  if(!m_infoVisible) return;
  showInfo(_T("%s"), m_editor.toString().cstr());
}

void CD3FunctionPlotterDlg::showInfo(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  setWindowText(getInfoPanel(), msg);
}
