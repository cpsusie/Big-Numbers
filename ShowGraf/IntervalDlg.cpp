#include "stdafx.h"
#include "IntervalDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIntervalDlg::CIntervalDlg(const RectangleTransformation &tr, CWnd* pParent) : m_tr(tr), CDialog(CIntervalDlg::IDD, pParent) {
    m_minx = m_tr.getFromRectangle().getMinX();
    m_maxx = m_tr.getFromRectangle().getMaxX();
    m_miny = m_tr.getFromRectangle().getMinY();
    m_maxy = m_tr.getFromRectangle().getMaxY();
}

void CIntervalDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITXFROM, m_minx);
    DDX_Text(pDX, IDC_EDITXTO  , m_maxx);
    DDX_Text(pDX, IDC_EDITYFROM, m_miny);
    DDX_Text(pDX, IDC_EDITYTO  , m_maxy);
}

BEGIN_MESSAGE_MAP(CIntervalDlg, CDialog)
    ON_COMMAND(ID_GOTO_XINTERVAL, OnGotoXInterval)
    ON_COMMAND(ID_GOTO_YINTERVAL, OnGotoYInterval)
END_MESSAGE_MAP()

BOOL CIntervalDlg::OnInitDialog() {
  __super::OnInitDialog();
  const TCHAR *fmt = _T("%.3lg");
  setWindowText(this, IDC_EDITXFROM, format(fmt, m_minx));
  setWindowText(this, IDC_EDITXTO  , format(fmt, m_maxx));
  setWindowText(this, IDC_EDITYFROM, format(fmt, m_miny));
  setWindowText(this, IDC_EDITYTO  , format(fmt, m_maxy));

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_INTERVAL));
  OnGotoXInterval();
  return FALSE;
}

void CIntervalDlg::OnOK() {
  UpdateData(TRUE);

  if(m_minx >= m_maxx) {
    OnGotoXInterval();
    MessageBox(_T("Invalid X-interval"), _T("Error"), MB_ICONEXCLAMATION);
    return;
  }
  if(m_miny >= m_maxy) {
    OnGotoYInterval();
    MessageBox(_T("Invalid Y-interval"), _T("Error"), MB_ICONEXCLAMATION);
    return;
  }
  try {
    RectangleTransformation tmp(m_tr);
    tmp.setFromRectangle(getDataRange());
    m_tr = tmp;
    __super::OnOK();
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONEXCLAMATION);
  }
}

Rectangle2D CIntervalDlg::getDataRange() {
  return Rectangle2D(m_minx,m_miny,m_maxx-m_minx,m_maxy-m_miny);
}

BOOL CIntervalDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CIntervalDlg::OnGotoXInterval() {
  gotoEditBox(this, IDC_EDITXFROM);
}

void CIntervalDlg::OnGotoYInterval() {
  gotoEditBox(this, IDC_EDITYFROM);
}
