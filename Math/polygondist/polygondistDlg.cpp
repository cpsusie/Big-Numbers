#include "stdafx.h"
#include <math.h>
#include "polygondist.h"
#include "polygondistDlg.h"
#include <BitSet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


CPolygondistDlg::CPolygondistDlg(CWnd *pParent)
  : CDialog(CPolygondistDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CPolygondistDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPolygondistDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_COMMAND(ID_FILE_EXIT            , OnFileExit            )
    ON_COMMAND(ID_TOOLS_FINDMAXDISTANCE, OnToolsFindmaxdistance)
    ON_COMMAND(ID_HELP_ABOUTPOLYGONDIST, OnHelpAboutpolygondist)
  ON_WM_CLOSE()
  ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CPolygondistDlg::OnInitDialog() {
  CDialog::OnInitDialog();

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

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  return TRUE;
}

void CPolygondistDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

void CPolygondistDlg::OnOK() {
}
void CPolygondistDlg::OnCancel() {
}
void CPolygondistDlg::OnClose() {
  OnFileExit();
}
void CPolygondistDlg::OnFileExit() {
  EndDialog(IDOK);
}
void CPolygondistDlg::OnHelpAboutpolygondist() {
  CAboutDlg dlg;
  dlg.DoModal();
}

BOOL CPolygondistDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CPolygondistDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  clearMax();
  Invalidate();
}

void CPolygondistDlg::OnPaint()  {
  if (IsIconic()) {
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
    CDialog::OnPaint();
    CClientDC dc(GetDlgItem(IDC_STATICPANEL));
    m_poly.paint(dc);
    if(m_maxIndex1 >= 0) {
      const CPoint &p1 = m_poly.point(m_maxIndex1);
      const CPoint &p2 = m_poly.point(m_maxIndex2);
      dc.MoveTo(p1);
      dc.LineTo(p2);
    }
  }
}

HCURSOR CPolygondistDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

CPoint CPolygondistDlg::dlgToPanel(CPoint p) const {
  ClientToScreen(&p);
  GetDlgItem(IDC_STATICPANEL)->ScreenToClient(&p);
  return p;
}

void CPolygondistDlg::showInfo(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  setWindowText(this, IDC_STATICINFO, msg);
}

void CPolygondistDlg::OnLButtonUp(UINT nFlags, CPoint point)  {
  CDialog::OnLButtonUp(nFlags, point);
}

void CPolygondistDlg::OnLButtonDown(UINT nFlags, CPoint point)  {
  point = dlgToPanel(point);
  m_poly.addPoint(point);
  clearMax();
  Invalidate();
  CDialog::OnLButtonDown(nFlags, point);
}

void CPolygondistDlg::OnRButtonDown(UINT nFlags, CPoint point)  {
  point = dlgToPanel(point);
  m_poly.removeNearest(point);
  clearMax();
  Invalidate();
  CDialog::OnRButtonDown(nFlags, point);
}

#define RED   RGB(255,0,0)
#define GREEN RGB(0,255,0)
#define BLUE  RGB(0,0,255)

void CPolygondistDlg::clearMax() {
  m_maxIndex1 = m_maxIndex2 = -1;
}

void CPolygondistDlg::OnToolsFindmaxdistance() {
  const UINT n      = m_poly.getPointCount();
  UINT       index1 = m_poly.findTopPoint();
  UINT       index2 = m_poly.findBottomPoint();
  if(index1 < 0) {
    MessageBox(_T("Ingen punkter i polygon"));
    return;
  }
  CWnd       *panel = GetDlgItem(IDC_STATICPANEL); 
  CClientDC   dc(panel);
  const CRect rect  = getClientRect(panel);

  UINT          nextI1 = (index1 + 1) % n;
  UINT          nextI2 = (index2 + 1) % n;
  const CPoint *p1     = &m_poly.point(index1);
  const CPoint *p2     = &m_poly.point(index2);
  const CPoint *nextP1 = &m_poly.point(nextI1);
  const CPoint *nextP2 = &m_poly.point(nextI2);
  vector        v1(    *p1,CPoint(p1->x+10,p1->y));
  vector        v2(    *p2,CPoint(p2->x-10,p2->y));
  vector        nextV1(*p1,*nextP1);
  vector        nextV2(*p2,*nextP2);
  double        angle1 = angle(v1, nextV1);
  double        angle2 = angle(v2, nextV2);

  line(*p1,v1).paint(dc,rect, RED );
  line(*p2,v2).paint(dc,rect, BLUE);
  UINT max1, max2, maxDist2 = 0;
  for(int i = 0; i < m_poly.getPointCount(); i++) {
    const UINT d2 = dist2(*p1,*p2);
    if (d2 > maxDist2) {
      maxDist2 = d2;
      max1 = index1;
      max2 = index2;
    }
    if(angle1 < angle2) {
      index1 = nextI1; nextI1 = (nextI1 + 1) % n;
      p1     = nextP1; nextP1 = &m_poly.point(nextI1);
      v1     = nextV1; nextV1 = vector(*p1,*nextP1);
      v2     = -v1;
      angle2 -= angle1;
      angle1 = angle(v1,nextV1);
      line(*p1, v1).paint(dc,rect, RED );
      line(*p2, v2).paint(dc,rect, BLUE);
    } else {
      index2 = nextI2; nextI2 = (nextI2 + 1) % n;
      p2     = nextP2; nextP2 = &m_poly.point(nextI2);
      v2     = nextV2; nextV2 = vector(*p2,*nextP2);
      v1     = -v2;
      angle1 -= angle2;
      angle2 = angle(v2, nextV2);

      line(*p1, v1).paint(dc,rect, RED );
      line(*p2, v2).paint(dc,rect, BLUE);
    }
    Sleep(2000);
  }
  m_maxIndex1 = max1;
  m_maxIndex2 = max2;
  Invalidate();

}

void CPolygondistDlg::OnMouseMove(UINT nFlags, CPoint point)  {
  point = dlgToPanel(point);
  const double k = m_poly.pointInside(point);
  showInfo(_T("(%3d,%3d):%lf      "),point.x,point.y, k);
  CDialog::OnMouseMove(nFlags, point);
}
