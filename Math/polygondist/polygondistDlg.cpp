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

static double angle(const vector &v1, const vector &v2) {
  double a = acos(v1*v2 / v1.length() / v2.length());
  return a * sign(det(v1,v2));
}

static double angle(const line &l1, const line &l2) {
  vector v1(l1.m_p1 - l1.m_p2);
  vector v2(l2.m_p1 - l2.m_p2);
  return angle(v1,v2);
}

void line::paint(CDC &dc, const CRect &rect) const {
  vector normal(m_p2.y - m_p1.y, m_p1.x - m_p2.x );
  int stepx = sign(m_p2.x - m_p1.x);
  int stepy = sign(m_p2.y - m_p1.y);
  if(stepx == 0) { // lodret linie
    dc.MoveTo(CPoint(m_p1.x,rect.top));
    dc.LineTo(CPoint(m_p1.x,rect.bottom));
  } else if(stepy == 0) { // vandret linie
    dc.MoveTo(CPoint(rect.left,m_p1.y));
    dc.LineTo(CPoint(rect.right,m_p1.y));
  } else {
    CPoint p1 = m_p1;
    while(rect.PtInRect(p1)) {
      CPoint pstepx(p1.x+stepx,p1.y);
      CPoint pstepy(p1.x,p1.y+stepy);
      if(abs((pstepx - m_p1) * normal) < abs((pstepy - m_p1) * normal))
        p1 = pstepx;
      else
        p1 = pstepy;
    }
    CPoint p2 = m_p2;
    while(rect.PtInRect(p2)) {
      CPoint pstepx(p2.x-stepx,p2.y);
      CPoint pstepy(p2.x,p2.y-stepy);
      if(abs((pstepx - m_p2) * normal) < abs((pstepy - m_p2) * normal))
        p2 = pstepx;
      else
        p2 = pstepy;
    }
    dc.MoveTo(p1);
    dc.LineTo(p2);
  }
}

double polygon::pointInside(const CPoint &p) const { // 1=inside, -1=outside, 0=edge
  const size_t n = m_points.size();
  if(n < 3) return -1;
  vector v(p,m_points[0]);
  double d = 0;
  for(size_t i = 1; i <= n; i++) {
    const vector vnext(p,m_points[i%n]);
    d += angle(v,vnext);
    v = vnext;
  }
  return (abs(d) > 1) ? 1 : -1; // d always +/- 2PI or 0
}

static void paintCross(CDC &dc, const CPoint &p) {
  dc.MoveTo(p.x-2,p.y-2);
  dc.LineTo(p.x+2,p.y+2);
  dc.MoveTo(p.x-2,p.y+2);
  dc.LineTo(p.x+2,p.y-2);
}

void polygon::paint(CDC &dc) const {
  if(m_points.size() > 0) {
    paintCross(dc,m_points[0]);
//    textOut(dc, m_points[0],format(_T("p0:(%d,%d)"),m_points[0].x,m_points[0].y));
    for(size_t i = 1; i < m_points.size(); i++) {
      paintCross(dc,m_points[i]);
      dc.MoveTo(m_points[i-1]);
      dc.LineTo(m_points[i]);
//    textOut(dc, m_points[i],format(_T("p%d:(%d,%d)"),(int)i, m_points[i].x,m_points[i].y));
    }

    dc.LineTo(m_points[0]);
  }
}

int polygon::findNearest(const CPoint &p) const {
  if(m_points.size() == 0) return -1;
  int current        = 0;
  double currentDist = dist(p,m_points[current]);
  for(size_t i = 1; i < m_points.size(); i++) {
    const double d = dist(p,m_points[i]);
    if(d < currentDist) {
      current = (int)i;
      currentDist = d;
    }
  }
  return current;
}

void polygon::removeNearest(const CPoint &p) {
  const int n = findNearest(p);
  if(n < 0) return;
  m_points.remove(n);
}

class DeterminantComparator : public Comparator<CPoint> {
private:
  const CPoint m_bottomPoint;
public:
  DeterminantComparator(const CPoint &bottomPoint) : m_bottomPoint(bottomPoint) {
  }
  AbstractComparator *clone() const {
    return new DeterminantComparator(m_bottomPoint);
  }
  int compare(const CPoint &p1, const CPoint &p2);
};

