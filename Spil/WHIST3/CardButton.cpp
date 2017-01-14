#include "stdafx.h"
#include "resource.h"
#include <Exception.h>
#include "Cardbutton.h"
#include "CardBitmap.h"

void CardButton::Draw() {
  CClientDC dc(this);
  CardBitmap::paintCard(dc,0,0,m_card);
}

void CardButton::DrawItem(LPDRAWITEMSTRUCT lpdrawitemstruct) {
  Draw();
}

CardButton::CardButton(CDialog *parent, Card card) {
  m_parent = parent;
  m_card   = card;
}

void CardButton::SetPosition(int x, int y, bool redraw) {
  int flags = /*SWP_NOZORDER |*/ SWP_NOSIZE;
  if(!redraw) flags |= SWP_NOREDRAW;
  CBitmapButton::SetWindowPos(&wndTop,x,y,CardBitmap::getCardWidth(),CardBitmap::getCardHeight(),flags);
}

void CardButton::Create(int x, int y) {
  if(!CBitmapButton::Create(_T("")
                           ,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_NOTIFY
                           ,CRect(x,y,x+CardBitmap::getCardWidth(),y+CardBitmap::getCardHeight())
                           ,m_parent
                           ,IDC_CARDBUTTON)) {
    throwException(_T("CBitmapButton::Create failed"));
  }
  SetPosition(x,y,true);
}

void CardButton::setCard(Card card) {
  m_card = card;
  Draw();
}

void CardButton::SetAbove(CWnd *under) {
  if(under == NULL) {
    CardButton::SetWindowPos(&wndBottom,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
  } else {
    CardButton::SetWindowPos(under,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
  }
}

MyCardButton::MyCardButton(CDialog *parent, Card card) : CardButton(parent,card) { 
  m_marked = false; 
  m_pos    = 0;
}

void MyCardButton::SetPosition(int pos, bool redraw) {
  CRect rect;
  m_parent->GetClientRect(&rect);
  m_pos = pos;
  int x = rect.Width() / 2 + (pos - 7) * 15;
  int y = rect.Height() - CardBitmap::getCardHeight() - 2 - (m_marked ? 20 : 0); // husk parantes
  CardButton::SetPosition(x,y,redraw);
}

void MyCardButton::mark() {
  if(!m_marked) {
    m_marked = true;
    SetPosition(m_pos);
  }
}

void MyCardButton::unmark() {
  if(m_marked) {
    m_marked = false;
    SetPosition(m_pos);
  }
}

