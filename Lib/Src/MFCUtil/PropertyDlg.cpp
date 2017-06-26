#include "pch.h"
#include <MFCUtil/PropertyDialog.h>

void PropertyDialog::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  if(m_showWinActive) return;
  try {
    m_showWinActive = true;
    if(bShow) {
      resetControls();
      reposition();
      setVisible(true);
    } else {
      setVisible(false);
    }
    m_showWinActive = false;
  } catch(...) {
    m_showWinActive = false;
    throw;
  }
}
