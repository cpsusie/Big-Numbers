#include "pch.h"
#include "MFCUtil/EditWithSpinButton.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CEditWithSpinButton::CEditWithSpinButton() {
}

CEditWithSpinButton::~CEditWithSpinButton() {
}

BEGIN_MESSAGE_MAP(CEditWithSpinButton, CEdit)
    ON_WM_CREATE()
END_MESSAGE_MAP()

int CEditWithSpinButton::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CEdit::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  return 0;
}
