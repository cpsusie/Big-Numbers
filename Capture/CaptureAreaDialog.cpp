#include "stdafx.h"
#include "CaptureAreaDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCaptureAreaDialog::CCaptureAreaDialog(CWnd *pParent) : CDialog(CCaptureAreaDialog::IDD, pParent) {
}

void CCaptureAreaDialog::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCaptureAreaDialog, CDialog)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CCaptureAreaDialog::OnInitDialog() {
  __super::OnInitDialog();
  m_screenSize = getScreenSize();
  SetWindowPos(NULL,0,0,m_screenSize.cx,m_screenSize.cy,SWP_NOZORDER|SWP_NOREDRAW|SWP_SHOWWINDOW);
  ::setWindowCursor(this, IDC_CROSS);
  m_state    = 0;
  m_lastMark = CPoint(-1,-1);

  return TRUE;
}

void CCaptureAreaDialog::OnLButtonDown(UINT nFlags, CPoint point) {
  if(m_state++ == 0) {
    removeMarkLines();
  }
  CClientDC dc(this);
  SIZE ss;
  ss.cx = ss.cy = 1;
  m_rect.right = m_rect.left   = point.x;
  m_rect.top   = m_rect.bottom = point.y;
  dc.DrawDragRect(&m_rect,ss,NULL,ss);

  __super::OnLButtonDown(nFlags, point);
}

void CCaptureAreaDialog::OnLButtonUp(UINT nFlags, CPoint point) {
  CClientDC dc(this);
  SIZE ss,oss;
  ss.cx = ss.cy = 0;
  oss.cx = 1; oss.cy = 1;
  dc.DrawDragRect(&m_rect,ss,&m_rect,oss);
  __super::OnLButtonUp(nFlags, point);
  OnOK();
}

void CCaptureAreaDialog::OnMouseMove(UINT nFlags, CPoint point) {
  if(m_state == 0) {
    drawMarkLines(point);
  } else if(m_state == 1) {
    CClientDC dc(this);
    CRect newrect;
    SIZE ss;
    newrect = m_rect;
    newrect.right  = point.x;
    newrect.bottom = point.y;
    ss.cx = ss.cy = 1;
    dc.DrawDragRect(&newrect,ss,&m_rect,ss);
    m_rect = newrect;
  }
  __super::OnMouseMove(nFlags, point);
}

void CCaptureAreaDialog::drawMarkLines(const CPoint &p) {
  CClientDC dc(this);
  const CRect r1(    -1, -1, m_screenSize.cx, p.y            );
  const CRect r2(    -1, -1, p.x            , m_screenSize.cy);
  const CRect lastr1(-1, -1, m_screenSize.cx, m_lastMark.y   );
  const CRect lastr2(-1, -1, m_lastMark.x   , m_screenSize.cy);
  const CSize sz(1,1);
  if(p.x >= 0) {
    if(m_lastMark.x >= 0) {
      dc.DrawDragRect(&r1, sz, &lastr1, sz);
      dc.DrawDragRect(&r2, sz, &lastr2, sz);
    } else { // last == empty
      dc.DrawDragRect(&r1, sz, NULL, sz);
      dc.DrawDragRect(&r2, sz, NULL, sz);
    }
  } else if(m_lastMark.x >= 0) {
    const CSize sz0(0,0);
    dc.DrawDragRect(r1, sz0, &lastr1, sz);
    dc.DrawDragRect(r2, sz0, &lastr2, sz);
  }
  m_lastMark = p;
}

void CCaptureAreaDialog::removeMarkLines() {
  drawMarkLines(CPoint(-1,-1));
}
