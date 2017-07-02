#include "stdafx.h"
#include "Diff.h"
#include "ZoomDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CZoomDlg::CZoomDlg(const String &s1, const String &s2, CWnd *pParent) : m_s1(s1), m_s2(s2), CDialog(CZoomDlg::IDD, pParent) {
  m_lines = format(_T("%s\r\n%s"), s1.cstr(), s2.cstr()).cstr();
  m_ignorecase = FALSE;
}

void CZoomDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_CHECKIGNORECASE, m_ignorecase);
  DDX_Text( pDX, IDC_EDIT2LINES, m_lines);
}

BEGIN_MESSAGE_MAP(CZoomDlg, CDialog)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_CHECKIGNORECASE, OnCheckIgnorecase)
END_MESSAGE_MAP()

BOOL CZoomDlg::OnInitDialog() {
  __super::OnInitDialog();

  try {
    m_diff.compareStrings(m_s1.cstr(), m_s2.cstr(), m_cmp);
  } catch(Exception e) {
    showException(e);
  }
  m_fontScale  = 1;

  m_editBox.substituteControl(this, IDC_EDIT2LINES, m_diff);

  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
  SetIcon(m_hIcon, false);
  createAndSetFont( m_fontScale);

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATORZOOM));
  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_EDIT2LINES     , RELATIVE_WIDTH | RELATIVE_HEIGHT);
  m_layoutManager.addControl(IDC_CHECKIGNORECASE, RELATIVE_Y_POS );
  m_layoutManager.addControl(IDOK               , PCT_RELATIVE_X_CENTER | RELATIVE_Y_POS );

  return TRUE;
}

void CZoomDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

BOOL CZoomDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }

  switch(pMsg->message) {
  case WM_MOUSEWHEEL:
    { const short delta = (short)(pMsg->wParam >> 16);
      const UINT  flags = (UINT)( pMsg->wParam & 0xffff);
      OnMouseWheel(flags, delta, pMsg->pt);
    }
    return true;

  default:
    break;
  }

  return __super::PreTranslateMessage(pMsg);
}

BOOL CZoomDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
#ifdef __NEVER__
  if (nFlags & MK_CONTROL) {
    if(zDelta > 0) {
      scrollRight();
    } else {
      scrollLeft();
    }
  } else
#endif
  if(zDelta > 0) {
    zoomIn();
  } else {
    zoomOut();
  }
  return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CZoomDlg::zoomIn() {
  const double oldScale = m_fontScale;
  m_fontScale = min(oldScale * 1.2, 7);
  if(m_fontScale != oldScale) {
    createAndSetFont(m_fontScale);
  }
}

void CZoomDlg::zoomOut() {
  const double oldScale = m_fontScale;
  m_fontScale = max(oldScale / 1.2, 1);
  if(m_fontScale != oldScale) {
    createAndSetFont(m_fontScale);
  }
}

void CZoomDlg::scrollRight() {
  scrollHorizontal(40);
}
void CZoomDlg::scrollLeft() {
  scrollHorizontal(-40);
}

void CZoomDlg::scrollHorizontal(int n) {
  CEdit *e = (CEdit*)GetDlgItem(IDC_EDIT2LINES);
  LOGFONT lf;
  m_font.GetLogFont(&lf);
  SCROLLINFO info;
  info.cbSize = sizeof(info);
  e->GetScrollInfo(SB_HORZ, &info);
  const int newPos = minMax(info.nPos + n, info.nMin, info.nMax);
  const int amount = newPos - info.nPos;
  if(amount != 0) {
    e->SetScrollPos(SB_HORZ, newPos);
    e->ScrollWindow(-amount,0);
    UpdateWindow();
  }
}

void CZoomDlg::OnCheckIgnorecase() {
  m_ignorecase = !m_ignorecase;
  m_cmp.setIgnoreCase(m_ignorecase? true : false);
  m_diff.compareStrings(m_s1.cstr(), m_s2.cstr(), m_cmp);
  Invalidate();
}

void CZoomDlg::createAndSetFont(double scale) {
  if(m_font.m_hObject) {
    m_font.DeleteObject();
  }
  LOGFONT lf = getOptions().m_logFont;
  lf.lfHeight = (int)(scale * lf.lfHeight);
  m_font.CreateFontIndirect(&lf);
  GetDlgItem(IDC_EDIT2LINES)->SetFont(&m_font);
}
