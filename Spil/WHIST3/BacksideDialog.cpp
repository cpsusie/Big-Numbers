#include "stdafx.h"
#include "whist3.h"
#include <Array.h>
#include "BacksideDialog.h"
#include "CardBitmap.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

BacksideDialog::BacksideDialog(CWnd *pParent) : CDialog(BacksideDialog::IDD, pParent) {
}

void BacksideDialog::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(BacksideDialog, CDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

void BacksideDialog::OnPaint() {
  CPaintDC dc(this);

  for(size_t i = 0; i < m_buttons.size(); i++) {
    m_buttons[i]->Draw();
  }

  // Do not call __super::OnPaint() for painting messages
}

BOOL BacksideDialog::PreTranslateMessage(MSG *pMsg) {
  switch(pMsg->message) {
  case WM_LBUTTONDOWN:
  { for(size_t i = 0; i < m_buttons.size(); i++) {
      CardButton *b = m_buttons[i];
      CRect r;
      b->GetWindowRect(&r);
      if(r.PtInRect(pMsg->pt)) {
        getOptions().m_backside = b->getCard();
        OnOK();
        return true;
      }
    }
  }
  default:
    return __super::PreTranslateMessage(pMsg);
  }
}

int BacksideDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  for(int i = 0, r = 0, c = 0; i < 16; i++) {
    CardButton *but = new CardButton(this,53+i);
    but->Create(c*(CardBitmap::getCardWidth()+2),r*(CardBitmap::getCardHeight()+2));
    m_buttons.add(but);
    c++;
    if(c == 4) {
      r++;
      c = 0;
    }
  }
  return 0;
}

void BacksideDialog::OnDestroy() {
  __super::OnDestroy();

  for(size_t i = 0; i < m_buttons.size(); i++) {
    CardButton *but = m_buttons[i];
    BOOL r = but->DestroyWindow();
    delete m_buttons[i];
  }
}

