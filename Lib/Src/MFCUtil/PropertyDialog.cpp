#include "pch.h"
#include <MFCUtil/PropertyDialog.h>

void PropertyDialog::setVisible(bool visible) {
  setProperty(m_propertyId + PROPDLG_VISIBLE_OFFSET, m_visible, visible);
}

void PropertyDialog::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  if(m_showWinActive) return;
  try {
    m_showWinActive = true;
    if(bShow) {
      resetControls();
      reposition();
      if(!m_hasBeenVisible) {
        m_hasBeenVisible = true;
        setNotifyEnable(true);
      }
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
