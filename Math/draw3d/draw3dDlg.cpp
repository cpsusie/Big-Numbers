#include "stdafx.h"
#include "draw3d.h"
#include "draw3dDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
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

CDraw3dDlg::CDraw3dDlg(CWnd *pParent) : CDialog(CDraw3dDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CDraw3dDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDraw3dDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_COMMAND(ID_FILE_RESETCAMERA, OnFileResetcamera)
  ON_COMMAND(ID_FILE_EXIT       , OnFileExit       )
  ON_COMMAND(ID_HELP_ABOUTDRAW3D, OnHelpAboutdraw3d)
END_MESSAGE_MAP()

D3DObject generatefloor(int y) {
  D3DObject p;
  int i;
  for(i = -10; i <= 10; i++) {
    p.addPoint(-10,y,i);
    p.addPoint( 10,y,i);
  }
  const int first2 = (int)p.m_mesh.m_points.size();
  for(i = -10; i <= 10; i++) {
    p.addPoint(i,y,-10);
    p.addPoint(i,y, 10);
  }
  for(i = 0; i <= 20; i++) {
    Face f(2);
    f.add(i*2  );
    f.add(i*2+1);
    p.addFace(f);
  }
  for(i = 0; i < 20; i++) {
    Face f(2);
    f.add(first2+i*2  );
    f.add(first2+i*2+1);
    p.addFace(f);
  }
  return p;
}

void initobject(D3DObject &p) {
  p.addPoint(1,1,2); // 0
  p.addPoint(2,1,2); // 1
  p.addPoint(2,2,2); // 2
  p.addPoint(1,2,2); // 3

  p.addPoint(1,1,3); // 4
  p.addPoint(2,1,3); // 5
  p.addPoint(2,2,3); // 6
  p.addPoint(1,2,3); // 7
  Face f(4); // front
  f.add(0);
  f.add(1);
  f.add(2);
  f.add(3);
  p.addFace(f);
  f.clear(-1); // back
  f.add(4);
  f.add(5);
  f.add(6);
  f.add(7);
  p.addFace(f);
  f.clear(-1); // bottom
  f.add(0);
  f.add(4);
  f.add(5);
  f.add(1);
  p.addFace(f);
  f.clear(-1); // top
  f.add(3);
  f.add(7);
  f.add(6);
  f.add(2);
  p.addFace(f);
}

BOOL CDraw3dDlg::OnInitDialog() {
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

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  m_scene.setRect(getClientRect(this, IDC_STATIC3D));
  resetCamera();
  D3DObject p;
  initobject(p);
  m_scene.addObject(p);
//    for(int y = 0; y < 20; y+=4)
//      m_scene.addObject(generatefloor(y));
  m_scene.addObject(generatefloor(0));

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDraw3dDlg::resetCamera() {
  D3DXVECTOR3 dir, up, campos;
  dir.x    = 0; dir.y    = 0; dir.z    =  1;
  up.x     = 0; up.y     = 1; up.z     =  0;
  campos.x = 0, campos.y = 0; campos.z = -4;
  m_scene.setCameraOrientation(dir,up);
  m_scene.setCameraPos(campos);
}

void CDraw3dDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CDraw3dDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
    m_scene.render(CPaintDC(GetDlgItem(IDC_STATIC3D)));
  }
}

HCURSOR CDraw3dDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CDraw3dDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_scene.setRect(getClientRect(this, IDC_STATIC3D));
  Invalidate();
}

BOOL CDraw3dDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CDraw3dDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  m_lastMouse = point;
  __super::OnLButtonDown(nFlags, point);
}

void CDraw3dDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  __super::OnLButtonUp(nFlags, point);
}

void CDraw3dDlg::walkWithCamera(float dist, float angle) {
  D3DXVECTOR3 cameraDir, cameraUp;
  D3DXVECTOR3 cameraPos;

  m_scene.getCameraOrientation(cameraDir, cameraUp);
  m_scene.getCameraPos(cameraPos);
  cameraPos += cameraDir * (float)dist;
  m_scene.setCameraPos(cameraPos);
  cameraDir = rotate(cameraDir, cameraUp, angle);
  m_scene.setCameraOrientation(cameraDir, cameraUp);
}

void CDraw3dDlg::rotateCameraUpDown(float angle) {
  D3DXVECTOR3 cameraDir, cameraUp, cameraRight;

  m_scene.getCameraOrientation(cameraDir, cameraUp, cameraRight);
  cameraDir = rotate(cameraDir, cameraRight, angle);
  cameraUp  = rotate(cameraUp , cameraRight, angle);
  m_scene.setCameraOrientation(cameraDir, cameraUp);
}

void CDraw3dDlg::rotateCameraLeftRight(float angle) {
  D3DXVECTOR3 cameraDir, cameraUp;

  m_scene.getCameraOrientation(cameraDir, cameraUp);
  cameraUp = rotate(cameraUp, cameraDir, angle);
  m_scene.setCameraOrientation(cameraDir, cameraUp);
}

void CDraw3dDlg::sidewalkWithCamera(float up, float right) {
  D3DXVECTOR3 cameraDir, cameraUp, cameraRight;
  D3DXVECTOR3 cameraPos;

  m_scene.getCameraOrientation(cameraDir, cameraUp, cameraRight);
  m_scene.getCameraPos(cameraPos);
  cameraPos += cameraUp * (float)up + cameraRight * (float)right;
  m_scene.setCameraPos(cameraPos);
}

void CDraw3dDlg::OnMouseMove(UINT nFlags, CPoint point) {
  const TCHAR *msg = EMPTYSTRING;
  if(nFlags & MK_LBUTTON) {
    if(nFlags & MK_CONTROL) {
      if(nFlags & MK_SHIFT) {
        rotateCameraLeftRight(float(point.x - m_lastMouse.x) / -100.0f);
        msg = _T("rotateCameraLeftRight");
      } else {
        rotateCameraUpDown(float(point.y - m_lastMouse.y) / 100.0f);
        msg = _T("rotateCameraUpDown");
      }
    } else {
      if(nFlags & MK_SHIFT) {
        sidewalkWithCamera(float(point.y - m_lastMouse.y) / -10.0f,
                           float(point.x - m_lastMouse.x) /  10.0f);
        msg = _T("sidewalkWithCamera");
      } else {
        walkWithCamera(float(point.y - m_lastMouse.y) / -10.0f,
                       float(point.x - m_lastMouse.x) / 300.0f);
        msg = _T("walkWithCamera");
      }
    }
    setWindowText(this, IDC_STATICINFO, msg);
    Invalidate();
    m_lastMouse = point;
  }
  __super::OnMouseMove(nFlags, point);
}


void CDraw3dDlg::OnOK() {
}

void CDraw3dDlg::OnCancel() {
}
void CDraw3dDlg::OnClose() {
  OnFileExit();
}
void CDraw3dDlg::OnFileExit() {
  EndDialog(IDOK);
}
void CDraw3dDlg::OnFileResetcamera() {
  resetCamera();
  Invalidate();
}

void CDraw3dDlg::OnHelpAboutdraw3d() {
  CAboutDlg().DoModal();
}
