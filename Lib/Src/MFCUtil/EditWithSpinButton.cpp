#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "MFCUtil/EditWithSpinButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEditWithSpinButton::CEditWithSpinButton() {
}

CEditWithSpinButton::~CEditWithSpinButton() {
}

BEGIN_MESSAGE_MAP(CEditWithSpinButton, CEdit)
    //{{AFX_MSG_MAP(CEditWithSpinButton)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CEditWithSpinButton::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CEdit::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  return 0;
}