int DeterminantComparator::compare(const CPoint &p1, const CPoint &p2) {
  const vector v1(m_bottomPoint, p1);
  const vector v2(m_bottomPoint, p2);
  return sign(det(v2,v1));
}

void polygon::convexHull() {
  const int    lowest      = findBottomPoint();
  const CPoint bottomPoint = m_points[lowest];
  m_points.remove(lowest);
  m_points.sort(DeterminantComparator(bottomPoint));
  PointArray hull;
  hull.add(bottomPoint);
  hull.add(m_points[0]);
  for(size_t i = 1; i < m_points.size(); i++) {
    hull.add(m_points[i]);
    const CPoint &pi = hull.last();
    for(;;) {
      if(hull.size() <= 3) break;
      UINT j = (UINT)hull.size()-2;
      vector v1(hull[j-1],pi);
      vector v2(hull[j-1],hull[j]);
      vector v3(hull[j-1],hull[0]);
      if(sign(det(v2,v1)) != sign(det(v2,v3))) {
        hull.remove(j);
      } else {
        break;
      }
    }
  }
  m_points = hull;
}

void polygon::addPoint(const CPoint &p) {
  m_points.add(p);
  if(m_points.size() <= 3) {
    return;
  }
  convexHull();
}

int polygon::findTopPoint() const {
  if(m_points.size() == 0) return -1;
  int current    = 0;
  int currentTop = m_points[current].y;
  for(size_t i = 1; i < m_points.size(); i++) {
    if(m_points[i].y < currentTop) {
      current = (int)i;
      currentTop = m_points[i].y;
    }
  }
  return current;
}

int polygon::findBottomPoint() const {
  if(m_points.size() == 0) return -1;
  int current    = 0;
  int currentTop = m_points[current].y;
  for(size_t i = 1; i < m_points.size(); i++) {
    if(m_points[i].y > currentTop) {
      current = (int)i;
      currentTop = m_points[i].y;
    }
  }
  return current;
}

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
    ON_BN_CLICKED(IDC_BUTTONFINDMAXDIST, OnButtonfindmaxdist)
    ON_WM_MOUSEMOVE()
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
    CClientDC dc(this);
    m_poly.paint(dc);
  }
}

HCURSOR CPolygondistDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CPolygondistDlg::OnLButtonUp(UINT nFlags, CPoint point)  {
  CDialog::OnLButtonUp(nFlags, point);
}

void CPolygondistDlg::OnLButtonDown(UINT nFlags, CPoint point)  {
  m_poly.addPoint(point);
  Invalidate();
  CDialog::OnLButtonDown(nFlags, point);
}

void CPolygondistDlg::OnRButtonDown(UINT nFlags, CPoint point)  {
  m_poly.removeNearest(point);
  Invalidate();
  CDialog::OnRButtonDown(nFlags, point);
}

void CPolygondistDlg::OnButtonfindmaxdist()  {
  int index1 = m_poly.findTopPoint();
  int index2 = m_poly.findBottomPoint();
  if(index1 < 0) {
    MessageBox(_T("Ingen punkter i polygon"));
    return;
  }
  CClientDC dc(this);
  CRect r = getClientRect(this);

  CPoint p1 = m_poly.point(index1);
  CPoint p2 = m_poly.point(index2);
  line l1(p1,CPoint(p1.x+1,p1.y));
  line l2(p2,CPoint(p2.x-1,p2.y));
  l1.paint(dc,r);
  l2.paint(dc,r);
  vector v1(l1.m_p1,l1.m_p2);
  vector v2(l2.m_p1,l2.m_p2);
  for(int i = 0; i < m_poly.pointCount(); i++) {
    UINT newi1 = (index1 + 1) % m_poly.pointCount();
    UINT newi2 = (index2 + 1) % m_poly.pointCount();
    vector newv1(p1,m_poly.point(newi1));
    vector newv2(p2,m_poly.point(newi2));
    if(angle(v1,newv1) < angle(v2,newv2)) {
      v1 = newv1;
    } else {
      v2 = newv2;
    }
    Sleep(1000);
  }
}

void CPolygondistDlg::OnMouseMove(UINT nFlags, CPoint point)  {
  CClientDC dc(this);

  const double k = m_poly.pointInside(point);
  textOut(dc,1,10,format(_T("(%3d,%3d):%lf      "),point.x,point.y,k));
  CDialog::OnMouseMove(nFlags, point);
}
