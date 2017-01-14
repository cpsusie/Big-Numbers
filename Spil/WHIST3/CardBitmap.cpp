#include "stdafx.h"
#include "resource.h"
#include "CardBitmap.h"

CBitmap CardBitmap::cardBitmap;
CDC     CardBitmap::cardDC;

#define CARDW 71
#define CARDH 96

#define CARDXOFF(n)  (((n) % 12) * CARDW)
#define CARDYOFF(n)  (((n) / 12) * CARDH)

void CardBitmap::init() {
  if(cardBitmap.m_hObject != NULL) {
    return;
  }
  HDC hdcScreen = getScreenDC();
  cardBitmap.LoadBitmap(IDB_CARDS_BITMAP);
  CDC tmpDC;
  tmpDC.Attach(hdcScreen);
  cardDC.CreateCompatibleDC(&tmpDC);
  cardDC.SelectObject(&cardBitmap);
  DeleteDC(hdcScreen);
}

void CardBitmap::paintCard(CDC &dc, const CPoint &point, int n) { // static
  paintCard(dc, point.x, point.y, n);
}


void CardBitmap::paintCard(CDC &dc, int x, int y, int n) {  // static
  init();
  dc.BitBlt(x,y,CARDW,CARDH,&cardDC,CARDXOFF(n),CARDYOFF(n),SRCCOPY);
}

int CardBitmap::getCardWidth() { // static
  return CARDW;
}

int CardBitmap::getCardHeight() { // static
  return CARDH;
}
