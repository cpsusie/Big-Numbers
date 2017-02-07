#include "stdafx.h"
#include "TestColorControlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTestColorControlDlg::CTestColorControlDlg(CWnd *pParent) : CDialog(CTestColorControlDlg::IDD, pParent) {
  m_caption      = _T("");
  m_hasBorder    = FALSE;
  m_enabled      = FALSE;
  m_visible      = TRUE;
  m_clientEdge   = FALSE;
  m_modalFrame   = FALSE;
  m_staticEdge   = FALSE;
  m_sunken       = FALSE;
  m_pickingColor = false;
}

void CTestColorControlDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(   pDX, IDC_EDIT_CAPTION    , m_caption   );
  DDX_Check(  pDX, IDC_CHECK_BORDER    , m_hasBorder );
  DDX_Check(  pDX, IDC_CHECK_ENABLED   , m_enabled   );
  DDX_Check(  pDX, IDC_CHECK_VISIBLE   , m_visible   );
  DDX_Check(  pDX, IDC_CHECK_CLIENTEDGE, m_clientEdge);
  DDX_Check(  pDX, IDC_CHECK_MODALFRAME, m_modalFrame);
  DDX_Check(  pDX, IDC_CHECK_STATICEDGE, m_staticEdge);
  DDX_Check(  pDX, IDC_CHECK_SUNKEN    , m_sunken    );
  DDX_Control(pDX, IDC_COLORMAPCTRL    , m_colormap  );
}

BEGIN_MESSAGE_MAP(CTestColorControlDlg, CDialog)
  ON_WM_PAINT()
  ON_BN_CLICKED(IDC_CHECK_BORDER    , OnCheckBorder       )
  ON_EN_CHANGE( IDC_EDIT_CAPTION    , OnChangeEditCaption )
  ON_BN_CLICKED(IDC_BUTTON_PICKCOLOR, OnButtonpickcolor   )
  ON_BN_CLICKED(IDC_CHECK_VISIBLE   , OnCheckVisible      )
  ON_BN_CLICKED(IDC_CHECK_ENABLED   , OnCheckEnabled      )
  ON_BN_CLICKED(IDC_CHECK_CLIENTEDGE, OnCheckClientEdge   )
  ON_BN_CLICKED(IDC_CHECK_MODALFRAME, OnCheckModalFrame   )
  ON_BN_CLICKED(IDC_CHECK_STATICEDGE, OnCheckStaticEdge   )
  ON_BN_CLICKED(IDC_CHECK_SUNKEN    , OnCheckSunken       )
  ON_COMMAND(ID_GOTO_CAPTION        , OnGotoCaption       )
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CTestColorControlDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_TESTCOLORCONTROL_ACCELERATOR));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_COLORMAPCTRL     , RELATIVE_SIZE  );
  m_layoutManager.addControl(IDC_STATIC_COLOR     , RELATIVE_X_POS );
  m_layoutManager.addControl(IDC_BUTTON_PICKCOLOR , RELATIVE_X_POS );
  m_layoutManager.addControl(IDC_CHECK_VISIBLE    , RELATIVE_Y_POS | PCT_RELATIVE_X_CENTER);
  m_layoutManager.addControl(IDC_CHECK_BORDER     , RELATIVE_Y_POS | PCT_RELATIVE_X_CENTER);
  m_layoutManager.addControl(IDC_CHECK_STATICEDGE , RELATIVE_Y_POS | PCT_RELATIVE_X_CENTER);
  m_layoutManager.addControl(IDC_CHECK_ENABLED    , RELATIVE_Y_POS | PCT_RELATIVE_X_CENTER);
  m_layoutManager.addControl(IDC_CHECK_CLIENTEDGE , RELATIVE_Y_POS | PCT_RELATIVE_X_CENTER);
  m_layoutManager.addControl(IDC_CHECK_SUNKEN     , RELATIVE_Y_POS | PCT_RELATIVE_X_CENTER);
  m_layoutManager.addControl(IDC_CHECK_MODALFRAME , RELATIVE_Y_POS | PCT_RELATIVE_X_CENTER);
  m_layoutManager.addControl(IDC_STATIC_CAPTION   , RELATIVE_Y_POS );
  m_layoutManager.addControl(IDC_EDIT_CAPTION     , RELATIVE_Y_POS );

  m_enabled    = m_colormap.IsWindowEnabled();
  m_sunken     = m_colormap.GetSunken();
  m_hasBorder  = m_colormap.GetBorder();
  m_caption    = m_colormap.GetCaption();
  m_clientEdge = m_colormap.GetClientEdge();
  m_staticEdge = m_colormap.GetStaticEdge();
  m_modalFrame = m_colormap.GetModalFrame();
  UpdateData(false);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CTestColorControlDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CTestColorControlDlg::OnPaint() {
  CDialog::OnPaint();
  OnColorchangedColormapctrl();
}

void CTestColorControlDlg::OnColorchangedColormapctrl() {
  CWnd *st = GetDlgItem(IDC_STATIC_COLOR);
  WINDOWPLACEMENT wp;
  st->GetWindowPlacement(&wp);
  CClientDC dc(this);
  dc.FillSolidRect(&wp.rcNormalPosition,m_colormap.GetColor());
}

void CTestColorControlDlg::OnButtonpickcolor() {
  SetCapture();
  m_pickingColor = true;
}

void CTestColorControlDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  CDialog::OnLButtonDown(nFlags, point);
  if(m_pickingColor) {
    HDC hdcScreen = CreateDC(_T("DISPLAY"), NULL, NULL, NULL); 
    ClientToScreen(&point);
    m_colormap.SetColor(GetPixel(hdcScreen,point.x,point.y));
    DeleteDC(hdcScreen);
  }
}

void CTestColorControlDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  CDialog::OnLButtonUp(nFlags, point);
  if(m_pickingColor) {
    ReleaseCapture();
    m_pickingColor = false;
  }
}

void CTestColorControlDlg::OnCheckVisible() {
  UpdateData();
  m_colormap.ShowWindow(m_visible ? SW_SHOW : SW_HIDE);
}

void CTestColorControlDlg::OnCheckEnabled() {
  UpdateData();
  m_colormap.EnableWindow(m_enabled);
}

void CTestColorControlDlg::OnCheckSunken() {
  UpdateData();
  m_colormap.SetSunken(m_sunken);
}

void CTestColorControlDlg::OnCheckBorder() {
  UpdateData();
  m_colormap.SetBorder(m_hasBorder);
}

void CTestColorControlDlg::OnCheckClientEdge() {
  UpdateData();
  m_colormap.SetClientEdge(m_clientEdge);
}

void CTestColorControlDlg::OnCheckStaticEdge() {
  UpdateData();
  m_colormap.SetStaticEdge(m_staticEdge);
}

void CTestColorControlDlg::OnCheckModalFrame() {
  UpdateData();
  m_colormap.SetModalFrame(m_modalFrame);
}

void CTestColorControlDlg::OnChangeEditCaption() {
  UpdateData();
  m_colormap.SetCaption(m_caption);
}

void CTestColorControlDlg::OnGotoCaption() {
  gotoEditBox(this, IDC_EDIT_CAPTION);
}

void CTestColorControlDlg::OnOK() {
}

void CTestColorControlDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);    
}

BEGIN_EVENTSINK_MAP(CTestColorControlDlg, CDialog)
  ON_EVENT(CTestColorControlDlg, IDC_COLORMAPCTRL, 1, CTestColorControlDlg::OnColorchangedColormapctrl, VTS_NONE)
END_EVENTSINK_MAP()


