#include "stdafx.h"
#include "showgraf.h"
#include "DegreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDegreeDlg::CDegreeDlg(const Point2DArray &pointArray, FunctionPlotter &fp, CWnd *pParent) 
: m_fp(fp)
, CDialog(CDegreeDlg::IDD, pParent) {

  for(size_t i = 0; i < pointArray.size(); i++) {
    m_data.add(pointArray[i]);
  }

  m_fit.solve(LSSD, m_data);
  m_degree = 0;
}

void CDegreeDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITDEGREE, m_degree);
    DDV_MinMaxUInt(pDX, m_degree, 0, 30);
}

BEGIN_MESSAGE_MAP(CDegreeDlg, CDialog)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPINDEGREE, OnDeltaposSpinDegree)
END_MESSAGE_MAP()

BOOL CDegreeDlg::OnInitDialog() {
  __super::OnInitDialog();

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

  m_fp.plotFunction( m_fit, getColor());
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
