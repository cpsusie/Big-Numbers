#include "stdafx.h"
#include "PropertyDialog.h"

void PropertyDialog::OnShowWindow(BOOL bShow, UINT nStatus) {
  CDialog::OnShowWindow(bShow, nStatus);

  if(bShow) {
    resetControls();
    reposition();
    setVisible(true);
  } else {
    setVisible(false);
  }
}
