#include "stdafx.h"
#include "showgraf.h"
#include "DegreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDegreeDlg::CDegreeDlg(const Point2DArray &pointArray, FunctionPlotter &fp, CWnd* pParent /*=NULL*/) 
: m_fp(fp)
, CDialog(CDegreeDlg::IDD, pParent) {

  for(size_t i = 0; i < pointArray.size(); i++) {
    m_data.add(pointArray[i]);
  }

  m_fit.solve(LSSD, m_data);

    //{{AFX_DATA_INIT(CDegreeDlg)
    m_degree = 0;
    //}}AFX_DATA_INIT
}


void CDegreeDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDegreeDlg)
    DDX_Text(pDX, IDC_EDITDEGREE, m_degree);
    DDV_MinMaxUInt(pDX, m_degree, 0, 30);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDegreeDlg, CDialog)
    //{{AFX_MSG_MAP(CDegreeDlg)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPINDEGREE, OnDeltaposSpinDegree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDegreeDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  setDegree(1);
  gotoEditBox(this, IDC_EDITDEGREE);
  return FALSE;
}

void CDegreeDlg::setDegree(int degree) {
  if(degree > m_fit.getMaxDegree()) {
    degree = m_fit.getMaxDegree();
  }
  m_fit.setDegree(degree);
  m_degree = m_fit.getActualDegree();
  UpdateData(FALSE);

  m_fp.plotFunction( m_fit, m_color);
}

void CDegreeDlg::OnDeltaposSpinDegree(NMHDR* pNMHDR, LRESULT* pResult) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

    const int delta = -pNMUpDown->iDelta;
    int degree = m_degree + delta;
    if(degree < 0) {
      return;
    } else if(degree > m_fit.getMaxDegree()) {
      return;
    }
    setDegree(degree);
    
    *pResult = 0;
}

void CDegreeDlg::OnOK() {
  CDialog::OnOK();
}
