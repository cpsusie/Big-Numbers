#include "pch.h"
#include <D3DGraphics/PropertyDialog.h>

void PropertyDialog::OnShowWindow(BOOL bShow, UINT nStatus) {
  CDialog::OnShowWindow(bShow, nStatus);
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
