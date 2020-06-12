#include "stdafx.h"
#include "RollingAvgDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CRollingAvgDlg::CRollingAvgDlg(const RollingAvg &rollingAvg, CWnd *pParent) : CDialog(IDD, pParent), m_enabled(FALSE) {
  m_rollingAvg = rollingAvg;
}

void CRollingAvgDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITROLLINGQUEUESIZE, m_queueSize);
  DDX_Check(pDX, IDC_CHECKENABLEROLLINGAVG, m_enabled);
}

BEGIN_MESSAGE_MAP(CRollingAvgDlg, CDialog)
  ON_NOTIFY(UDN_DELTAPOS, IDC_EDITROLLINGQUEUESIZESPIN, OnDeltaposEditRollingQueueSizeSpin)
  ON_BN_CLICKED(IDC_CHECKENABLEROLLINGAVG             , OnClickedCheckEnableRollingAvg    )
END_MESSAGE_MAP()

BOOL CRollingAvgDlg::OnInitDialog() {
  __super::OnInitDialog();
  paramToWin(m_rollingAvg);
  gotoEditBox(this, IDC_EDITROLLINGQUEUESIZE);
  return FALSE;
}

void CRollingAvgDlg::OnDeltaposEditRollingQueueSizeSpin(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  const int delta = -pNMUpDown->iDelta;

  const String str = getWindowText(this, IDC_EDITROLLINGQUEUESIZE);
  UINT queueSize;
  if(_stscanf(str.cstr(), _T("%u"), &queueSize) != 1) {
    return;
  }
  const UINT newQueueSize = queueSize + delta;
  if(!RollingAvg::isValidQueueSize(newQueueSize)) {
    return;
  }
  setEditValue(this, IDC_EDITROLLINGQUEUESIZE, newQueueSize);
  *pResult = 0;
}

void CRollingAvgDlg::OnClickedCheckEnableRollingAvg() {
  const bool enable = (IsDlgButtonChecked(IDC_CHECKENABLEROLLINGAVG) == BST_CHECKED);
  enableFields(enable);
}

void CRollingAvgDlg::enableFields(bool enable) {
  enableWindowList(*this, enable, IDC_EDITROLLINGQUEUESIZE, IDC_EDITROLLINGQUEUESIZESPIN, 0);
}

void CRollingAvgDlg::paramToWin(const RollingAvg &param) {
  m_enabled   = param.isEnabled();
  m_queueSize = param.getQueueSize();
  enableFields(m_enabled);
  UpdateData(false);
}

void CRollingAvgDlg::winToParam(RollingAvg &param) {
  if(!UpdateData()) {
    return;
  }
  param.setEnabled(  m_enabled);
  param.setQueueSize(m_queueSize);
}

bool CRollingAvgDlg::validate() {
  if(!IsDlgButtonChecked(IDC_CHECKENABLEROLLINGAVG)) {
    return true;
  }
  if(!UpdateData()) {
    return false;
  }
  if(!RollingAvg::isValidQueueSize(m_queueSize)) {
    gotoEditBox(this, IDC_EDITROLLINGQUEUESIZE);
    showWarning(_T("Valid interval:[1..%u]"), RollingAvg::getMaxQueueSize());
    return false;
  }
  return true;
}

void CRollingAvgDlg::OnOK() {
  if(!validate()) {
    return;
  }
  winToParam(m_rollingAvg);
  __super::OnOK();
}

void CRollingAvgDlg::OnCancel() {
  __super::OnCancel();
}
