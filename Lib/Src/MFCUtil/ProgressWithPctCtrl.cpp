#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/ProgressWithPctCtrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4244)

CProgressWithPctCtrl::CProgressWithPctCtrl() {
  m_showPct  = true;
}

CProgressWithPctCtrl::~CProgressWithPctCtrl() {
}

BEGIN_MESSAGE_MAP(CProgressWithPctCtrl, CProgressCtrl)
	//{{AFX_MSG_MAP(CProgressWithPctCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

DEFINECLASSNAME(CProgressWithPctCtrl);

void CProgressWithPctCtrl::substituteControl(CWnd *parent, int id) {
  CProgressCtrl *ctrl = (CProgressCtrl*)parent->GetDlgItem(id);
  if(ctrl == NULL) {
    parent->MessageBox(format(_T("%s::%s:Control %d not found"), s_className, __FUNCTION__, id).cstr(), _T("Error"), MB_ICONWARNING);
    return;
  }
  const int      style    = ctrl->GetStyle();
  const int      exStyle  = ctrl->GetExStyle();
  const CRect    rect     = getWindowRect(ctrl);
  const COLORREF barColor = ctrl->GetBarColor();
  const COLORREF bkColor  = ctrl->GetBkColor();
  ctrl->DestroyWindow();
  if(!Create(style, rect, parent, id)) {
    parent->MessageBox(format(_T("%s::%s::Create failed"), s_className, __FUNCTION__).cstr(), _T("Error"), MB_ICONWARNING);
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
