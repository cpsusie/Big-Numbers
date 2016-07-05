#include "stdafx.h"
#include "IntervalDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CIntervalDlg::CIntervalDlg(const RectangleTransformation &tr, CWnd* pParent) : m_tr(tr), CDialog(CIntervalDlg::IDD, pParent) {
    //{{AFX_DATA_INIT(CIntervalDlg)
    m_minx = m_tr.getFromRectangle().getMinX();
    m_maxx = m_tr.getFromRectangle().getMaxX();
    m_miny = m_tr.getFromRectangle().getMinY();
    m_maxy = m_tr.getFromRectangle().getMaxY();
    //}}AFX_DATA_INIT
}

void CIntervalDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CIntervalDlg)
    DDX_Text(pDX, IDC_EDITXFROM, m_minx);
    DDX_Text(pDX, IDC_EDITXTO  , m_maxx);
    DDX_Text(pDX, IDC_EDITYFROM, m_miny);
    DDX_Text(pDX, IDC_EDITYTO  , m_maxy);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIntervalDlg, CDialog)
    //{{AFX_MSG_MAP(CIntervalDlg)
	ON_COMMAND(ID_GOTO_XINTERVAL, OnGotoXInterval)
	ON_COMMAND(ID_GOTO_YINTERVAL, OnGotoYInterval)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CIntervalDlg::OnInitDialog() {
  UpdateData(FALSE);
	
  CDialog::OnInitDialog();
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_INTERVAL));
  OnGotoXInterval();
  return FALSE;
}

void CIntervalDlg::OnOK() {
  UpdateData(TRUE);

  if(m_minx >= m_maxx) {
    MessageBox(_T("Invalid X-interval"), _T("Error"), MB_ICONEXCLAMATION);
    return;
  }
  if(m_miny >= m_maxy) {
    MessageBox(_T("Invalid Y-interval"), _T("Error"), MB_ICONEXCLAMATION);
    return;
  }
  try {
    RectangleTransformation tmp(m_tr);
    tmp.setFromRectangle(getDataRange());
    m_tr = tmp;
    CDialog::OnOK();
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
  return CDialog::PreTranslateMessage(pMsg);
}

void CIntervalDlg::OnGotoXInterval() {
  CEdit *e = (CEdit*)GetDlgItem(IDC_EDITXFROM);
  e->SetFocus();
  e->SetSel(0,20);
}

void CIntervalDlg::OnGotoYInterval() {
  CEdit *e = (CEdit*)GetDlgItem(IDC_EDITYFROM);
  e->SetFocus();
  e->SetSel(0,20);
}
