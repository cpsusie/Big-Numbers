#include "stdafx.h"
#include "ShwGraph.h"
#include "IntervalDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IntervalDlg::IntervalDlg(CWnd* pParent /*=NULL*/) : CDialog(IntervalDlg::IDD, pParent) {
    m_maxxstr = EMPTYSTRING;
    m_maxystr = EMPTYSTRING;
    m_minxstr = EMPTYSTRING;
    m_minystr = EMPTYSTRING;
    m_autoscaley = FALSE;
}

void IntervalDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_MAXX, m_maxxstr);
    DDX_Text(pDX, IDC_EDIT_MAXY, m_maxystr);
    DDX_Text(pDX, IDC_EDIT_MINX, m_minxstr);
    DDX_Text(pDX, IDC_EDIT_MINY, m_minystr);
    DDX_Check(pDX, IDC_AUTOSCALECHECK, m_autoscaley);
}

BEGIN_MESSAGE_MAP(IntervalDlg, CDialog)
END_MESSAGE_MAP()

BOOL IntervalDlg::OnInitDialog() {
  TCHAR tmp[40];
  
  _stprintf(tmp,_T("%lg"), m_minx);
  m_minxstr = tmp;

  _stprintf(tmp,_T("%lg"), m_maxx);
  m_maxxstr = tmp;

  _stprintf(tmp,_T("%lg"), m_miny);
  m_minystr = tmp;

  _stprintf(tmp,_T("%lg"), m_maxy);
  m_maxystr = tmp;
  UpdateData(FALSE);
    
  CDialog::OnInitDialog();
  return TRUE; 
}

void IntervalDlg::OnOK() {
  double minx,maxx,miny,maxy;
  UpdateData(TRUE);

  if(_stscanf(m_minxstr,_T("%le"),&minx ) != 1 ||
     _stscanf(m_maxxstr,_T("%le"),&maxx ) != 1 ||
     _stscanf(m_minystr,_T("%le"),&miny ) != 1 ||
     _stscanf(m_maxystr,_T("%le"),&maxy ) != 1 
    ) {
    MessageBox(_T("Invalid input"),_T("Error"), MB_ICONEXCLAMATION);
    return;
  }
  if(minx >= maxx) {
    MessageBox(_T("Invalid X-interval"),_T("Error"), MB_ICONEXCLAMATION);
    return;
  }
  if(miny >= maxy) {
    MessageBox(_T("Invalid Y-interval"),_T("Error"), MB_ICONEXCLAMATION);
    return;
  }
  m_minx = minx; m_maxx = maxx;
  m_miny = miny; m_maxy = maxy;

  CDialog::OnOK();
}

