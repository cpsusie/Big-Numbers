#include "pch.h"
#include <MFCUtil/ProgressWithPctCtrl.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4244)

CProgressWithPctCtrl::CProgressWithPctCtrl() {
  m_showPct  = true;
}

CProgressWithPctCtrl::~CProgressWithPctCtrl() {
}

BEGIN_MESSAGE_MAP(CProgressWithPctCtrl, CProgressCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CProgressWithPctCtrl::substituteControl(CWnd *parent, int id) {
  DEFINEMETHODNAME;
  CProgressCtrl *ctrl = (CProgressCtrl*)parent->GetDlgItem(id);
  if(ctrl == NULL) {
    showError(_T("%s:Control %d not found"), method, id);
    return;
  }
  const int      style    = ctrl->GetStyle();
  const int      exStyle  = ctrl->GetExStyle();
  const CRect    rect     = getWindowRect(ctrl);
  const COLORREF barColor = ctrl->GetBarColor();
  const COLORREF bkColor  = ctrl->GetBkColor();
  ctrl->DestroyWindow();
  if(!Create(style, rect, parent, id)) {
    showError(_T("%s:Create failed"), method);
    return;
  }
  ModifyStyleEx(0, exStyle);
  SetBarColor(barColor);
  SetBkColor(bkColor);
}

void CProgressWithPctCtrl::OnPaint() {
  CProgressCtrl::OnPaint();

  if(m_showPct) {
    CClientDC dc(this);
    int lower, upper;
    GetRange(lower, upper);
    const CRect  cr       = getClientRect(this);
    const int    rangeLen = upper - lower;
    const int    pos      = GetPos();
    const double q = rangeLen ? ((double)(pos - lower) / rangeLen) : 0;

    const String msg      = format(_T("%.0lf%%"), q * 100.0);
    CFont       *font     = GetParent()->GetFont();
    CFont       *oldFont  = dc.SelectObject(font);
    const CSize  textSize = getTextExtent(dc, msg);
    const CPoint tp((cr.Width() - textSize.cx) / 2, (cr.Height() - textSize.cy)/2);
    dc.SelectObject(oldFont);
    textOutTransparentBackground(dc, tp, msg, *font, RGB(0,0,0));
  }
}

void CProgressWithPctCtrl::setShowPercent(bool show) {
  if(show != m_showPct) {
    m_showPct = show;
    if(::IsWindow(m_hWnd)) Invalidate(FALSE);
  }
}
